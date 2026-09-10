// hats_test.cpp - rigorous tests for HATS parsing, processing, and management.
#include <gtest/gtest.h>

#include "elizaos/hats.hpp"

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <future>
#include <stdexcept>
#include <thread>
#include <variant>

using namespace elizaos;

namespace {

class TemporaryFile {
public:
    TemporaryFile(const std::string& payload, const std::string& suffix) {
        static std::atomic<unsigned int> sequence{0};
        path_ = (std::filesystem::temp_directory_path() /
                 ("hats_test_" + std::to_string(++sequence) + suffix)).string();
        std::ofstream file(path_, std::ios::binary);
        file << payload;
        if (!file) {
            throw std::runtime_error("could not create HATS test file");
        }
    }

    ~TemporaryFile() { std::remove(path_.c_str()); }
    const std::string& path() const { return path_; }

private:
    std::string path_;
};

DataSourceConfig configFor(const std::string& id, DataSourceType type,
                           const std::string& path) {
    return DataSourceConfig{id, type, path, {}, true, std::nullopt};
}

DataSet sentinelData() {
    return DataSet{{{"sentinel", DataValue{std::string("unchanged")}}}};
}

template <typename T>
const T& valueAt(const DataRecord& record, const std::string& key) {
    return std::get<T>(record.at(key));
}

ProcessingStep transformStep(
    std::unordered_map<std::string, std::any> parameters) {
    ProcessingStep step;
    step.operation = ProcessingOperation::TRANSFORM;
    step.parameters = std::move(parameters);
    return step;
}

class InstrumentedSource final : public DataSource {
public:
    InstrumentedSource(DataSourceConfig config, DataSet data,
                       std::function<void()> callback = {})
        : DataSource(config), data_(std::move(data)), callback_(std::move(callback)) {}

    HatsStatus connect() override {
        if (callback_) {
            callback_();
        }
        connected_.store(true);
        return HatsStatus::SUCCESS;
    }

    HatsStatus disconnect() override {
        if (callback_) {
            callback_();
        }
        connected_.store(false);
        return HatsStatus::SUCCESS;
    }

    HatsStatus loadData(DataSet& data) override {
        if (callback_) {
            callback_();
        }
        data = data_;
        return HatsStatus::SUCCESS;
    }

    bool isConnected() const override { return connected_.load(); }

private:
    DataSet data_;
    std::function<void()> callback_;
    std::atomic<bool> connected_{false};
};

} // namespace

TEST(HatsUtils, ParsesAndFormatsEveryDataType) {
    EXPECT_EQ(valueAt<int>({{"v", hats_utils::parseDataValue(" 42 ")}}, "v"), 42);
    EXPECT_DOUBLE_EQ(valueAt<double>({{"v", hats_utils::parseDataValue("3.25")}}, "v"),
                     3.25);
    EXPECT_TRUE(valueAt<bool>({{"v", hats_utils::parseDataValue("TRUE")}}, "v"));
    EXPECT_EQ(valueAt<std::string>({{"v", hats_utils::parseDataValue(" text ")}}, "v"),
              "text");
    EXPECT_EQ(hats_utils::dataValueToString(DataValue{7}), "7");
    EXPECT_EQ(hats_utils::dataValueToString(DataValue{2.5}), "2.500000");
    EXPECT_EQ(hats_utils::dataValueToString(DataValue{false}), "false");
    EXPECT_EQ(hats_utils::dataValueToString(DataValue{std::string("x")}), "x");
}

TEST(HatsUtils, CreatesOnlyActiveSupportedConfiguredSources) {
    TemporaryFile file("{}", ".json");
    EXPECT_NE(hats_utils::createDataSource(configFor("json", DataSourceType::JSON,
                                                     file.path())),
              nullptr);
    EXPECT_NE(hats_utils::createDataSource(configFor("csv", DataSourceType::CSV,
                                                     file.path())),
              nullptr);

    auto invalid = configFor("", DataSourceType::JSON, file.path());
    EXPECT_EQ(hats_utils::createDataSource(invalid), nullptr);
    invalid = configFor("inactive", DataSourceType::JSON, file.path());
    invalid.isActive = false;
    EXPECT_EQ(hats_utils::createDataSource(invalid), nullptr);
    invalid = configFor("xml", DataSourceType::XML, file.path());
    EXPECT_EQ(hats_utils::createDataSource(invalid), nullptr);
}

