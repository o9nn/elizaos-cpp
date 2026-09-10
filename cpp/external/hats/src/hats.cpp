#include "elizaos/hats.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>
#include <type_traits>
#include <unordered_set>

namespace elizaos {
namespace {

using Json = nlohmann::json;

std::string trimCopy(const std::string& value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
    if (first == value.end()) {
        return {};
    }
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    }).base();
    return std::string(first, last);
}

std::string lowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool hasSupportedType(const DataSourceConfig& config) {
    return config.type == DataSourceType::JSON || config.type == DataSourceType::CSV;
}

bool hasUsableIdentity(const DataSourceConfig& config) {
    return config.isActive && !trimCopy(config.id).empty() && !config.location.empty();
}

bool jsonValueToDataValue(const Json& value, DataValue& converted) {
    if (value.is_boolean()) {
        converted = value.get<bool>();
        return true;
    }
    if (value.is_string()) {
        converted = value.get<std::string>();
        return true;
    }
    if (value.is_number_integer()) {
        const auto number = value.get<std::int64_t>();
        if (number < std::numeric_limits<int>::min() ||
            number > std::numeric_limits<int>::max()) {
            return false;
        }
        converted = static_cast<int>(number);
        return true;
    }
    if (value.is_number_unsigned()) {
        const auto number = value.get<std::uint64_t>();
        if (number > static_cast<std::uint64_t>(std::numeric_limits<int>::max())) {
            return false;
        }
        converted = static_cast<int>(number);
        return true;
    }
    if (value.is_number_float()) {
        const double number = value.get<double>();
        if (!std::isfinite(number)) {
            return false;
        }
        converted = number;
        return true;
    }
    return false;
}

bool jsonObjectToRecord(const Json& object, DataRecord& record) {
    if (!object.is_object()) {
        return false;
    }

    DataRecord candidate;
    for (auto it = object.cbegin(); it != object.cend(); ++it) {
        DataValue value;
        if (!jsonValueToDataValue(it.value(), value)) {
            return false;
        }
        candidate.emplace(it.key(), std::move(value));
    }
    record = std::move(candidate);
    return true;
}

bool appendJsonDocument(const Json& document, const std::string& mode,
                        DataSet& candidate) {
    if (mode == "object") {
        DataRecord record;
        if (!jsonObjectToRecord(document, record)) {
            return false;
        }
        candidate.push_back(std::move(record));
        return true;
    }

    if (mode == "array") {
        if (!document.is_array()) {
            return false;
        }
        for (const auto& item : document) {
            DataRecord record;
            if (!jsonObjectToRecord(item, record)) {
                return false;
            }
            candidate.push_back(std::move(record));
        }
        return true;
    }
    return false;
}

bool parseJsonLines(const std::string& content, DataSet& candidate) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        if (trimCopy(line).empty()) {
            continue;
        }
        try {
            const Json item = Json::parse(line);
            DataRecord record;
            if (!jsonObjectToRecord(item, record)) {
                return false;
            }
            candidate.push_back(std::move(record));
        } catch (const Json::exception&) {
            return false;
        }
    }
    return true;
}

struct CsvField {
    std::string value;
    bool quoted = false;
    bool started = false;
};

using CsvRow = std::vector<CsvField>;

