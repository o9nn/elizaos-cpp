// characterfile_test.cpp - E2E tests for CharacterFileLoader and helpers.
#include <gtest/gtest.h>
#include "elizaos/characterfile.hpp"
#include <cstdio>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace elizaos;

TEST(ValidationResult, AccumulatesErrorsAndWarnings) {
    ValidationResult v;
    v.addError("missing-name");
    v.addWarning("deprecated-field");
    EXPECT_EQ(v.errors.size(), 1u);
    EXPECT_EQ(v.warnings.size(), 1u);
    EXPECT_FALSE(v.getSummary().empty());
}

TEST(CharacterFileFormat, Defaults) {
    CharacterFileFormat f;
    EXPECT_FALSE(f.version.empty());
    EXPECT_FALSE(f.formatType.empty());
}

class CharacterFileLoaderTest : public ::testing::Test {
protected:
    CharacterFileLoader loader;
};

TEST_F(CharacterFileLoaderTest, ExportToJsonRoundtripStable) {
    CharacterProfile cp("Alice \"Autonomy\"", "Test character with newline\nand slash \\");
    cp.id = "alice-autonomy";
    cp.creator = "e2e-suite";
    auto json = loader.exportToJson(cp);
    EXPECT_NE(json.find("\\\"Autonomy\\\""), std::string::npos);
    auto loaded = loader.loadFromJson(json);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->name, cp.name);
    EXPECT_EQ(loaded->description, cp.description);
    EXPECT_EQ(loaded->id, cp.id);
    EXPECT_EQ(loaded->creator, cp.creator);
}

TEST_F(CharacterFileLoaderTest, SaveAndLoadFile) {
    CharacterProfile cp("Bob", "Test");
    cp.id = "bob-roundtrip";
    cp.creator = "characterfile-test";
    auto path = std::string("/tmp/cf_test_bob.json");
    ASSERT_TRUE(loader.saveToFile(cp, path));
    auto loaded = loader.loadFromFile(path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->name, cp.name);
    EXPECT_EQ(loaded->description, cp.description);
    EXPECT_EQ(loaded->id, cp.id);
    EXPECT_EQ(loaded->creator, cp.creator);
    std::remove(path.c_str());
}

TEST_F(CharacterFileLoaderTest, IsCharacterFileExtensions) {
    EXPECT_TRUE(loader.isCharacterFile("/tmp/x.json"));
    auto exts = loader.getSupportedExtensions();
    EXPECT_FALSE(exts.empty());
}

TEST_F(CharacterFileLoaderTest, ValidateInvalidJsonHandled) {
    auto v = loader.validateJson("{not json");
    EXPECT_FALSE(v.isValid);
    EXPECT_FALSE(v.errors.empty());
}

TEST_F(CharacterFileLoaderTest, StatisticsAvailable) {
    auto s = loader.getStatistics();
    EXPECT_NO_THROW(loader.setStrictValidation(false));
    EXPECT_NO_THROW(loader.setStrictValidation(true));
    SUCCEED() << "stats keys=" << s.size();
}

TEST(CharacterFileTemplate, BasicAndDetailed) {
    auto basic = CharacterFileTemplate::createBasicTemplate();
    auto detailed = CharacterFileTemplate::createDetailedTemplate();
    EXPECT_FALSE(basic.empty());
    EXPECT_FALSE(detailed.empty());
    auto types = CharacterFileTemplate::getTemplateTypes();
    EXPECT_FALSE(types.empty());
}

TEST(CharacterFileUtils, SanitizationAndIds) {
    auto id = CharacterFileUtils::generateCharacterId();
    EXPECT_FALSE(id.empty());
    EXPECT_TRUE(CharacterFileUtils::isValidCharacterId(id));

    auto fn = CharacterFileUtils::sanitizeFilename("My Char/Name?");
    EXPECT_NE(fn.find('/'), 0u); // no leading slash
    EXPECT_FALSE(CharacterFileUtils::getFileExtension().empty());
    EXPECT_FALSE(CharacterFileUtils::createFilename("Alice").empty());
    EXPECT_FALSE(CharacterFileUtils::formatDisplayName("alice_smith").empty());
    EXPECT_FALSE(CharacterFileUtils::getCurrentTimestamp().empty());
}