TEST(HatsJsonSource, SupportsObjectArrayAndJsonLinesModesWithExactTypes) {
    TemporaryFile objectFile(R"({"escaped":"line\n\"quote\"","i":7,"d":2.5,"b":true})",
                             ".json");
    auto objectConfig = configFor("object", DataSourceType::JSON, objectFile.path());
    objectConfig.parameters["format"] = "object";
    JsonDataSource objectSource(objectConfig);
    ASSERT_EQ(objectSource.connect(), HatsStatus::SUCCESS);
    DataSet objectData;
    ASSERT_EQ(objectSource.loadData(objectData), HatsStatus::SUCCESS);
    ASSERT_EQ(objectData.size(), 1u);
    EXPECT_EQ(valueAt<std::string>(objectData[0], "escaped"), "line\n\"quote\"");
    EXPECT_EQ(valueAt<int>(objectData[0], "i"), 7);
    EXPECT_DOUBLE_EQ(valueAt<double>(objectData[0], "d"), 2.5);
    EXPECT_TRUE(valueAt<bool>(objectData[0], "b"));

    TemporaryFile arrayFile(R"([{"id":1},{"id":2}])", ".json");
    auto arrayConfig = configFor("array", DataSourceType::JSON, arrayFile.path());
    arrayConfig.parameters["format"] = "array";
    JsonDataSource arraySource(arrayConfig);
    ASSERT_EQ(arraySource.connect(), HatsStatus::SUCCESS);
    DataSet arrayData;
    ASSERT_EQ(arraySource.loadData(arrayData), HatsStatus::SUCCESS);
    ASSERT_EQ(arrayData.size(), 2u);
    EXPECT_EQ(valueAt<int>(arrayData[1], "id"), 2);

    TemporaryFile linesFile("{\"id\":1}\n\n{\"id\":2,\"text\":\"a,b\"}\n", ".jsonl");
    auto linesConfig = configFor("lines", DataSourceType::JSON, linesFile.path());
    linesConfig.parameters["format"] = "jsonl";
    JsonDataSource linesSource(linesConfig);
    ASSERT_EQ(linesSource.connect(), HatsStatus::SUCCESS);
    DataSet linesData;
    ASSERT_EQ(linesSource.loadData(linesData), HatsStatus::SUCCESS);
    EXPECT_EQ(linesData.size(), 2u);
    EXPECT_EQ(valueAt<std::string>(linesData[1], "text"), "a,b");
}

TEST(HatsJsonSource, AutoModeAcceptsAllDocumentShapesAndRejectsUnsupportedValues) {
    for (const std::string& payload : {
             std::string(R"({"id":1})"),
             std::string(R"([{"id":1},{"id":2}])"),
             std::string("{\"id\":1}\n{\"id\":2}\n")}) {
        TemporaryFile file(payload, ".json");
        JsonDataSource source(configFor("auto", DataSourceType::JSON, file.path()));
        ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
        DataSet data;
        EXPECT_EQ(source.loadData(data), HatsStatus::SUCCESS);
        EXPECT_FALSE(data.empty());
    }

    for (const std::string& payload : {
             std::string(R"({"null":null})"),
             std::string(R"({"nested":{"x":1}})"),
             std::string(R"({"array":[1]})"),
             std::string(R"([{"ok":1},2])"),
             std::string(R"(42)"),
             std::string(R"({"huge":2147483648})")}) {
        TemporaryFile file(payload, ".json");
        JsonDataSource source(configFor("bad", DataSourceType::JSON, file.path()));
        ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
        DataSet output = sentinelData();
        EXPECT_EQ(source.loadData(output), HatsStatus::ERROR_INVALID_FORMAT);
        EXPECT_EQ(output, sentinelData());
    }
}