bool parseCsv(std::istream& input, char delimiter, std::vector<CsvRow>& rows) {
    CsvRow row;
    CsvField field;
    bool inQuotes = false;
    bool afterQuote = false;

    auto finishField = [&]() {
        row.push_back(std::move(field));
        field = CsvField{};
        afterQuote = false;
    };
    auto finishRow = [&]() {
        const bool blank = row.empty() && !field.started && field.value.empty();
        if (!blank) {
            finishField();
            rows.push_back(std::move(row));
            row = CsvRow{};
        }
        field = CsvField{};
        afterQuote = false;
    };

    char ch = '\0';
    while (input.get(ch)) {
        if (inQuotes) {
            if (ch == '"') {
                if (input.peek() == '"') {
                    input.get(ch);
                    field.value.push_back('"');
                } else {
                    inQuotes = false;
                    afterQuote = true;
                }
            } else {
                field.value.push_back(ch);
            }
            continue;
        }

        if (afterQuote) {
            if (ch == delimiter) {
                finishField();
            } else if (ch == '\n') {
                finishRow();
            } else if (ch == '\r') {
                if (input.peek() == '\n') {
                    input.get(ch);
                }
                finishRow();
            } else {
                return false;
            }
            continue;
        }

        if (ch == '"') {
            if (field.started || !field.value.empty()) {
                return false;
            }
            field.started = true;
            field.quoted = true;
            inQuotes = true;
        } else if (ch == delimiter) {
            finishField();
        } else if (ch == '\n') {
            finishRow();
        } else if (ch == '\r') {
            if (input.peek() == '\n') {
                input.get(ch);
            }
            finishRow();
        } else {
            field.started = true;
            field.value.push_back(ch);
        }
    }

    if (inQuotes) {
        return false;
    }
    if (!row.empty() || field.started || !field.value.empty() || afterQuote) {
        finishField();
        rows.push_back(std::move(row));
    }
    return !input.bad();
}

bool validUniqueNames(const FieldList& names) {
    std::unordered_set<std::string> seen;
    for (const auto& name : names) {
        if (name.empty() || !seen.emplace(name).second) {
            return false;
        }
    }
    return true;
}

template <typename T>
const T* getAnyParameter(const std::unordered_map<std::string, std::any>& params,
                         const std::string& name) {
    const auto it = params.find(name);
    return it == params.end() ? nullptr : std::any_cast<T>(&it->second);
}

bool hasOnlyParameters(const std::unordered_map<std::string, std::any>& params,
                       const std::set<std::string>& accepted) {
    return std::all_of(params.begin(), params.end(), [&](const auto& entry) {
        return accepted.find(entry.first) != accepted.end();
    });
}