TEST(CharacterFileConvenience, LoadAndSave) {
    CharacterProfile cp("Conv", "");
    auto path = std::string("/tmp/cf_conv.json");
    EXPECT_TRUE(saveCharacterToFile(cp, path));
    auto v = validateCharacterFile(path);
    SUCCEED() << "valid=" << v.isValid;
    auto loaded = loadCharacterFromFile(path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->name, cp.name);
    std::remove(path.c_str());
}


namespace {

class TemporaryCharacterDirectory {
public:
    TemporaryCharacterDirectory() {
        static std::atomic<unsigned long long> sequence{0};
        path_ = std::filesystem::temp_directory_path() /
            ("eliza-characterfile-" + std::to_string(sequence.fetch_add(1)) + "-" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(path_);
    }

    ~TemporaryCharacterDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path_, error);
    }

    const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

void writeCharacter(const std::filesystem::path& path,
                    const std::string& name,
                    const std::string& description = "watch test") {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(output.is_open());
    output << "{\"name\":\"" << name << "\",\"description\":\""
           << description << "\",\"id\":\"char-watch\"}";
    ASSERT_TRUE(output.good());
}

template <typename Predicate>
bool waitUntil(Predicate predicate, std::chrono::milliseconds timeout = std::chrono::seconds(3)) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return predicate();
}

std::string statistic(const JsonValue& statistics, const std::string& key) {
    return std::any_cast<std::string>(statistics.at(key));
}

} // namespace

TEST(CharacterFileLoaderValidation, RejectsTrailingAndTruncatedContentTransactionally) {
    CharacterFileLoader loader;
    const auto valid = loader.loadFromJson(R"({"name":"Stable","description":"good"})");
    ASSERT_TRUE(valid.has_value());

    EXPECT_FALSE(loader.loadFromJson(R"({"name":"Broken","description":"bad")").has_value());
    EXPECT_FALSE(loader.loadFromJson(R"({"name":"Broken","description":"bad"} garbage)").has_value());
    EXPECT_FALSE(loader.loadFromJson(R"({"name":"Broken","description":"bad",})").has_value());
    EXPECT_FALSE(loader.loadFromJson(R"({"name":"Broken","description":[1,]})").has_value());
    EXPECT_EQ(valid->name, "Stable");
}

TEST(CharacterFileLoaderValidation, CustomSchemaFailureDoesNotPoisonNextLoad) {
    CharacterFileLoader loader;
    JsonValue schema;
    schema["name"] = std::string{};
    schema["description"] = std::string{};
    schema["creator"] = std::string{};
    loader.setValidationSchema(schema);

    EXPECT_FALSE(loader.loadFromJson(R"({"name":"No Creator","description":"bad"})").has_value());
    const auto valid = loader.loadFromJson(
        R"({"name":"Complete","description":"good","creator":"suite"})");
    ASSERT_TRUE(valid.has_value());
    EXPECT_EQ(valid->name, "Complete");
}

TEST(CharacterFileManagerWatcher, ValidatesPathAndInterval) {
    CharacterFileManager manager;
    EXPECT_FALSE(manager.setWatchInterval(std::chrono::milliseconds::zero()));
    EXPECT_FALSE(manager.setWatchInterval(std::chrono::milliseconds(-1)));
    EXPECT_FALSE(manager.watchDirectory(""));
    EXPECT_FALSE(manager.watchDirectory("/definitely/not/a/character-directory"));

    TemporaryCharacterDirectory directory;
    EXPECT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(10)));
    EXPECT_TRUE(manager.watchDirectory(directory.path().string(), false));
    EXPECT_EQ(statistic(manager.getOperationStatistics(), "isWatching"), "true");
    manager.stopWatching();
    EXPECT_EQ(statistic(manager.getOperationStatistics(), "isWatching"), "false");
}