TEST(HatsJsonSource, MalformedOrMixedJsonLinesAreTransactional) {
    for (const std::string& payload : {
             std::string("{\"id\":1}\nnot-json\n"),
             std::string("{\"id\":1}\n[1,2]\n"),
             std::string("{\"id\":1}\n{\"id\":2\n")}) {
        TemporaryFile file(payload, ".jsonl");
        auto config = configFor("jsonl", DataSourceType::JSON, file.path());
        config.parameters["format"] = "jsonl";
        JsonDataSource source(config);
        ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
        DataSet output = sentinelData();
        EXPECT_EQ(source.loadData(output), HatsStatus::ERROR_INVALID_FORMAT);
        EXPECT_EQ(output, sentinelData());
    }
}

TEST(HatsJsonSource, ExplicitObjectModeRejectsAnArrayTransactionally) {
    TemporaryFile arrayFile(R"([{"id":1}])", ".json");
    auto config = configFor("mode", DataSourceType::JSON, arrayFile.path());
    config.parameters["format"] = "object";
    JsonDataSource source(config);
    ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
    DataSet output = sentinelData();
    EXPECT_EQ(source.loadData(output), HatsStatus::ERROR_INVALID_FORMAT);
    EXPECT_EQ(output, sentinelData());
}

TEST(HatsJsonSource, ValidatesConfigurationAndConnectionState) {
    TemporaryFile file("{}", ".json");
    auto config = configFor("inactive", DataSourceType::JSON, file.path());
    config.isActive = false;
    JsonDataSource inactive(config);
    EXPECT_EQ(inactive.connect(), HatsStatus::ERROR_INVALID_SOURCE);

    config = configFor("", DataSourceType::JSON, file.path());
    JsonDataSource emptyId(config);
    EXPECT_EQ(emptyId.connect(), HatsStatus::ERROR_INVALID_SOURCE);

    config = configFor("wrong", DataSourceType::CSV, file.path());
    JsonDataSource wrongType(config);
    EXPECT_EQ(wrongType.connect(), HatsStatus::ERROR_INVALID_FORMAT);

    config = configFor("format", DataSourceType::JSON, file.path());
    config.parameters["format"] = "guess";
    JsonDataSource wrongFormat(config);
    EXPECT_EQ(wrongFormat.connect(), HatsStatus::ERROR_INVALID_FORMAT);

    JsonDataSource disconnected(configFor("state", DataSourceType::JSON, file.path()));
    DataSet output = sentinelData();
    EXPECT_EQ(disconnected.loadData(output), HatsStatus::ERROR_INVALID_SOURCE);
    EXPECT_EQ(output, sentinelData());
}

TEST(HatsCsvSource, ParsesQuotingEscapedQuotesNewlinesAndEmptyFields) {
    TemporaryFile file(
        "name,note,empty,number\r\n"
        "\"Doe, Jane\",\"said \"\"hello\"\"\",,42\r\n"
        "Alice,\"first line\nsecond line\",\"  spaced  \",3.5\r\n",
        ".csv");
    CsvDataSource source(configFor("csv", DataSourceType::CSV, file.path()));
    ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
    DataSet data;
    ASSERT_EQ(source.loadData(data), HatsStatus::SUCCESS);
    ASSERT_EQ(data.size(), 2u);
    EXPECT_EQ(valueAt<std::string>(data[0], "name"), "Doe, Jane");
    EXPECT_EQ(valueAt<std::string>(data[0], "note"), "said \"hello\"");
    EXPECT_EQ(valueAt<std::string>(data[0], "empty"), "");
    EXPECT_EQ(valueAt<int>(data[0], "number"), 42);
    EXPECT_EQ(valueAt<std::string>(data[1], "note"), "first line\nsecond line");
    EXPECT_EQ(valueAt<std::string>(data[1], "empty"), "  spaced  ");
    EXPECT_DOUBLE_EQ(valueAt<double>(data[1], "number"), 3.5);
}