bool parseStrictInteger(const std::string& text, int& value) {
    const std::string trimmed = trimCopy(text);
    if (trimmed.empty()) {
        return false;
    }
    try {
        std::size_t position = 0;
        const int parsed = std::stoi(trimmed, &position);
        if (position != trimmed.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool parseStrictDouble(const std::string& text, double& value) {
    const std::string trimmed = trimCopy(text);
    if (trimmed.empty()) {
        return false;
    }
    try {
        std::size_t position = 0;
        const double parsed = std::stod(trimmed, &position);
        if (position != trimmed.size() || !std::isfinite(parsed)) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool convertValue(const DataValue& input, const std::string& target,
                  DataValue& output) {
    const std::string normalized = lowerCopy(target);
    if (normalized == "string") {
        output = hats_utils::dataValueToString(input);
        return true;
    }

    if (normalized == "int") {
        if (const auto* value = std::get_if<int>(&input)) {
            output = *value;
            return true;
        }
        if (const auto* value = std::get_if<bool>(&input)) {
            output = *value ? 1 : 0;
            return true;
        }
        if (const auto* value = std::get_if<double>(&input)) {
            if (std::isfinite(*value) && std::trunc(*value) == *value &&
                *value >= std::numeric_limits<int>::min() &&
                *value <= std::numeric_limits<int>::max()) {
                output = static_cast<int>(*value);
                return true;
            }
            return false;
        }
        int parsed = 0;
        if (parseStrictInteger(std::get<std::string>(input), parsed)) {
            output = parsed;
            return true;
        }
        return false;
    }

    if (normalized == "double") {
        if (const auto* value = std::get_if<double>(&input)) {
            if (!std::isfinite(*value)) {
                return false;
            }
            output = *value;
            return true;
        }
        if (const auto* value = std::get_if<int>(&input)) {
            output = static_cast<double>(*value);
            return true;
        }
        if (const auto* value = std::get_if<bool>(&input)) {
            output = *value ? 1.0 : 0.0;
            return true;
        }
        double parsed = 0.0;
        if (parseStrictDouble(std::get<std::string>(input), parsed)) {
            output = parsed;
            return true;
        }
        return false;
    }

    if (normalized == "bool") {
        if (const auto* value = std::get_if<bool>(&input)) {
            output = *value;
            return true;
        }
        if (const auto* value = std::get_if<int>(&input)) {
            if (*value == 0 || *value == 1) {
                output = (*value == 1);
                return true;
            }
            return false;
        }
        if (const auto* value = std::get_if<double>(&input)) {
            if (*value == 0.0 || *value == 1.0) {
                output = (*value == 1.0);
                return true;
            }
            return false;
        }
        const std::string parsed = lowerCopy(trimCopy(std::get<std::string>(input)));
        if (parsed == "true" || parsed == "1") {
            output = true;
            return true;
        }
        if (parsed == "false" || parsed == "0") {
            output = false;
            return true;
        }
        return false;
    }
    return false;
}

int valueCategory(const DataValue& value) {
    if (std::holds_alternative<bool>(value)) {
        return 0;
    }
    if (std::holds_alternative<int>(value) || std::holds_alternative<double>(value)) {
        return 1;
    }
    return 2;
}

int compareDataValues(const DataValue& left, const DataValue& right) {
    const int leftCategory = valueCategory(left);
    const int rightCategory = valueCategory(right);
    if (leftCategory != rightCategory) {
        return leftCategory < rightCategory ? -1 : 1;
    }

    if (leftCategory == 0) {
        const bool lhs = std::get<bool>(left);
        const bool rhs = std::get<bool>(right);
        return lhs == rhs ? 0 : (lhs ? 1 : -1);
    }
    if (leftCategory == 1) {
        const long double lhs = std::holds_alternative<int>(left)
                                    ? static_cast<long double>(std::get<int>(left))
                                    : static_cast<long double>(std::get<double>(left));
        const long double rhs = std::holds_alternative<int>(right)
                                    ? static_cast<long double>(std::get<int>(right))
                                    : static_cast<long double>(std::get<double>(right));
        const bool lhsNan = std::isnan(lhs);
        const bool rhsNan = std::isnan(rhs);
        if (lhsNan || rhsNan) {
            return lhsNan == rhsNan ? 0 : (lhsNan ? -1 : 1);
        }
        return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
    }

    const auto& lhs = std::get<std::string>(left);
    const auto& rhs = std::get<std::string>(right);
    return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
}

} // namespace

JsonDataSource::JsonDataSource(const DataSourceConfig& config)
    : DataSource(config) {}

HatsStatus JsonDataSource::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!config_.isActive || trimCopy(config_.id).empty() ||
        config_.type != DataSourceType::JSON || config_.location.empty()) {
        connected_ = false;
        return config_.type == DataSourceType::JSON
                   ? HatsStatus::ERROR_INVALID_SOURCE
                   : HatsStatus::ERROR_INVALID_FORMAT;
    }
    const auto format = config_.parameters.find("format");
    if (format != config_.parameters.end() && format->second != "auto" &&
        format->second != "object" && format->second != "array" &&
        format->second != "jsonl") {
        connected_ = false;
        return HatsStatus::ERROR_INVALID_FORMAT;
    }
    std::ifstream file(config_.location, std::ios::binary);
    connected_ = file.good();
    return connected_ ? HatsStatus::SUCCESS : HatsStatus::ERROR_INVALID_SOURCE;
}

HatsStatus JsonDataSource::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    connected_ = false;
    return HatsStatus::SUCCESS;
}

HatsStatus JsonDataSource::loadData(DataSet& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_ || !config_.isActive || config_.type != DataSourceType::JSON) {
        return HatsStatus::ERROR_INVALID_SOURCE;
    }

    std::ifstream file(config_.location, std::ios::binary);
    if (!file.is_open()) {
        return HatsStatus::ERROR_ACCESS_DENIED;
    }
    const std::string content((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    if (file.bad()) {
        return HatsStatus::ERROR_ACCESS_DENIED;
    }

    const auto formatEntry = config_.parameters.find("format");
    const std::string mode = formatEntry == config_.parameters.end()
                                 ? "auto"
                                 : formatEntry->second;
    DataSet candidate;
    try {
        if (mode == "jsonl") {
            if (!parseJsonLines(content, candidate)) {
                return HatsStatus::ERROR_INVALID_FORMAT;
            }
        } else if (mode == "object" || mode == "array") {
            const Json document = Json::parse(content);
            if (!appendJsonDocument(document, mode, candidate)) {
                return HatsStatus::ERROR_INVALID_FORMAT;
            }
        } else if (mode == "auto") {
            try {
                const Json document = Json::parse(content);
                const std::string documentMode = document.is_object()
                                                     ? "object"
                                                     : (document.is_array() ? "array" : "");
                if (documentMode.empty() ||
                    !appendJsonDocument(document, documentMode, candidate)) {
                    return HatsStatus::ERROR_INVALID_FORMAT;
                }
            } catch (const Json::parse_error&) {
                candidate.clear();
                if (!parseJsonLines(content, candidate)) {
                    return HatsStatus::ERROR_INVALID_FORMAT;
                }
            }
        } else {
            return HatsStatus::ERROR_INVALID_FORMAT;
        }
    } catch (const Json::exception&) {
        return HatsStatus::ERROR_INVALID_FORMAT;
    } catch (...) {
        return HatsStatus::ERROR_INVALID_FORMAT;
    }

    data = std::move(candidate);
    return HatsStatus::SUCCESS;
}

bool JsonDataSource::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

CsvDataSource::CsvDataSource(const DataSourceConfig& config)
    : DataSource(config) {
    const auto delimiter = config_.parameters.find("delimiter");
    if (delimiter != config_.parameters.end()) {
        if (delimiter->second.size() != 1 || delimiter->second[0] == '"' ||
            delimiter->second[0] == '\r' || delimiter->second[0] == '\n') {
            configValid_ = false;
        } else {
            delimiter_ = delimiter->second[0];
        }
    }

    const auto header = config_.parameters.find("hasHeader");
    if (header != config_.parameters.end()) {
        const std::string normalized = lowerCopy(header->second);
        if (normalized == "true" || normalized == "1") {
            hasHeader_ = true;
        } else if (normalized == "false" || normalized == "0") {
            hasHeader_ = false;
        } else {
            configValid_ = false;
        }
    }
}

HatsStatus CsvDataSource::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!configValid_ || config_.type != DataSourceType::CSV) {
        connected_ = false;
        return HatsStatus::ERROR_INVALID_FORMAT;
    }
    if (!config_.isActive || trimCopy(config_.id).empty() || config_.location.empty()) {
        connected_ = false;
        return HatsStatus::ERROR_INVALID_SOURCE;
    }
    std::ifstream file(config_.location, std::ios::binary);
    connected_ = file.good();
    return connected_ ? HatsStatus::SUCCESS : HatsStatus::ERROR_INVALID_SOURCE;
}

