#pragma once

/**
 * ElizaOS C++ - Persistence Module
 *
 * Provides persistent storage backends for agent memory and state.
 * Implements Task 1.2.1: SQLite/RocksDB integration for durable storage.
 * 
 * Features:
 * - Abstract storage interface for backend flexibility
 * - SQLite backend for relational data
 * - Key-value store abstraction
 * - Transaction support
 * - Connection pooling
 * - Async operations with futures
 */

#include "elizaos/elizaos.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <future>
#include <queue>
#include <condition_variable>
#include <any>

namespace elizaos {

// Forward declarations
class StorageBackend;
class SQLiteBackend;
class TransactionScope;
class ConnectionPool;

// ============================================================================
// Storage Result Types
// ============================================================================

template<typename T>
struct StorageResult {
    bool success = false;
    std::optional<T> value;
    std::optional<std::string> error;
    int64_t rowsAffected = 0;
    
    static StorageResult<T> ok(T val, int64_t rows = 0) {
        return {true, std::move(val), std::nullopt, rows};
    }
    
    static StorageResult<T> fail(const std::string& err) {
        return {false, std::nullopt, err, 0};
    }
    
    explicit operator bool() const { return success; }
};

// Specialization for void operations
template<>
struct StorageResult<void> {
    bool success = false;
    std::optional<std::string> error;
    int64_t rowsAffected = 0;
    
    static StorageResult<void> ok(int64_t rows = 0) {
        return {true, std::nullopt, rows};
    }
    
    static StorageResult<void> fail(const std::string& err) {
        return {false, err, 0};
    }
    
    explicit operator bool() const { return success; }
};

// ============================================================================
// Query Parameter Types
// ============================================================================

using QueryParam = std::variant<
    std::nullptr_t,
    int64_t,
    double,
    std::string,
    std::vector<uint8_t>  // BLOB data
>;

using QueryParams = std::vector<QueryParam>;

// ============================================================================
// Row and Column Types
// ============================================================================

struct ColumnValue {
    std::string name;
    QueryParam value;
    
    // Type helpers
    bool isNull() const { return std::holds_alternative<std::nullptr_t>(value); }
    bool isInt() const { return std::holds_alternative<int64_t>(value); }
    bool isDouble() const { return std::holds_alternative<double>(value); }
    bool isString() const { return std::holds_alternative<std::string>(value); }
    bool isBlob() const { return std::holds_alternative<std::vector<uint8_t>>(value); }
    
    // Value getters with defaults
    int64_t asInt(int64_t def = 0) const {
        return isInt() ? std::get<int64_t>(value) : def;
    }
    
    double asDouble(double def = 0.0) const {
        return isDouble() ? std::get<double>(value) : def;
    }
    
    std::string asString(const std::string& def = "") const {
        return isString() ? std::get<std::string>(value) : def;
    }
    
    std::vector<uint8_t> asBlob() const {
        return isBlob() ? std::get<std::vector<uint8_t>>(value) : std::vector<uint8_t>{};
    }
};

using Row = std::vector<ColumnValue>;
using ResultSet = std::vector<Row>;

// ============================================================================
// Storage Configuration
// ============================================================================

struct StorageConfig {
    std::string type = "sqlite";           // "sqlite", "rocksdb", "memory"
    std::string path = ":memory:";         // Database path
    int maxConnections = 10;               // Connection pool size
    int connectionTimeout = 30000;         // Timeout in milliseconds
    bool enableWAL = true;                 // Write-Ahead Logging for SQLite
    bool enableForeignKeys = true;         // Foreign key constraints
    int busyTimeout = 5000;                // SQLite busy timeout
    int cacheSize = 2000;                  // SQLite cache size in pages
    std::string journalMode = "WAL";       // SQLite journal mode
    bool syncMode = true;                  // Synchronous writes
    
    // Performance tuning
    int batchSize = 1000;                  // Batch operation size
    bool enableQueryLogging = false;       // Log all queries
    