TEST(HatsCsvSource, SupportsHeaderlessAndCustomDelimiter) {
    TemporaryFile file("1;true;alpha\n2;false;beta\n", ".csv");
    auto config = configFor("csv", DataSourceType::CSV, file.path());
    config.parameters["hasHeader"] = "false";
    config.parameters["delimiter"] = ";";
    CsvDataSource source(config);
    ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
    DataSet data;
    ASSERT_EQ(source.loadData(data), HatsStatus::SUCCESS);
    ASSERT_EQ(data.size(), 2u);
    EXPECT_EQ(valueAt<int>(data[0], "col_0"), 1);
    EXPECT_TRUE(valueAt<bool>(data[0], "col_1"));
    EXPECT_EQ(valueAt<std::string>(data[1], "col_2"), "beta");
}

TEST(HatsCsvSource, RejectsMalformedHeadersRowsQuotesAndConfigTransactionally) {
    const std::vector<std::string> payloads{
        "a,a\n1,2\n",
        "a,\n1,2\n",
        "a,b\n1\n",
        "a,b\n1,2,3\n",
        "a,b\n\"unterminated,2\n",
        "a,b\n\"value\"junk,2\n"};
    for (const auto& payload : payloads) {
        TemporaryFile file(payload, ".csv");
        CsvDataSource source(configFor("bad", DataSourceType::CSV, file.path()));
        ASSERT_EQ(source.connect(), HatsStatus::SUCCESS);
        DataSet output = sentinelData();
        EXPECT_EQ(source.loadData(output), HatsStatus::ERROR_INVALID_FORMAT);
        EXPECT_EQ(output, sentinelData());
    }

    TemporaryFile file("a,b\n1,2\n", ".csv");
    auto config = configFor("bad-config", DataSourceType::CSV, file.path());
    config.parameters["delimiter"] = "::";
    CsvDataSource badDelimiter(config);
    EXPECT_EQ(badDelimiter.connect(), HatsStatus::ERROR_INVALID_FORMAT);
    config.parameters.erase("delimiter");
    config.parameters["hasHeader"] = "perhaps";
    CsvDataSource badHeader(config);
    EXPECT_EQ(badHeader.connect(), HatsStatus::ERROR_INVALID_FORMAT);
}

TEST(HatsProcessor, FilterIsTransactionalWhenCallbackThrows) {
    DataProcessor processor;
    ProcessingStep step;
    step.operation = ProcessingOperation::FILTER;
    step.condition = [](const DataRecord& record) {
        if (valueAt<int>(record, "id") == 2) {
            throw std::runtime_error("failure");
        }
        return true;
    };
    processor.addStep(step);
    const DataSet input{{{"id", 1}}, {{"id", 2}}};
    DataSet output = sentinelData();
    EXPECT_EQ(processor.process(input, output), HatsStatus::ERROR_PROCESSING_FAILED);
    EXPECT_EQ(output, sentinelData());
}

TEST(HatsProcessor, FilterRejectsMissingConditionAndUnexpectedParameters) {
    for (const bool addUnexpectedParameter : {false, true}) {
        DataProcessor processor;
        ProcessingStep step;
        step.operation = ProcessingOperation::FILTER;
        if (addUnexpectedParameter) {
            step.parameters["unexpected"] = true;
            step.condition = [](const DataRecord&) { return true; };
        }
        processor.addStep(step);
        DataSet output = sentinelData();
        EXPECT_EQ(processor.process(DataSet{{{"id", 1}}}, output),
                  HatsStatus::ERROR_PROCESSING_FAILED);
        EXPECT_EQ(output, sentinelData());
    }
}

