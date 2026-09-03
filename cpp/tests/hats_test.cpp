// hats_test.cpp - E2E tests for elizaos::HatsManager and DataSource pipeline.
#include <gtest/gtest.h>
#include "elizaos/hats.hpp"

#include <fstream>
#include <cstdio>
#include <variant>
#include <algorithm>

using namespace elizaos;

namespace {
std::string writeTmp(const std::string& payload, const std::string& suffix) {
    static int n = 0;
    std::string p = "/tmp/hats_test_" + std::to_string(++n) + "_" + suffix;
    std::ofstream f(p);
    f << payload;
    return p;
}
}

TEST(HatsUtils, ParseDataValue) {
    auto a = hats_utils::parseDataValue("42");
    EXPECT_TRUE(std::holds_alternative<int>(a));
    auto b = hats_utils::parseDataValue("3.14");
    EXPECT_TRUE(std::holds_alternative<double>(b));
    auto c = hats_utils::parseDataValue("true");
    EXPECT_TRUE(std::holds_alternative<bool>(c) || std::holds_alternative<std::string>(c));
    auto d = hats_utils::parseDataValue("hello");
    EXPECT_TRUE(std::holds_alternative<std::string>(d));
}

TEST(HatsUtils, DataValueToString) {
    DataValue i = 7;
    EXPECT_FALSE(hats_utils::dataValueToString(i).empty());
    DataValue s = std::string("text");
    EXPECT_NE(hats_utils::dataValueToString(s).find("text"), std::string::npos);
}

TEST(HatsUtils, StatusToString) {
    EXPECT_FALSE(hats_utils::statusToString(HatsStatus::SUCCESS).empty());
    EXPECT_FALSE(hats_utils::statusToString(HatsStatus::ERROR_INVALID_SOURCE).empty());
}

TEST(HatsCsvSource, ConnectAndLoad) {
    auto path = writeTmp("a,b,c\n1,2,3\n4,5,6\n", "ds.csv");
    DataSourceConfig cfg;
    cfg.id = "csv-1";
    cfg.type = DataSourceType::CSV;
    cfg.location = path;
    CsvDataSource ds(cfg);
    EXPECT_EQ(ds.connect(), HatsStatus::SUCCESS);
    EXPECT_TRUE(ds.isConnected());

    DataSet data;
    auto st = ds.loadData(data);
    EXPECT_EQ(st, HatsStatus::SUCCESS);
    EXPECT_GE(data.size(), 1u);

    EXPECT_EQ(ds.disconnect(), HatsStatus::SUCCESS);
    std::remove(path.c_str());
}

TEST(HatsJsonSource, ConnectAndLoad) {
    auto path = writeTmp(R"([{"k":"v"}])", "ds.json");
    DataSourceConfig cfg;
    cfg.id = "json-1";
    cfg.type = DataSourceType::JSON;
    cfg.location = path;
    JsonDataSource ds(cfg);
    EXPECT_EQ(ds.connect(), HatsStatus::SUCCESS);
    DataSet data;
    auto st = ds.loadData(data);
    EXPECT_EQ(st, HatsStatus::SUCCESS);
    std::remove(path.c_str());
}

TEST(HatsManager, RegisterAndUnregister) {
    HatsManager mgr;
    DataSourceConfig cfg;
    cfg.id = "s1";
    cfg.type = DataSourceType::JSON;
    cfg.location = "/tmp/none";
    auto src = std::make_unique<JsonDataSource>(cfg);
    EXPECT_EQ(mgr.registerDataSource(std::move(src)), HatsStatus::SUCCESS);
    EXPECT_TRUE(mgr.isSourceRegistered("s1"));
    EXPECT_EQ(mgr.getRegisteredSourceCount(), 1u);
    EXPECT_NE(mgr.getDataSource("s1"), nullptr);
    EXPECT_EQ(mgr.unregisterDataSource("s1"), HatsStatus::SUCCESS);
    EXPECT_FALSE(mgr.isSourceRegistered("s1"));
}

TEST(HatsManager, GetDataSourceIds) {
    HatsManager mgr;
    DataSourceConfig c1{ "a", DataSourceType::JSON, "/tmp/a", {}, true, std::nullopt };
    DataSourceConfig c2{ "b", DataSourceType::CSV,  "/tmp/b", {}, true, std::nullopt };
    mgr.registerDataSource(std::make_unique<JsonDataSource>(c1));
    mgr.registerDataSource(std::make_unique<CsvDataSource>(c2));
    auto ids = mgr.getDataSourceIds();
    EXPECT_EQ(ids.size(), 2u);
}

TEST(HatsManager, MergeDataSets) {
    HatsManager mgr;
    DataSet a = {{{"k", DataValue{std::string("v1")}}}};
    DataSet b = {{{"k", DataValue{std::string("v2")}}}};
    DataSet out;
    EXPECT_EQ(mgr.mergeDataSets({a, b}, out), HatsStatus::SUCCESS);
    EXPECT_EQ(out.size(), 2u);
}

TEST(HatsProcessor, AddAndClearSteps) {
    DataProcessor p;
    EXPECT_EQ(p.getStepCount(), 0u);
    ProcessingStep s;
    s.operation = ProcessingOperation::FILTER;
    s.condition = [](const DataRecord&) { return true; };
    p.addStep(s);
    EXPECT_EQ(p.getStepCount(), 1u);
    p.clearSteps();
    EXPECT_EQ(p.getStepCount(), 0u);
}

TEST(HatsProcessor, FilterPipeline) {
    DataProcessor p;
    ProcessingStep s;
    s.operation = ProcessingOperation::FILTER;
    s.condition = [](const DataRecord& r) {
        auto it = r.find("keep");
        if (it == r.end()) return false;
        return std::holds_alternative<bool>(it->second) && std::get<bool>(it->second);
    };
    p.addStep(s);
    DataSet input = {
        {{"keep", DataValue{true}},  {"v", DataValue{1}}},
        {{"keep", DataValue{false}}, {"v", DataValue{2}}},
    };
    DataSet output;
    EXPECT_EQ(p.process(input, output), HatsStatus::SUCCESS);
    EXPECT_EQ(output.size(), 1u);
}

TEST(HatsUtils, CreateDataSourceFromConfig) {
    DataSourceConfig cfg;
    cfg.id = "auto";
    cfg.type = DataSourceType::CSV;
    cfg.location = "/tmp/none";
    auto ds = hats_utils::createDataSource(cfg);
    EXPECT_NE(ds, nullptr);
    if (ds) {
        EXPECT_EQ(ds->getId(), "auto");
    }
}
