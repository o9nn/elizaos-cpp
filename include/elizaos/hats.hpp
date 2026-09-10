#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>
#include <variant>
#include <any>
#include <chrono>
#include <mutex>
#include <shared_mutex>

namespace elizaos {

// Forward declarations
class DataSource;
class DataProcessor;
class DataFilter;

// Basic types for HATs protocol
using DataValue = std::variant<int, double, std::string, bool>;
using DataRecord = std::unordered_map<std::string, DataValue>;
using DataSet = std::vector<DataRecord>;
using Timestamp = std::chrono::system_clock::time_point;

// Exact std::any payload types accepted by TRANSFORM.  A transform applies
// parameters in this order: rename, drop/select, add_constant, convert,
// callback.  Unknown parameters, wrong std::any types, missing fields,
// conflicting destinations, and failed callbacks reject the whole operation.
using FieldRenameMap = std::unordered_map<std::string, std::string>;
using FieldList = std::vector<std::string>;
using ConstantFieldMap = std::unordered_map<std::string, DataValue>;
using FieldConversionMap = std::unordered_map<std::string, std::string>;
using TransformCallback = std::function<bool(DataRecord&)>;

/**
 * Supported data source types
 */
enum class DataSourceType {
    JSON,
    CSV,
    XML,
    DATABASE,
    API,
    STREAM,
    CUSTOM
};

/**
 * Data processing operations
 */
enum class ProcessingOperation {
    FILTER,
    TRANSFORM,
    AGGREGATE,
    SORT,
    GROUP,
    JOIN
};

/**
 * HATs protocol status codes
 */
enum class HatsStatus {
    SUCCESS,
    ERROR_INVALID_SOURCE,
    ERROR_INVALID_FORMAT,
    ERROR_PROCESSING_FAILED,
    ERROR_NOT_FOUND,
    ERROR_ACCESS_DENIED
};

/**
 * Data source configuration
 */
struct DataSourceConfig {
    std::string id;
    DataSourceType type;
    std::string location;  // file path, URL, connection string, etc.
    std::unordered_map<std::string, std::string> parameters;
    bool isActive = true;
    std::optional<std::chrono::milliseconds> refreshInterval;
};

/**
 * Data processing pipeline step
 */
struct ProcessingStep {
    ProcessingOperation operation;
    /**
     * Operation parameter schemas:
     *
     * TRANSFORM:
     *   "rename"       -> FieldRenameMap
     *   "drop"         -> FieldList (mutually exclusive with "select")
     *   "select"       -> FieldList (mutually exclusive with "drop")
     *   "add_constant" -> ConstantFieldMap
     *   "convert"      -> FieldConversionMap; targets: int/double/string/bool
     *   "callback"     -> TransformCallback
     * At least one parameter is required.
     *
     * SORT:
     *   "key"       -> std::string (required)
     *   "direction" -> std::string (optional: "asc" or "desc")
     *
     * AGGREGATE, GROUP, and JOIN are deliberately unsupported and return
     * ERROR_PROCESSING_FAILED rather than silently passing records through.
     */
    std::unordered_map<std::string, std::any> parameters;
    std::function<bool(const DataRecord&)> condition;
};

/**
 * Abstract base class for data sources
 */
class DataSource {
public:
    DataSource(const DataSourceConfig& config) : config_(config) {}
    virtual ~DataSource() = default;

    virtual HatsStatus connect() = 0;
    virtual HatsStatus disconnect() = 0;
    virtual HatsStatus loadData(DataSet& data) = 0;
    virtual bool isConnected() const = 0;
    