TEST(HatsProcessor, TransformAppliesDocumentedOperationsInOrder) {
    DataProcessor processor;
    std::atomic<int> callbackCount{0};
    TransformCallback callback = [&](DataRecord& record) {
        ++callbackCount;
        if (processor.getStepCount() != 1u) {
            return false;
        }
        record.emplace("callback", true);
        return true;
    };
    processor.addStep(transformStep({
        {"rename", FieldRenameMap{{"old", "renamed"}}},
        {"select", FieldList{"renamed", "age"}},
        {"add_constant", ConstantFieldMap{{"kind", std::string("person")}}},
        {"convert", FieldConversionMap{{"age", "int"}, {"renamed", "string"}}},
        {"callback", callback}}));

    const DataSet input{{{"old", 9}, {"age", std::string("41")}, {"drop", true}}};
    DataSet output;
    ASSERT_EQ(processor.process(input, output), HatsStatus::SUCCESS);
    ASSERT_EQ(output.size(), 1u);
    EXPECT_EQ(output[0].size(), 4u);
    EXPECT_EQ(valueAt<std::string>(output[0], "renamed"), "9");
    EXPECT_EQ(valueAt<int>(output[0], "age"), 41);
    EXPECT_EQ(valueAt<std::string>(output[0], "kind"), "person");
    EXPECT_TRUE(valueAt<bool>(output[0], "callback"));
    EXPECT_EQ(callbackCount.load(), 1);
}

TEST(HatsProcessor, TransformDropAndEveryTypeConversion) {
    DataProcessor processor;
    processor.addStep(transformStep({
        {"drop", FieldList{"discard"}},
        {"convert", FieldConversionMap{{"integer", "int"},
                                         {"floating", "double"},
                                         {"boolean", "bool"},
                                         {"text", "string"}}}}));
    const DataSet input{{{"integer", std::string("7")},
                         {"floating", 4},
                         {"boolean", std::string("false")},
                         {"text", true},
                         {"discard", 1}}};
    DataSet output;
    ASSERT_EQ(processor.process(input, output), HatsStatus::SUCCESS);
    EXPECT_EQ(valueAt<int>(output[0], "integer"), 7);
    EXPECT_DOUBLE_EQ(valueAt<double>(output[0], "floating"), 4.0);
    EXPECT_FALSE(valueAt<bool>(output[0], "boolean"));
    EXPECT_EQ(valueAt<std::string>(output[0], "text"), "true");
    EXPECT_EQ(output[0].count("discard"), 0u);
}

TEST(HatsProcessor, TransformRejectsMalformedParametersAndFailedRecordsTransactionally) {
    const DataSet input{{{"a", std::string("1")}}, {{"a", std::string("bad")}}};
    const std::vector<ProcessingStep> invalidSteps{
        transformStep({}),
        transformStep({{"unknown", 1}}),
        transformStep({{"rename", std::string("wrong")}}),
        transformStep({{"drop", FieldList{"a"}}, {"select", FieldList{"a"}}}),
        transformStep({{"select", FieldList{"a", "a"}}}),
        transformStep({{"convert", FieldConversionMap{{"a", "uint"}}}}),
        transformStep({{"convert", FieldConversionMap{{"a", "int"}}}}),
        transformStep({{"callback", TransformCallback([](DataRecord&) { return false; })}})};

    for (const auto& step : invalidSteps) {
        DataProcessor processor;
        processor.addStep(step);
        DataSet output = sentinelData();
        EXPECT_EQ(processor.process(input, output), HatsStatus::ERROR_PROCESSING_FAILED);
        EXPECT_EQ(output, sentinelData());
    }
}