    StorageConfig() = default;
    StorageConfig(const std::string& p) : path(p) {}
    
    static StorageConfig inMemory() {
        return StorageConfig(":memory:");
    }
    
    static StorageConfig file(const std::string& path) {
        StorageConfig cfg(path);
        cfg.enableWAL = true;
        return cfg;
    }
};

// ============================================================================
// Transaction Interface
// ============================================================================

enum class IsolationLevel {
    DEFERRED,      // Default SQLite behavior
    IMMEDIATE,     // Lock on first write
    EXCLUSIVE      // Lock immediately
};

class Transaction {
public:
    virtual ~Transaction() = default;
    
    virtual bool commit() = 0;
    virtual bool rollback() = 0;
    virtual bool isActive() const = 0;
    
    // Execute within transaction
    virtual StorageResult<ResultSet> execute(
        const std::string& sql, 
        const QueryParams& params = {}) = 0;
};

// RAII transaction scope
class TransactionScope {
public:
    TransactionScope(std::shared_ptr<Transaction> txn);
    ~TransactionScope();
    
    // Non-copyable
    TransactionScope(const TransactionScope&) = delete;
    TransactionScope& operator=(const TransactionScope&) = delete;
    
    // Movable
    TransactionScope(TransactionScope&& other) noexcept;
    TransactionScope& operator=(TransactionScope&& other) noexcept;
    
    void commit();
    void rollback();
    bool isActive() const { return active_ && txn_ && txn_->isActive(); }
    
    StorageResult<ResultSet> execute(
        const std::string& sql, 
        const QueryParams& params = {});
    
private:
    std::shared_ptr<Transaction> txn_;
    bool active_ = true;
    bool committed_ = false;
};

// ============================================================================
// Abstract Storage Backend
// ============================================================================

class StorageBackend {
public:
    virtual ~StorageBackend() = default;
    
    // Connection management
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Query execution
    virtual StorageResult<ResultSet> query(
        const std::string& sql, 
        const QueryParams& params = {}) = 0;
    
    virtual StorageResult<void> execute(
        const std::string& sql, 
        const QueryParams& params = {}) = 0;
    
    // Batch operations
    virtual StorageResult<void> executeBatch(
        const std::string& sql,
        const std::vector<QueryParams>& paramBatches) = 0;
    
    // Transaction support
    virtual std::shared_ptr<Transaction> beginTransaction(
        IsolationLevel level = IsolationLevel::DEFERRED) = 0;
    
    // Schema operations
    virtual StorageResult<void> createTable(
        const std::string& name,
        const std::vector<std::string>& columns) = 0;
    
    virtual StorageResult<void> dropTable(const std::string& name) = 0;
    
    virtual bool tableExists(const std::string& name) = 0;
    
    virtual std::vector<std::string> listTables() = 0;
    
    // Utility
    virtual int64_t lastInsertRowId() = 0;
    virtual int64_t changes() = 0;
    virtual std::string escapeString(const std::string& str) = 0;
    
    // Async operations
    virtual std::future<StorageResult<ResultSet>> queryAsync(
        const std::string& sql,
        const QueryParams& params = {}) = 0;
    
    virtual std::future<StorageResult<void>> executeAsync(
        const std::string& sql,
        const QueryParams& params = {}) = 0;
    
    // Backend info
    virtual std::string getBackendType() const = 0;
    virtual std::string getVersion() const = 0;
};

// ============================================================================
// SQLite Backend Implementation
// ============================================================================

class SQLiteBackend : public StorageBackend {
public:
    explicit SQLiteBackend(const StorageConfig& config);
    ~SQLiteBackend() override;
    
    // Non-copyable
    SQLiteBackend(const SQLiteBackend&) = delete;
    SQLiteBackend& operator=(const SQLiteBackend&) = delete;
    
    // Connection management
    bool connect() override;
    bool disconnect() override;
    bool isConnected() const override;
    