HatsStatus CsvDataSource::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    connected_ = false;
    return HatsStatus::SUCCESS;
}

HatsStatus CsvDataSource::loadData(DataSet& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_ || !config_.isActive || config_.type != DataSourceType::CSV) {
        return HatsStatus::ERROR_INVALID_SOURCE;
    }
    if (!configValid_) {
        return HatsStatus::ERROR_INVALID_FORMAT;
    }

    std::ifstream file(config_.location, std::ios::binary);
    if (!file.is_open()) {
        return HatsStatus::ERROR_ACCESS_DENIED;
    }
    std::vector<CsvRow> rows;
    if (!parseCsv(file, delimiter_, rows)) {
        return HatsStatus::ERROR_INVALID_FORMAT;
    }

    std::vector<std::string> headers;
    std::size_t firstDataRow = 0;
    std::size_t columnCount = 0;
    if (hasHeader_) {
        if (rows.empty()) {
            return HatsStatus::ERROR_INVALID_FORMAT;
        }
        columnCount = rows.front().size();
        std::unordered_set<std::string> seen;
        for (const auto& field : rows.front()) {
            const std::string header = trimCopy(field.value);
            if (header.empty() || !seen.emplace(header).second) {
                return HatsStatus::ERROR_INVALID_FORMAT;
            }
            headers.push_back(header);
        }
        firstDataRow = 1;
    } else if (!rows.empty()) {
        columnCount = rows.front().size();
        for (std::size_t index = 0; index < columnCount; ++index) {
            headers.push_back("col_" + std::to_string(index));
        }
    }

    DataSet candidate;
    for (std::size_t rowIndex = firstDataRow; rowIndex < rows.size(); ++rowIndex) {
        if (rows[rowIndex].size() != columnCount) {
            return HatsStatus::ERROR_INVALID_FORMAT;
        }
        DataRecord record;
        for (std::size_t column = 0; column < columnCount; ++column) {
            const auto& field = rows[rowIndex][column];
            if (field.quoted && trimCopy(field.value) != field.value) {
                record.emplace(headers[column], field.value);
            } else {
                record.emplace(headers[column], hats_utils::parseDataValue(field.value));
            }
        }
        candidate.push_back(std::move(record));
    }

    data = std::move(candidate);
    return HatsStatus::SUCCESS;
}