TEST(HatsProcessor, SortIsStableNamedDirectionalAndTypeAware) {
    const DataSet input{
        {{"id", std::string("bool")}, {"key", false}},
        {{"id", std::string("integer")}, {"key", 10}},
        {{"id", std::string("double")}, {"key", 2.5}},
        {{"id", std::string("same-a")}, {"key", 10.0}},
        {{"id", std::string("same-b")}, {"key", 10}},
        {{"id", std::string("string")}, {"key", std::string("1")}}};

    ProcessingStep ascending;
    ascending.operation = ProcessingOperation::SORT;
    ascending.parameters = {{"key", std::string("key")},
                            {"direction", std::string("asc")}};
    DataProcessor processor;
    processor.addStep(ascending);
    DataSet output;
    ASSERT_EQ(processor.process(input, output), HatsStatus::SUCCESS);
    EXPECT_EQ(valueAt<std::string>(output[0], "id"), "bool");
    EXPECT_EQ(valueAt<std::string>(output[1], "id"), "double");
    EXPECT_EQ(valueAt<std::string>(output[2], "id"), "integer");
    EXPECT_EQ(valueAt<std::string>(output[3], "id"), "same-a");
    EXPECT_EQ(valueAt<std::string>(output[4], "id"), "same-b");
    EXPECT_EQ(valueAt<std::string>(output[5], "id"), "string");

    ascending.parameters["direction"] = std::string("desc");
    DataProcessor descending;
    descending.addStep(ascending);
    ASSERT_EQ(descending.process(input, output), HatsStatus::SUCCESS);
    EXPECT_EQ(valueAt<std::string>(output.front(), "id"), "string");
    EXPECT_EQ(valueAt<std::string>(output.back(), "id"), "bool");
}

TEST(HatsProcessor, SortRejectsMalformedParamsAndMissingKeysTransactionally) {
    const DataSet input{{{"value", 1}}, {{"other", 2}}};
    const std::vector<std::unordered_map<std::string, std::any>> parameterSets{
        {},
        {{"key", 3}},
        {{"key", std::string("value")}, {"direction", std::string("sideways")}},
        {{"key", std::string("value")}, {"extra", true}},
        {{"key", std::string("value")}}};
    for (const auto& parameters : parameterSets) {
        ProcessingStep step;
        step.operation = ProcessingOperation::SORT;
        step.parameters = parameters;
        DataProcessor processor;
        processor.addStep(step);
        DataSet output = sentinelData();
        EXPECT_EQ(processor.process(input, output), HatsStatus::ERROR_PROCESSING_FAILED);
        EXPECT_EQ(output, sentinelData());
    }
}

TEST(HatsProcessor, UnsupportedOperationEnumsFailTruthfully) {
    for (const auto operation : {ProcessingOperation::AGGREGATE,
                                 ProcessingOperation::GROUP,
                                 ProcessingOperation::JOIN}) {
        DataProcessor processor;
        ProcessingStep step;
        step.operation = operation;
        processor.addStep(step);
        DataSet output = sentinelData();
        EXPECT_EQ(processor.process(DataSet{{{"x", 1}}}, output),
                  HatsStatus::ERROR_PROCESSING_FAILED);
        EXPECT_EQ(output, sentinelData());
    }
}

TEST(HatsProcessor, StepStorageSupportsConcurrentReadsAndRegistration) {
    DataProcessor processor;
    ProcessingStep step;
    step.operation = ProcessingOperation::FILTER;
    step.condition = [](const DataRecord&) { return true; };

    std::atomic<bool> start{false};
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&] {
            while (!start.load()) {}
            for (int n = 0; n < 500; ++n) {
                processor.addStep(step);
                (void)processor.getStepCount();
            }
        });
    }
    start.store(true);
    for (auto& thread : threads) {
        thread.join();
    }
    EXPECT_EQ(processor.getStepCount(), 2000u);
    processor.clearSteps();
    EXPECT_EQ(processor.getStepCount(), 0u);
}