    // Query execution
    StorageResult<ResultSet> query(
        const std::string& sql, 
        const QueryParams& params = {}) override;
    
    StorageResult<void> execute(
        const std::string& sql, 
        const QueryParams& params = {}) override;
    
    // Batch operations
    StorageResult<void> executeBatch(
        const std::string& sql,
        const std::vector<QueryParams>& paramBatches) override;
    
    // Transaction support
    std::shared_ptr<Transaction> beginTransaction(
        IsolationLevel level = IsolationLevel::DEFERRED) override;
    
    // Schema operations
    StorageResult<void> createTable(
        const std::string& name,
        const std::vector<std::string>& columns) override;
    
    StorageResult<void> dropTable(const std::string& name) override;
    
    bool tableExists(const std::string& name) override;
    
    std::vector<std::string> listTables() override;
    
    // Utility
    int64_t lastInsertRowId() override;
    int64_t changes() override;
    std::string escapeString(const std::string& str) override;
    
    // Async operations
    std::future<StorageResult<ResultSet>> queryAsync(
        const std::string& sql,
        const QueryParams& params = {}) override;
    
    std::future<StorageResult<void>> executeAsync(
        const std::string& sql,
        const QueryParams& params = {}) override;
    
    // Backend info
    std::string getBackendType() const override { return "sqlite"; }
    std::string getVersion() const override;
    
    // SQLite-specific methods
    void vacuum();
    void checkpoint();
    int64_t getFileSize() const;
    
private:
    StorageConfig config_;
    void* db_ = nullptr;  // sqlite3* handle
    mutable std::mutex mutex_;
    bool connected_ = false;
    
    // Helper methods
    void applyPragmas();
    bool prepareStatement(const std::string& sql, void** stmt);
    bool bindParameters(void* stmt, const QueryParams& params);
    ResultSet extractResults(void* stmt);
    std::string getErrorMessage() const;
};

// ============================================================================
// Connection Pool
// ============================================================================

class ConnectionPool {
public:
    ConnectionPool(const StorageConfig& config);
    ~ConnectionPool();
    
    // Connection management
    std::shared_ptr<StorageBackend> acquire();
    void release(std::shared_ptr<StorageBackend> conn);
    
    // Pool status
    size_t availableConnections() const;
    size_t activeConnections() const;
    size_t totalConnections() const;
    
    // Pool management
    void resize(size_t newSize);
    void clear();
    
private:
    StorageConfig config_;
    std::queue<std::shared_ptr<StorageBackend>> available_;
    std::vector<std::shared_ptr<StorageBackend>> all_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool shutdown_ = false;
    
    std::shared_ptr<StorageBackend> createConnection();
};

// ============================================================================
// Memory Persistence Layer
// ============================================================================

/**
 * Provides persistent storage for agent memories
 * Integrates with AgentMemoryManager for durable storage
 */
class MemoryPersistence {
public:
    explicit MemoryPersistence(std::shared_ptr<StorageBackend> backend);
    ~MemoryPersistence();
    
    // Initialize schema
    bool initialize();
    
    // Memory CRUD operations
    StorageResult<void> saveMemory(
        const UUID& memoryId,
        const std::string& content,
        const std::string& agentId,
        const std::string& roomId,
        const std::string& tableName,
        const std::vector<float>& embedding = {},
        const std::string& metadata = "{}");
    
    StorageResult<std::unordered_map<std::string, std::string>> loadMemory(
        const UUID& memoryId,
        const std::string& tableName);
    
    StorageResult<std::vector<std::unordered_map<std::string, std::string>>> 
    loadMemoriesByAgent(
        const std::string& agentId,
        const std::string& tableName,
        int limit = 100);
    
    StorageResult<std::vector<std::unordered_map<std::string, std::string>>> 
    loadMemoriesByRoom(
        const std::string& roomId,
        const std::string& tableName,
        int limit = 100);
    
    StorageResult<void> deleteMemory(
        const UUID& memoryId,
        const std::string& tableName);
    
