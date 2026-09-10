#include <gtest/gtest.h>

#include "elizaos/elizas_list.hpp"

#include <atomic>
#include <filesystem>
#include <thread>
#include <vector>

using namespace elizaos;

namespace {
Project makeProject(const std::string& id) {
    Project value;
    value.id = id;
    value.name = "Project " + id;
    value.description = "Concurrent project";
    value.projectUrl = "https://example.com/" + id;
    value.github = "owner/" + id;
    value.author = {"Owner", "owner", std::nullopt};
    value.tags = {"threaded"};
    value.addedOn = "2025-01-01";
    value.metrics = Metrics{1, 0};
    return value;
}

class TempDir {
public:
    TempDir() {
        path = std::filesystem::temp_directory_path() /
               ("elizas_list_comprehensive_" + std::to_string(counter++));
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }
    ~TempDir() { std::filesystem::remove_all(path); }
    std::filesystem::path path;
    static inline std::atomic<int> counter{0};
};
}

TEST(ElizasListComprehensive, ConcurrentRegistrationDiscoveryQueryAndStatistics) {
    TempDir temp;
    ElizasList list(temp.path);
    constexpr int writers = 8;
    constexpr int perWriter = 100;
    std::atomic<bool> start{false};
    std::vector<std::thread> threads;
    for (int writer = 0; writer < writers; ++writer) {
        threads.emplace_back([&, writer] {
            while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
            for (int i = 0; i < perWriter; ++i) {
                const auto id = "p-" + std::to_string(writer) + "-" + std::to_string(i);
                EXPECT_TRUE(list.addProject(makeProject(id)));
                (void)list.getProjectCount();
                (void)list.searchProjects("threaded");
                (void)list.getProjectsByTag("threaded");
                (void)list.getAllTags();
            }
        });
    }
    start.store(true, std::memory_order_release);
    for (auto& thread : threads) thread.join();
    EXPECT_EQ(list.getProjectCount(), static_cast<std::size_t>(writers * perWriter));
}

TEST(ElizasListComprehensive, ConcurrentSnapshotsAndAtomicWritesAreAlwaysLoadable) {
    TempDir temp;
    ElizasList list(temp.path);
    for (int i = 0; i < 100; ++i) ASSERT_TRUE(list.addProject(makeProject("seed-" + std::to_string(i))));
    ASSERT_TRUE(list.saveToJson("snapshot.json"));

    std::atomic<bool> start{false};
    std::atomic<bool> stop{false};
    std::thread writer([&] {
        while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
        for (int round = 0; round < 100; ++round) {
            EXPECT_TRUE(list.saveToJson("snapshot.json"));
        }
        stop.store(true, std::memory_order_release);
    });
    std::thread mutator([&] {
        while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
        for (int i = 0; i < 100; ++i) {
            EXPECT_TRUE(list.addProject(makeProject("new-" + std::to_string(i))));
        }
    });
    std::thread reader([&] {
        while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
        while (!stop.load(std::memory_order_acquire)) {
            ElizasList snapshot(temp.path);
            if (snapshot.loadFromJson("snapshot.json")) {
                EXPECT_GE(snapshot.getProjectCount(), 100U);
            }
        }
    });
    start.store(true, std::memory_order_release);
    writer.join();
    mutator.join();
    reader.join();

    ElizasList finalSnapshot(temp.path);
    ASSERT_TRUE(finalSnapshot.loadFromJson("snapshot.json"));
    EXPECT_GE(finalSnapshot.getProjectCount(), 100U);
}

TEST(ElizasListComprehensive, ConcurrentRemoveAndReadHaveNoDanglingReferences) {
    TempDir temp;
    ElizasList list(temp.path);
    for (int i = 0; i < 500; ++i) ASSERT_TRUE(list.addProject(makeProject("item-" + std::to_string(i))));
    std::atomic<bool> start{false};
    std::thread remover([&] {
        while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
        for (int i = 0; i < 500; ++i) EXPECT_TRUE(list.removeProject("item-" + std::to_string(i)));
    });
    std::thread reader([&] {
        while (!start.load(std::memory_order_acquire)) std::this_thread::yield();
        for (int round = 0; round < 500; ++round) {
            const auto value = list.getProject("item-250");
            if (value) { EXPECT_EQ(value->id, "item-250"); }
            for (const auto& project : list.getAllProjects()) EXPECT_FALSE(project.id.empty());
        }
    });
    start.store(true, std::memory_order_release);
    remover.join();
    reader.join();
    EXPECT_EQ(list.getProjectCount(), 0U);
}