TEST(HatsManager, ValidatesRegistrationLifecycleAndReturnsSortedIds) {
    HatsManager manager;
    TemporaryFile file("{}", ".json");
    EXPECT_EQ(manager.registerDataSource(nullptr), HatsStatus::ERROR_INVALID_SOURCE);

    auto empty = configFor("", DataSourceType::JSON, file.path());
    EXPECT_EQ(manager.registerDataSource(std::make_unique<JsonDataSource>(empty)),
              HatsStatus::ERROR_INVALID_SOURCE);
    auto inactive = configFor("inactive", DataSourceType::JSON, file.path());
    inactive.isActive = false;
    EXPECT_EQ(manager.registerDataSource(std::make_unique<JsonDataSource>(inactive)),
              HatsStatus::ERROR_INVALID_SOURCE);
    auto unsupported = configFor("xml", DataSourceType::XML, file.path());
    EXPECT_EQ(manager.registerDataSource(
                  std::make_unique<InstrumentedSource>(unsupported, DataSet{})),
              HatsStatus::ERROR_INVALID_SOURCE);

    for (const auto& id : {std::string("z"), std::string("a"), std::string("m")}) {
        EXPECT_EQ(manager.registerDataSource(std::make_unique<JsonDataSource>(
                      configFor(id, DataSourceType::JSON, file.path()))),
                  HatsStatus::SUCCESS);
    }
    EXPECT_EQ(manager.getDataSourceIds(), (std::vector<std::string>{"a", "m", "z"}));
    EXPECT_EQ(manager.registerDataSource(std::make_unique<JsonDataSource>(
                  configFor("a", DataSourceType::JSON, file.path()))),
              HatsStatus::ERROR_INVALID_SOURCE);
    EXPECT_NE(manager.getDataSource("a"), nullptr);
    EXPECT_EQ(manager.unregisterDataSource("a"), HatsStatus::SUCCESS);
    EXPECT_EQ(manager.unregisterDataSource("a"), HatsStatus::ERROR_NOT_FOUND);
    DataSet missingOutput = sentinelData();
    EXPECT_EQ(manager.loadFromSource("missing", missingOutput), HatsStatus::ERROR_NOT_FOUND);
    EXPECT_EQ(missingOutput, sentinelData());
}

TEST(HatsManager, LoadingAndMergingAreTransactional) {
    HatsManager manager;
    TemporaryFile good("{\"id\":1}\n", ".jsonl");
    TemporaryFile bad("{\"id\":2}\ninvalid\n", ".jsonl");
    auto goodConfig = configFor("good", DataSourceType::JSON, good.path());
    goodConfig.parameters["format"] = "jsonl";
    auto badConfig = configFor("bad", DataSourceType::JSON, bad.path());
    badConfig.parameters["format"] = "jsonl";
    ASSERT_EQ(manager.registerDataSource(std::make_unique<JsonDataSource>(goodConfig)),
              HatsStatus::SUCCESS);
    ASSERT_EQ(manager.registerDataSource(std::make_unique<JsonDataSource>(badConfig)),
              HatsStatus::SUCCESS);

    DataSet output = sentinelData();
    EXPECT_EQ(manager.loadFromMultipleSources({"good", "bad"}, output),
              HatsStatus::ERROR_INVALID_FORMAT);
    EXPECT_EQ(output, sentinelData());

    EXPECT_EQ(manager.mergeDataSets({DataSet{{{"a", 1}}}, DataSet{{{"b", 2}}}}, output),
              HatsStatus::SUCCESS);
    EXPECT_EQ(output.size(), 2u);
}