bool CsvDataSource::isConnected() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}

void DataProcessor::addStep(const ProcessingStep& step) {
    std::lock_guard<std::mutex> lock(mutex_);
    steps_.push_back(step);
}

HatsStatus DataProcessor::process(const DataSet& input, DataSet& output) {
    std::vector<ProcessingStep> steps;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        steps = steps_;
    }

    DataSet current = input;
    try {
        for (const auto& step : steps) {
            DataSet next;
            HatsStatus status = HatsStatus::ERROR_PROCESSING_FAILED;
            switch (step.operation) {
                case ProcessingOperation::FILTER:
                    if (!step.parameters.empty() || !step.condition) {
                        return HatsStatus::ERROR_PROCESSING_FAILED;
                    }
                    status = applyFilter(current, next, step.condition);
                    break;
                case ProcessingOperation::TRANSFORM:
                    if (step.condition) {
                        return HatsStatus::ERROR_PROCESSING_FAILED;
                    }
                    status = applyTransform(current, next, step.parameters);
                    break;
                case ProcessingOperation::SORT: {
                    if (step.condition ||
                        !hasOnlyParameters(step.parameters, {"key", "direction"})) {
                        return HatsStatus::ERROR_PROCESSING_FAILED;
                    }
                    const auto* key = getAnyParameter<std::string>(step.parameters, "key");
                    if (key == nullptr || key->empty()) {
                        return HatsStatus::ERROR_PROCESSING_FAILED;
                    }
                    std::string direction = "asc";
                    if (step.parameters.find("direction") != step.parameters.end()) {
                        const auto* configuredDirection =
                            getAnyParameter<std::string>(step.parameters, "direction");
                        if (configuredDirection == nullptr) {
                            return HatsStatus::ERROR_PROCESSING_FAILED;
                        }
                        direction = lowerCopy(*configuredDirection);
                    }
                    if (direction != "asc" && direction != "desc") {
                        return HatsStatus::ERROR_PROCESSING_FAILED;
                    }
                    if (std::any_of(current.begin(), current.end(), [&](const DataRecord& record) {
                            return record.find(*key) == record.end();
                        })) {
                        return HatsStatus::ERROR_PROCESSING_FAILED;
                    }
                    next = current;
                    const bool ascending = direction == "asc";
                    std::stable_sort(next.begin(), next.end(), [&](const DataRecord& left,
                                                                   const DataRecord& right) {
                        const int comparison = compareDataValues(left.at(*key), right.at(*key));
                        return ascending ? comparison < 0 : comparison > 0;
                    });
                    status = HatsStatus::SUCCESS;
                    break;
                }
                case ProcessingOperation::AGGREGATE:
                case ProcessingOperation::GROUP:
                case ProcessingOperation::JOIN:
                    return HatsStatus::ERROR_PROCESSING_FAILED;
            }
            if (status != HatsStatus::SUCCESS) {
                return HatsStatus::ERROR_PROCESSING_FAILED;
            }
            current = std::move(next);
        }
    } catch (...) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }

    output = std::move(current);
    return HatsStatus::SUCCESS;
}