TEST(CharacterFileManagerWatcher, InvalidRestartLeavesExistingWatcherRunning) {
    TemporaryCharacterDirectory directory;
    CharacterFileManager manager;
    ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(10)));
    std::atomic<int> callbacks{0};
    manager.setChangeCallback([&](const CharacterFileChangeEvent&) { ++callbacks; });
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));

    EXPECT_FALSE(manager.watchDirectory((directory.path() / "missing").string(), false));
    EXPECT_EQ(statistic(manager.getOperationStatistics(), "isWatching"), "true");
    writeCharacter(directory.path() / "still-watched.json", "Still Watched");
    ASSERT_TRUE(waitUntil([&] { return callbacks.load() == 1; }));
    manager.stopWatching();
}

TEST(CharacterFileManagerWatcher, ReportsCreateModifyDeleteAndDeduplicatesRapidWrites) {
    TemporaryCharacterDirectory directory;
    const auto file = directory.path() / "watched.json";
    CharacterFileManager manager;
    ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(20)));

    std::mutex eventsMutex;
    std::vector<CharacterFileChangeEvent> events;
    manager.setChangeCallback([&](const CharacterFileChangeEvent& event) {
        std::lock_guard<std::mutex> lock(eventsMutex);
        events.push_back(event);
    });
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));

    writeCharacter(file, "Created");
    ASSERT_TRUE(waitUntil([&] {
        std::lock_guard<std::mutex> lock(eventsMutex);
        return events.size() >= 1;
    }));
    {
        std::lock_guard<std::mutex> lock(eventsMutex);
        ASSERT_EQ(events.size(), 1u);
        EXPECT_EQ(events[0].type, CharacterFileChangeType::Created);
        EXPECT_EQ(std::filesystem::path(events[0].path), file);
    }

    writeCharacter(file, "First rapid update");
    writeCharacter(file, "Second rapid update with different length");
    writeCharacter(file, "Final rapid update");
    ASSERT_TRUE(waitUntil([&] {
        std::lock_guard<std::mutex> lock(eventsMutex);
        return events.size() >= 2;
    }));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lock(eventsMutex);
        ASSERT_EQ(events.size(), 2u);
        EXPECT_EQ(events[1].type, CharacterFileChangeType::Modified);
    }

    ASSERT_TRUE(std::filesystem::remove(file));
    ASSERT_TRUE(waitUntil([&] {
        std::lock_guard<std::mutex> lock(eventsMutex);
        return events.size() >= 3;
    }));
    manager.stopWatching();
    {
        std::lock_guard<std::mutex> lock(eventsMutex);
        ASSERT_EQ(events.size(), 3u);
        EXPECT_EQ(events[2].type, CharacterFileChangeType::Deleted);
    }
}

TEST(CharacterFileManagerWatcher, CallbackCanQueryReplaceItselfAndThrow) {
    TemporaryCharacterDirectory directory;
    CharacterFileManager manager;
    ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(10)));
    std::atomic<int> firstCallbackCount{0};
    std::atomic<int> replacementCallbackCount{0};

    manager.setChangeCallback([&](const CharacterFileChangeEvent&) {
        ++firstCallbackCount;
        EXPECT_EQ(statistic(manager.getOperationStatistics(), "isWatching"), "true");
        manager.setChangeCallback([&](const CharacterFileChangeEvent&) {
            ++replacementCallbackCount;
            throw std::runtime_error("intentional callback failure");
        });
    });
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));
    writeCharacter(directory.path() / "one.json", "One");
    ASSERT_TRUE(waitUntil([&] { return firstCallbackCount.load() == 1; }));
    writeCharacter(directory.path() / "two.json", "Two");
    ASSERT_TRUE(waitUntil([&] { return replacementCallbackCount.load() == 1; }));
    writeCharacter(directory.path() / "three.json", "Three");
    ASSERT_TRUE(waitUntil([&] { return replacementCallbackCount.load() == 2; }));
    manager.stopWatching();
}