    const DataSourceConfig& getConfig() const { return config_; }
    const std::string& getId() const { return config_.id; }
    DataSourceType getType() const { return config_.type; }

protected:
    DataSourceConfig config_;
};

/**
 * JSON data source implementation
 */
class JsonDataSource : public DataSource {
public:
    /**
     * config.parameters["format"] selects strict parsing mode:
     *   "object" - one top-level JSON object
     *   "array"  - one top-level array containing only objects
     *   "jsonl"  - one object per non-blank physical line
     *   "auto"   - object/array first, then JSON Lines (default)
     * JSON null, arrays/objects as field values, and numbers outside DataValue's
     * representable range are rejected transactionally.
     */
    JsonDataSource(const DataSourceConfig& config);
    
    HatsStatus connect() override;
    HatsStatus disconnect() override;
    HatsStatus loadData(DataSet& data) override;
    bool isConnected() const override;

private:
    mutable std::mutex mutex_;
    bool connected_ = false;
};

/**
 * CSV data source implementation
 */
class CsvDataSource : public DataSource {
public:
    /**
     * CSV uses RFC 4180-style quoted fields, doubled quote escaping, and
     * embedded newlines.  "delimiter" must contain exactly one character and
     * "hasHeader" accepts true/false/1/0.  Headers must be non-empty and unique;
     * all records must have the same number of columns.
     */
    CsvDataSource(const DataSourceConfig& config);
    
    HatsStatus connect() override;
    HatsStatus disconnect() override;
    HatsStatus loadData(DataSet& data) override;
    bool isConnected() const override;

private:
    mutable std::mutex mutex_;
    bool connected_ = false;
    char delimiter_ = ',';
    bool hasHeader_ = true;
    bool configValid_ = true;
};

/**
 * Data processor for transforming and filtering data
 */
class DataProcessor {
public:
    DataProcessor() = default;
    
    // Add processing step to pipeline
    void addStep(const ProcessingStep& step);
    
    // Process data through pipeline
    HatsStatus process(const DataSet& input, DataSet& output);
    
    // Clear all processing steps
    void clearSteps();
    
    // Get number of processing steps
    size_t getStepCount() const;

private:
    mutable std::mutex mutex_;
    std::vector<ProcessingStep> steps_;
    
    HatsStatus applyFilter(const DataSet& input, DataSet& output, 
                          const std::function<bool(const DataRecord&)>& filter);
    HatsStatus applyTransform(const DataSet& input, DataSet& output,
                             const std::unordered_map<std::string, std::any>& params);
};

/**
 * Main HATs protocol manager
 */
class HatsManager {
public:
    HatsManager() = default;
    ~HatsManager() = default;

    // Data source management
    HatsStatus registerDataSource(std::unique_ptr<DataSource> source);
    HatsStatus unregisterDataSource(const std::string& sourceId);
    // Shared ownership keeps a looked-up source alive across concurrent
    // unregistration. Source virtual methods remain responsible for their own
    // internal synchronization when called directly by clients.
    std::shared_ptr<DataSource> getDataSource(const std::string& sourceId) const;
    std::vector<std::string> getDataSourceIds() const;
    
    // Data operations
    HatsStatus loadFromSource(const std::string& sourceId, DataSet& data);
    HatsStatus loadFromMultipleSources(const std::vector<std::string>& sourceIds, DataSet& data);
    
    // Data processing
    HatsStatus processData(const std::string& sourceId, 
                          const std::vector<ProcessingStep>& steps,
                          DataSet& output);
    
    // Utility methods
    HatsStatus mergeDataSets(const std::vector<DataSet>& inputs, DataSet& merged);
    size_t getRegisteredSourceCount() const;
    bool isSourceRegistered(const std::string& sourceId) const;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<DataSource>> dataSources_;
};

/**
 * Utility functions for HATs protocol
 */
namespace hats_utils {
    // Convert DataValue to string representation
    std::string dataValueToString(const DataValue& value);
    
    // Parse string to DataValue with type inference
    DataValue parseDataValue(const std::string& str);
    
    // Create data source from configuration
    std::unique_ptr<DataSource> createDataSource(const DataSourceConfig& config);
    
    // Get status string for debugging
    std::string statusToString(HatsStatus status);
}

} // namespace elizaos