TEST(HatsManager, SourceCallbacksOccurOutsideManagerLock) {
    HatsManager manager;
    std::atomic<int> callbackDepth{0};
    auto config = configFor("instrumented", DataSourceType::JSON, "memory");
    const auto callback = [&] {
        ++callbackDepth;
        // Re-enter both shared-lock manager APIs. A callback made while the
        // unregister unique lock is held would deadlock here.
        (void)manager.getRegisteredSourceCount();
        (void)manager.getDataSourceIds();
    };
    auto source = std::make_unique<InstrumentedSource>(config, DataSet{{{"id", 1}}},
                                                       callback);
    ASSERT_EQ(manager.registerDataSource(std::move(source)), HatsStatus::SUCCESS);

    DataSet output;
    auto load = std::async(std::launch::async, [&] {
        return manager.loadFromSource("instrumented", output);
    });
    EXPECT_EQ(load.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_EQ(load.get(), HatsStatus::SUCCESS);
    EXPECT_EQ(callbackDepth.load(), 2);

    auto removal = std::async(std::launch::async, [&] {
        return manager.unregisterDataSource("instrumented");
    });
    EXPECT_EQ(removal.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    EXPECT_EQ(removal.get(), HatsStatus::SUCCESS);
    EXPECT_EQ(callbackDepth.load(), 3);
}

TEST(HatsManager, ConcurrentReadersAndRegistrationAreSafe) {
    HatsManager manager;
    TemporaryFile file("{}", ".json");
    constexpr int sourceCount = 200;
    std::atomic<bool> start{false};
    std::atomic<bool> failed{false};

    std::thread writer([&] {
        while (!start.load()) {}
        for (int i = 0; i < sourceCount; ++i) {
            const std::string id = "source-" + std::to_string(i);
            if (manager.registerDataSource(std::make_unique<JsonDataSource>(
                    configFor(id, DataSourceType::JSON, file.path()))) != HatsStatus::SUCCESS) {
                failed.store(true);
            }
        }
    });

    std::vector<std::thread> readers;
    for (int i = 0; i < 4; ++i) {
        readers.emplace_back([&] {
            while (!start.load()) {}
            for (int n = 0; n < 2000; ++n) {
                const auto ids = manager.getDataSourceIds();
                if (!std::is_sorted(ids.begin(), ids.end()) ||
                    manager.getRegisteredSourceCount() > sourceCount) {
                    failed.store(true);
                }
                (void)manager.isSourceRegistered("source-1");
                (void)manager.getDataSource("source-1");
            }
        });
    }

    start.store(true);
    writer.join();
    for (auto& reader : readers) {
        reader.join();
    }
    EXPECT_FALSE(failed.load());
    EXPECT_EQ(manager.getRegisteredSourceCount(), sourceCount);
}

TEST(HatsManager, ProcessDataRunsARealPipeline) {
    HatsManager manager;
    TemporaryFile file("name,age\nAlice,30\nBob,20\n", ".csv");
    ASSERT_EQ(manager.registerDataSource(std::make_unique<CsvDataSource>(
                  configFor("people", DataSourceType::CSV, file.path()))),
              HatsStatus::SUCCESS);

    ProcessingStep filter;
    filter.operation = ProcessingOperation::FILTER;
    filter.condition = [](const DataRecord& record) {
        return valueAt<int>(record, "age") >= 30;
    };
    ProcessingStep transform = transformStep(
        {{"rename", FieldRenameMap{{"name", "person"}}}});
    DataSet output;
    ASSERT_EQ(manager.processData("people", {filter, transform}, output),
              HatsStatus::SUCCESS);
    ASSERT_EQ(output.size(), 1u);
    EXPECT_EQ(valueAt<std::string>(output[0], "person"), "Alice");
}

TEST(HatsUtils, StatusNamesCoverAllEnums) {
    EXPECT_EQ(hats_utils::statusToString(HatsStatus::SUCCESS), "SUCCESS");
    EXPECT_EQ(hats_utils::statusToString(HatsStatus::ERROR_INVALID_SOURCE),
              "ERROR_INVALID_SOURCE");
    EXPECT_EQ(hats_utils::statusToString(HatsStatus::ERROR_INVALID_FORMAT),
              "ERROR_INVALID_FORMAT");
    EXPECT_EQ(hats_utils::statusToString(HatsStatus::ERROR_PROCESSING_FAILED),
              "ERROR_PROCESSING_FAILED");
    EXPECT_EQ(hats_utils::statusToString(HatsStatus::ERROR_NOT_FOUND),
              "ERROR_NOT_FOUND");
    EXPECT_EQ(hats_utils::statusToString(HatsStatus::ERROR_ACCESS_DENIED),
              "ERROR_ACCESS_DENIED");
}