void DataProcessor::clearSteps() {
    std::lock_guard<std::mutex> lock(mutex_);
    steps_.clear();
}

std::size_t DataProcessor::getStepCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return steps_.size();
}

HatsStatus DataProcessor::applyFilter(
    const DataSet& input, DataSet& output,
    const std::function<bool(const DataRecord&)>& filter) {
    DataSet candidate;
    candidate.reserve(input.size());
    for (const auto& record : input) {
        if (filter(record)) {
            candidate.push_back(record);
        }
    }
    output = std::move(candidate);
    return HatsStatus::SUCCESS;
}

HatsStatus DataProcessor::applyTransform(
    const DataSet& input, DataSet& output,
    const std::unordered_map<std::string, std::any>& params) {
    static const std::set<std::string> accepted{
        "rename", "drop", "select", "add_constant", "convert", "callback"};
    if (params.empty() || !hasOnlyParameters(params, accepted) ||
        (params.count("drop") != 0 && params.count("select") != 0)) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }

    const FieldRenameMap* rename = nullptr;
    const FieldList* drop = nullptr;
    const FieldList* select = nullptr;
    const ConstantFieldMap* constants = nullptr;
    const FieldConversionMap* conversions = nullptr;
    const TransformCallback* callback = nullptr;

    if (params.count("rename") != 0 &&
        (rename = getAnyParameter<FieldRenameMap>(params, "rename")) == nullptr) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if (params.count("drop") != 0 &&
        (drop = getAnyParameter<FieldList>(params, "drop")) == nullptr) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if (params.count("select") != 0 &&
        (select = getAnyParameter<FieldList>(params, "select")) == nullptr) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if (params.count("add_constant") != 0 &&
        (constants = getAnyParameter<ConstantFieldMap>(params, "add_constant")) == nullptr) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if (params.count("convert") != 0 &&
        (conversions = getAnyParameter<FieldConversionMap>(params, "convert")) == nullptr) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if (params.count("callback") != 0 &&
        (callback = getAnyParameter<TransformCallback>(params, "callback")) == nullptr) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }

    if ((rename != nullptr && rename->empty()) || (drop != nullptr && drop->empty()) ||
        (select != nullptr && select->empty()) ||
        (constants != nullptr && constants->empty()) ||
        (conversions != nullptr && conversions->empty()) ||
        (callback != nullptr && !*callback)) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if ((drop != nullptr && !validUniqueNames(*drop)) ||
        (select != nullptr && !validUniqueNames(*select))) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }

    std::unordered_set<std::string> renameSources;
    std::unordered_set<std::string> renameDestinations;
    if (rename != nullptr) {
        for (const auto& entry : *rename) {
            if (entry.first.empty() || entry.second.empty() ||
                !renameSources.emplace(entry.first).second ||
                !renameDestinations.emplace(entry.second).second) {
                return HatsStatus::ERROR_PROCESSING_FAILED;
            }
        }
    }
    if (constants != nullptr &&
        std::any_of(constants->begin(), constants->end(), [](const auto& entry) {
            return entry.first.empty();
        })) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }
    if (conversions != nullptr &&
        std::any_of(conversions->begin(), conversions->end(), [](const auto& entry) {
            const std::string target = lowerCopy(entry.second);
            return entry.first.empty() ||
                   (target != "int" && target != "double" &&
                    target != "string" && target != "bool");
        })) {
        return HatsStatus::ERROR_PROCESSING_FAILED;
    }

    DataSet candidate = input;
    for (auto& record : candidate) {
        if (rename != nullptr) {
            std::unordered_map<std::string, DataValue> renamedValues;
            for (const auto& entry : *rename) {
                const auto source = record.find(entry.first);
                if (source == record.end()) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
                if (record.find(entry.second) != record.end() &&
                    renameSources.find(entry.second) == renameSources.end()) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
                renamedValues.emplace(entry.second, source->second);
            }
            for (const auto& entry : *rename) {
                record.erase(entry.first);
            }
            for (auto& entry : renamedValues) {
                record.emplace(std::move(entry.first), std::move(entry.second));
            }
        }

        if (drop != nullptr) {
            for (const auto& name : *drop) {
                if (record.erase(name) != 1) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
            }
        } else if (select != nullptr) {
            DataRecord selected;
            for (const auto& name : *select) {
                const auto field = record.find(name);
                if (field == record.end()) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
                selected.emplace(name, field->second);
            }
            record = std::move(selected);
        }

        if (constants != nullptr) {
            for (const auto& entry : *constants) {
                if (!record.emplace(entry.first, entry.second).second) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
            }
        }

        if (conversions != nullptr) {
            for (const auto& entry : *conversions) {
                const auto field = record.find(entry.first);
                if (field == record.end()) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
                DataValue converted;
                if (!convertValue(field->second, entry.second, converted)) {
                    return HatsStatus::ERROR_PROCESSING_FAILED;
                }
                field->second = std::move(converted);
            }
        }

        if (callback != nullptr && !(*callback)(record)) {
            return HatsStatus::ERROR_PROCESSING_FAILED;
        }
    }

    output = std::move(candidate);
    return HatsStatus::SUCCESS;
}

