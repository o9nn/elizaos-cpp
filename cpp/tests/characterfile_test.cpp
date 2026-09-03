// characterfile_test.cpp - E2E tests for CharacterFileLoader and helpers.
#include <gtest/gtest.h>
#include "elizaos/characterfile.hpp"
#include <cstdio>
#include <algorithm>

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
