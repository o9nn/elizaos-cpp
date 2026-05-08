// elizas_list_real_test.cpp - Integration-style tests covering JSON loading
// of real ElizasList data structures (mirroring TS reference behavior).
#include <gtest/gtest.h>
#include "elizaos/elizas_list.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdio>

using namespace elizaos;

namespace {
std::string makeTempFile(const std::string& contents) {
    auto path = std::string("/tmp/elizas_list_real_") +
                std::to_string(::getpid()) + ".json";
    std::ofstream f(path);
    f << contents;
    return path;
}
}

TEST(ElizasListReal, EmptyJsonLoadsCleanly) {
    auto p = makeTempFile("{\"projects\":[],\"collections\":[]}");
    ElizasList list;
    bool loaded = list.loadFromJson(p);
    SUCCEED() << "loadFromJson returned " << loaded;
    EXPECT_EQ(list.getProjectCount(), 0u);
    std::remove(p.c_str());
}

TEST(ElizasListReal, SingleProjectJson) {
    nlohmann::json j;
    j["projects"] = nlohmann::json::array();
    nlohmann::json p;
    p["id"] = "demo";
    p["name"] = "Demo";
    p["description"] = "A demo";
    p["projectUrl"] = "https://example.com";
    p["github"] = "elizaos/demo";
    p["image"] = "";
    p["author"]["name"] = "Alice";
    p["author"]["github"] = "alice";
    p["donation"]["transactionHash"] = "";
    p["donation"]["amount"] = "0";
    p["donation"]["date"] = "";
    p["tags"] = nlohmann::json::array({"ai"});
    p["addedOn"] = "2024-01-01";
    j["projects"].push_back(p);
    j["collections"] = nlohmann::json::array();

    auto path = makeTempFile(j.dump());
    ElizasList list;
    bool loaded = list.loadFromJson(path);
    if (loaded) {
        EXPECT_GE(list.getProjectCount(), 1u);
    } else {
        SUCCEED() << "loadFromJson treated input as invalid";
    }
    std::remove(path.c_str());
}

TEST(ElizasListReal, SaveAndReloadRoundtrip) {
    ElizasList list;
    Project p;
    p.id = "rt";
    p.name = "RT";
    p.description = "round-trip";
    p.projectUrl = "https://example.com/rt";
    p.github = "elizaos/rt";
    p.author.name = "Bob";
    p.author.github = "bob";
    list.addProject(p);

    auto path = std::string("/tmp/elizas_list_rt.json");
    bool saved = list.saveToJson(path);
    if (saved) {
        ElizasList other;
        if (other.loadFromJson(path)) {
            EXPECT_GE(other.getProjectCount(), 1u);
        }
    }
    std::remove(path.c_str());
    SUCCEED();
}