HatsStatus HatsManager::registerDataSource(std::unique_ptr<DataSource> source) {
    if (!source) {
        return HatsStatus::ERROR_INVALID_SOURCE;
    }
    const DataSourceConfig& config = source->getConfig();
    if (!hasUsableIdentity(config) || !hasSupportedType(config)) {
        return HatsStatus::ERROR_INVALID_SOURCE;
    }

    std::shared_ptr<DataSource> shared(std::move(source));
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (dataSources_.find(config.id) != dataSources_.end()) {
        return HatsStatus::ERROR_INVALID_SOURCE;
    }
    dataSources_.emplace(config.id, std::move(shared));
    return HatsStatus::SUCCESS;
}

HatsStatus HatsManager::unregisterDataSource(const std::string& sourceId) {
    std::shared_ptr<DataSource> source;
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        const auto found = dataSources_.find(sourceId);
        if (found == dataSources_.end()) {
            return HatsStatus::ERROR_NOT_FOUND;
        }
        source = std::move(found->second);
        dataSources_.erase(found);
    }
    return source->disconnect();
}

std::shared_ptr<DataSource> HatsManager::getDataSource(const std::string& sourceId) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    const auto found = dataSources_.find(sourceId);
    return found == dataSources_.end() ? nullptr : found->second;
}