TEST(CharacterFileManagerWatcher, CallbackCanStopAndManagerCanRestart) {
    TemporaryCharacterDirectory directory;
    CharacterFileManager manager;
    ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(10)));
    std::atomic<int> callbacks{0};
    manager.setChangeCallback([&](const CharacterFileChangeEvent&) {
        ++callbacks;
        manager.stopWatching();
    });
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));
    writeCharacter(directory.path() / "first.json", "First");
    ASSERT_TRUE(waitUntil([&] { return callbacks.load() == 1; }));
    ASSERT_TRUE(waitUntil([&] {
        return statistic(manager.getOperationStatistics(), "isWatching") == "false";
    }));
    writeCharacter(directory.path() / "ignored.json", "Ignored");
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    EXPECT_EQ(callbacks.load(), 1);

    manager.setChangeCallback([&](const CharacterFileChangeEvent&) { ++callbacks; });
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));
    writeCharacter(directory.path() / "restart.json", "Restart");
    ASSERT_TRUE(waitUntil([&] { return callbacks.load() == 2; }));
    manager.stopWatching();
}

TEST(CharacterFileManagerWatcher, StopAndDestructionPreventLaterCallbacks) {
    TemporaryCharacterDirectory directory;
    const auto file = directory.path() / "lifetime.json";
    std::atomic<int> callbacks{0};
    {
        CharacterFileManager manager;
        ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(10)));
        manager.setChangeCallback([&](const CharacterFileChangeEvent&) {
            ++callbacks;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        });
        ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));
        writeCharacter(file, "Before stop");
        ASSERT_TRUE(waitUntil([&] { return callbacks.load() == 1; }));
        manager.stopWatching();
        writeCharacter(file, "After stop");
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        EXPECT_EQ(callbacks.load(), 1);
    }
    writeCharacter(file, "After destruction");
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    EXPECT_EQ(callbacks.load(), 1);
}

TEST(CharacterFileManagerWatcher, MalformedModifyRollsBackAutoImport) {
    TemporaryCharacterDirectory directory;
    const auto file = directory.path() / "auto.json";
    auto characterManager = std::make_shared<CharacterManager>();
    CharacterFileManager manager;
    manager.setCharacterManager(characterManager);
    ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(10)));
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), true));

    writeCharacter(file, "Stable");
    ASSERT_TRUE(waitUntil([&] {
        const auto character = characterManager->getCharacter("char-watch");
        return character && character->name == "Stable";
    }));

    {
        std::ofstream malformed(file, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(malformed.is_open());
        malformed << R"({"name":"Corrupt","description":)";
    }
    ASSERT_TRUE(waitUntil([&] {
        return std::stoull(statistic(manager.getOperationStatistics(), "errorCount")) >= 1;
    }));
    const auto retained = characterManager->getCharacter("char-watch");
    ASSERT_TRUE(retained.has_value());
    EXPECT_EQ(retained->name, "Stable");
    manager.stopWatching();
}

TEST(CharacterFileManagerWatcher, ConcurrentStatisticsQueriesDuringChanges) {
    TemporaryCharacterDirectory directory;
    const auto file = directory.path() / "concurrent.json";
    CharacterFileManager manager;
    ASSERT_TRUE(manager.setWatchInterval(std::chrono::milliseconds(5)));
    std::atomic<int> callbacks{0};
    manager.setChangeCallback([&](const CharacterFileChangeEvent&) { ++callbacks; });
    ASSERT_TRUE(manager.watchDirectory(directory.path().string(), false));

    std::atomic<bool> done{false};
    std::thread reader([&] {
        while (!done.load()) {
            const auto stats = manager.getOperationStatistics();
            EXPECT_TRUE(stats.find("isWatching") != stats.end());
            EXPECT_TRUE(stats.find("errorCount") != stats.end());
        }
    });
    for (int index = 0; index < 30; ++index) {
        writeCharacter(file, "Concurrent " + std::to_string(index));
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }
    ASSERT_TRUE(waitUntil([&] { return callbacks.load() > 0; }));
    manager.stopWatching();
    done = true;
    reader.join();
    EXPECT_EQ(statistic(manager.getOperationStatistics(), "isWatching"), "false");
}