    StorageResult<void> deleteMemoriesByAgent(
        const std::string& agentId,
        const std::string& tableName);
    
    // Search operations
    StorageResult<std::vector<std::unordered_map<std::string, std::string>>> 
    searchMemories(
        const std::string& query,
        const std::string& tableName,
        int limit = 10);
    
    // Batch operations
    StorageResult<void> saveMemoriesBatch(
        const std::vector<std::tuple<UUID, std::string, std::string, std::string>>& memories,
        const std::string& tableName);
    
    // Statistics
    int64_t getMemoryCount(const std::string& tableName);
    int64_t getMemoryCountByAgent(const std::string& agentId, const std::string& tableName);
    
    // Maintenance
    void vacuum();
    void optimize();
    
private:
    std::shared_ptr<StorageBackend> backend_;
    bool initialized_ = false;
    
    // Table management
    void ensureTable(const std::string& tableName);
    std::unordered_set<std::string> existingTables_;
    std::mutex tablesMutex_;
};

// ============================================================================
// Key-Value Store Abstraction
// ============================================================================

/**
 * Simple key-value store interface for configuration and state
 */
class KeyValueStore {
public:
    explicit KeyValueStore(std::shared_ptr<StorageBackend> backend);
    ~KeyValueStore();
    
    // Initialize schema
    bool initialize();
    
    // Basic operations
    StorageResult<void> set(const std::string& key, const std::string& value);
    StorageResult<std::string> get(const std::string& key);
    StorageResult<void> remove(const std::string& key);
    bool exists(const std::string& key);
    
    // Batch operations
    StorageResult<void> setMany(const std::unordered_map<std::string, std::string>& pairs);
    StorageResult<std::unordered_map<std::string, std::string>> getMany(
        const std::vector<std::string>& keys);
    
    // Prefix operations
    StorageResult<std::vector<std::pair<std::string, std::string>>> getByPrefix(
        const std::string& prefix);
    StorageResult<void> removeByPrefix(const std::string& prefix);
    
    // Expiration support
    StorageResult<void> setWithExpiry(
        const std::string& key, 
        const std::string& value,
        std::chrono::seconds ttl);
    void cleanupExpired();
    
    // Statistics
    int64_t count();
    int64_t countByPrefix(const std::string& prefix);
    
private:
    std::shared_ptr<StorageBackend> backend_;
    bool initialized_ = false;
};

// ============================================================================
// Global Storage Manager
// ============================================================================

class StorageManager {
public:
    static StorageManager& getInstance();
    
    // Initialize with config
    bool initialize(const StorageConfig& config);
    bool shutdown();
    
    // Get backends
    std::shared_ptr<StorageBackend> getBackend();
    std::shared_ptr<ConnectionPool> getPool();
    
    // High-level interfaces
    std::shared_ptr<MemoryPersistence> getMemoryPersistence();
    std::shared_ptr<KeyValueStore> getKeyValueStore();
    
    // Status
    bool isInitialized() const { return initialized_; }
    StorageConfig getConfig() const { return config_; }
    
private:
    StorageManager() = default;
    ~StorageManager() = default;
    
    // Non-copyable
    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;
    
    StorageConfig config_;
    std::shared_ptr<ConnectionPool> pool_;
    std::shared_ptr<MemoryPersistence> memoryPersistence_;
    std::shared_ptr<KeyValueStore> kvStore_;
    bool initialized_ = false;
    std::mutex mutex_;
};

// ============================================================================
// Convenience Functions
// ============================================================================

namespace storage {
    // Initialize global storage
    bool init(const StorageConfig& config = StorageConfig::inMemory());
    bool shutdown();
    
    // Get interfaces
    std::shared_ptr<StorageBackend> backend();
    std::shared_ptr<MemoryPersistence> memories();
    std::shared_ptr<KeyValueStore> kv();
    
    // Quick key-value operations
    bool set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool remove(const std::string& key);
}

} // namespace elizaos