std::vector<std::string> HatsManager::getDataSourceIds() const {
    std::vector<std::string> ids;
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        ids.reserve(dataSources_.size());
        for (const auto& entry : dataSources_) {
            ids.push_back(entry.first);
        }
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

HatsStatus HatsManager::loadFromSource(const std::string& sourceId, DataSet& data) {
    const std::shared_ptr<DataSource> source = getDataSource(sourceId);
    if (!source) {
        return HatsStatus::ERROR_NOT_FOUND;
    }
    if (!source->isConnected()) {
        const HatsStatus status = source->connect();
        if (status != HatsStatus::SUCCESS) {
            return status;
        }
    }
    return source->loadData(data);
}

HatsStatus HatsManager::loadFromMultipleSources(
    const std::vector<std::string>& sourceIds, DataSet& data) {
    std::vector<DataSet> datasets;
    datasets.reserve(sourceIds.size());
    for (const auto& sourceId : sourceIds) {
        DataSet sourceData;
        const HatsStatus status = loadFromSource(sourceId, sourceData);
        if (status != HatsStatus::SUCCESS) {
            return status;
        }
        datasets.push_back(std::move(sourceData));
    }
    DataSet candidate;
    const HatsStatus status = mergeDataSets(datasets, candidate);
    if (status == HatsStatus::SUCCESS) {
        data = std::move(candidate);
    }
    return status;
}

HatsStatus HatsManager::processData(const std::string& sourceId,
                                    const std::vector<ProcessingStep>& steps,
                                    DataSet& output) {
    DataSet sourceData;
    const HatsStatus status = loadFromSource(sourceId, sourceData);
    if (status != HatsStatus::SUCCESS) {
        return status;
    }
    DataProcessor processor;
    for (const auto& step : steps) {
        processor.addStep(step);
    }
    return processor.process(sourceData, output);
}

HatsStatus HatsManager::mergeDataSets(const std::vector<DataSet>& inputs,
                                      DataSet& merged) {
    DataSet candidate;
    std::size_t totalSize = 0;
    for (const auto& input : inputs) {
        totalSize += input.size();
    }
    candidate.reserve(totalSize);
    for (const auto& input : inputs) {
        candidate.insert(candidate.end(), input.begin(), input.end());
    }
    merged = std::move(candidate);
    return HatsStatus::SUCCESS;
}

std::size_t HatsManager::getRegisteredSourceCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return dataSources_.size();
}

bool HatsManager::isSourceRegistered(const std::string& sourceId) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return dataSources_.find(sourceId) != dataSources_.end();
}

namespace hats_utils {

std::string dataValueToString(const DataValue& value) {
    return std::visit([](const auto& item) -> std::string {
        using ValueType = std::decay_t<decltype(item)>;
        if constexpr (std::is_same_v<ValueType, std::string>) {
            return item;
        } else if constexpr (std::is_same_v<ValueType, bool>) {
            return item ? "true" : "false";
        } else {
            return std::to_string(item);
        }
    }, value);
}

DataValue parseDataValue(const std::string& str) {
    const std::string trimmed = trimCopy(str);
    if (trimmed.empty()) {
        return std::string{};
    }
    const std::string normalized = lowerCopy(trimmed);
    if (normalized == "true") {
        return true;
    }
    if (normalized == "false") {
        return false;
    }

    int integer = 0;
    if (parseStrictInteger(trimmed, integer)) {
        return integer;
    }
    double floatingPoint = 0.0;
    if (parseStrictDouble(trimmed, floatingPoint)) {
        return floatingPoint;
    }
    return trimmed;
}

std::unique_ptr<DataSource> createDataSource(const DataSourceConfig& config) {
    if (!hasUsableIdentity(config)) {
        return nullptr;
    }
    switch (config.type) {
        case DataSourceType::JSON:
            return std::make_unique<JsonDataSource>(config);
        case DataSourceType::CSV:
            return std::make_unique<CsvDataSource>(config);
        default:
            return nullptr;
    }
}

std::string statusToString(HatsStatus status) {
    switch (status) {
        case HatsStatus::SUCCESS:
            return "SUCCESS";
        case HatsStatus::ERROR_INVALID_SOURCE:
            return "ERROR_INVALID_SOURCE";
        case HatsStatus::ERROR_INVALID_FORMAT:
            return "ERROR_INVALID_FORMAT";
        case HatsStatus::ERROR_PROCESSING_FAILED:
            return "ERROR_PROCESSING_FAILED";
        case HatsStatus::ERROR_NOT_FOUND:
            return "ERROR_NOT_FOUND";
        case HatsStatus::ERROR_ACCESS_DENIED:
            return "ERROR_ACCESS_DENIED";
    }
    return "UNKNOWN_ERROR";
}

} // namespace hats_utils
} // namespace elizaos
