/**
 * @file persistence.cpp
 * @brief SQLite persistence backend implementation for ElizaOS
 *
 * Implements the authoritative API defined in include/persistence.hpp.
 * All classes live in namespace elizaos (no nested persistence namespace).
 */

#include "elizaos/persistence.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <fstream>

namespace elizaos {

// ============================================================================
// TransactionScope Implementation
// ============================================================================

TransactionScope::TransactionScope(std::shared_ptr<Transaction> txn)
    : txn_(std::move(txn)), active_(true), committed_(false) {}

TransactionScope::~TransactionScope() {
    if (active_ && !committed_ && txn_ && txn_->isActive()) {
        txn_->rollback();
    }
}

TransactionScope::TransactionScope(TransactionScope&& other) noexcept
    : txn_(std::move(other.txn_)), active_(other.active_), committed_(other.committed_) {
    other.active_ = false;
}

TransactionScope& TransactionScope::operator=(TransactionScope&& other) noexcept {
    if (this != &other) {
        txn_ = std::move(other.txn_);
        active_ = other.active_;
        committed_ = other.committed_;
        other.active_ = false;
    }
    return *this;
}

void TransactionScope::commit() {
    if (active_ && txn_ && txn_->isActive()) {
        txn_->commit();
        committed_ = true;
    }
}

void TransactionScope::rollback() {
    if (active_ && txn_ && txn_->isActive()) {
        txn_->rollback();
        active_ = false;
    }
}

StorageResult<ResultSet> TransactionScope::execute(
    const std::string& sql, const QueryParams& params) {
    if (!active_ || !txn_ || !txn_->isActive()) {
        return StorageResult<ResultSet>::fail("Transaction not active");
    }
    return txn_->execute(sql, params);
}

// ============================================================================
// SQLiteTransaction Implementation
// ============================================================================

class SQLiteTransaction : public Transaction {
public:
    SQLiteTransaction(sqlite3* db, IsolationLevel level)
        : db_(db), active_(true) {
        std::string beginSql = "BEGIN";
        switch (level) {
            case IsolationLevel::IMMEDIATE: beginSql = "BEGIN IMMEDIATE"; break;
            case IsolationLevel::EXCLUSIVE: beginSql = "BEGIN EXCLUSIVE"; break;
            default: break;
        }
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, beginSql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string err = errMsg ? errMsg : "Unknown error";
            if (errMsg) sqlite3_free(errMsg);
            active_ = false;
            throw std::runtime_error("Failed to begin transaction: " + err);
        }
    }

    bool commit() override {
        if (!active_) return false;
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, "COMMIT", nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);
        active_ = false;
        return rc == SQLITE_OK;
    }

    bool rollback() override {
        if (!active_) return false;
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);
        active_ = false;
        return rc == SQLITE_OK;
    }

    bool isActive() const override { return active_; }

    StorageResult<ResultSet> execute(
        const std::string& sql, const QueryParams& params) override {
        if (!active_) return StorageResult<ResultSet>::fail("Transaction not active");

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            return StorageResult<ResultSet>::fail(sqlite3_errmsg(db_));
        }

        // Bind parameters
        for (size_t i = 0; i < params.size(); ++i) {
            int idx = static_cast<int>(i) + 1;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    sqlite3_bind_null(stmt, idx);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    sqlite3_bind_int64(stmt, idx, arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    sqlite3_bind_double(stmt, idx, arg);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    sqlite3_bind_text(stmt, idx, arg.c_str(), -1, SQLITE_TRANSIENT);
                } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                    sqlite3_bind_blob(stmt, idx, arg.data(), static_cast<int>(arg.size()), SQLITE_TRANSIENT);
                }
            }, params[i]);
        }

        ResultSet results;
        int colCount = sqlite3_column_count(stmt);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            Row row;
            for (int c = 0; c < colCount; ++c) {
                ColumnValue cv;
                cv.name = sqlite3_column_name(stmt, c);
                int type = sqlite3_column_type(stmt, c);
                switch (type) {
                    case SQLITE_NULL: cv.value = nullptr; break;
                    case SQLITE_INTEGER: cv.value = static_cast<int64_t>(sqlite3_column_int64(stmt, c)); break;
                    case SQLITE_FLOAT: cv.value = sqlite3_column_double(stmt, c); break;
                    case SQLITE_TEXT: {
                        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, c));
                        cv.value = std::string(text ? text : "");
                        break;
                    }
                    case SQLITE_BLOB: {
                        const uint8_t* data = static_cast<const uint8_t*>(sqlite3_column_blob(stmt, c));
                        int size = sqlite3_column_bytes(stmt, c);
                        cv.value = std::vector<uint8_t>(data, data + size);
                        break;
                    }
                }
                row.push_back(std::move(cv));
            }
            results.push_back(std::move(row));
        }

        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            return StorageResult<ResultSet>::fail(sqlite3_errmsg(db_));
        }
        return StorageResult<ResultSet>::ok(std::move(results));
    }

private:
    sqlite3* db_;
    bool active_;
};

// ============================================================================
// SQLiteBackend Implementation
// ============================================================================

SQLiteBackend::SQLiteBackend(const StorageConfig& config)
    : config_(config) {}

SQLiteBackend::~SQLiteBackend() {
    disconnect();
}

bool SQLiteBackend::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (connected_) return true;

    sqlite3* db = nullptr;
    int rc = sqlite3_open(config_.path.c_str(), &db);
    if (rc != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return false;
    }
    db_ = db;
    connected_ = true;
    applyPragmas();
    return true;
}

bool SQLiteBackend::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return true;
    if (db_) {
        sqlite3_close(static_cast<sqlite3*>(db_));
        db_ = nullptr;
    }
    connected_ = false;
    return true;
}

bool SQLiteBackend::isConnected() const {
    return connected_;
}

StorageResult<ResultSet> SQLiteBackend::query(
    const std::string& sql, const QueryParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return StorageResult<ResultSet>::fail("Not connected");

    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return StorageResult<ResultSet>::fail(sqlite3_errmsg(db));
    }

    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i) + 1;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                sqlite3_bind_null(stmt, idx);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                sqlite3_bind_int64(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, double>) {
                sqlite3_bind_double(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, arg.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                sqlite3_bind_blob(stmt, idx, arg.data(), static_cast<int>(arg.size()), SQLITE_TRANSIENT);
            }
        }, params[i]);
    }

    ResultSet results;
    int colCount = sqlite3_column_count(stmt);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Row row;
        for (int c = 0; c < colCount; ++c) {
            ColumnValue cv;
            cv.name = sqlite3_column_name(stmt, c);
            int type = sqlite3_column_type(stmt, c);
            switch (type) {
                case SQLITE_NULL: cv.value = nullptr; break;
                case SQLITE_INTEGER: cv.value = static_cast<int64_t>(sqlite3_column_int64(stmt, c)); break;
                case SQLITE_FLOAT: cv.value = sqlite3_column_double(stmt, c); break;
                case SQLITE_TEXT: {
                    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, c));
                    cv.value = std::string(text ? text : "");
                    break;
                }
                case SQLITE_BLOB: {
                    const uint8_t* data = static_cast<const uint8_t*>(sqlite3_column_blob(stmt, c));
                    int size = sqlite3_column_bytes(stmt, c);
                    cv.value = std::vector<uint8_t>(data, data + size);
                    break;
                }
            }
            row.push_back(std::move(cv));
        }
        results.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        return StorageResult<ResultSet>::fail(sqlite3_errmsg(db));
    }
    return StorageResult<ResultSet>::ok(std::move(results));
}

StorageResult<void> SQLiteBackend::execute(
    const std::string& sql, const QueryParams& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return StorageResult<void>::fail("Not connected");

    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return StorageResult<void>::fail(sqlite3_errmsg(db));
    }

    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i) + 1;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                sqlite3_bind_null(stmt, idx);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                sqlite3_bind_int64(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, double>) {
                sqlite3_bind_double(stmt, idx, arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                sqlite3_bind_text(stmt, idx, arg.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                sqlite3_bind_blob(stmt, idx, arg.data(), static_cast<int>(arg.size()), SQLITE_TRANSIENT);
            }
        }, params[i]);
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        return StorageResult<void>::fail(sqlite3_errmsg(db));
    }
    return StorageResult<void>::ok(static_cast<int64_t>(sqlite3_changes(db)));
}

StorageResult<void> SQLiteBackend::executeBatch(
    const std::string& sql, const std::vector<QueryParams>& paramBatches) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return StorageResult<void>::fail("Not connected");

    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return StorageResult<void>::fail(sqlite3_errmsg(db));
    }

    int64_t totalChanges = 0;
    for (const auto& params : paramBatches) {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        for (size_t i = 0; i < params.size(); ++i) {
            int idx = static_cast<int>(i) + 1;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    sqlite3_bind_null(stmt, idx);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    sqlite3_bind_int64(stmt, idx, arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    sqlite3_bind_double(stmt, idx, arg);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    sqlite3_bind_text(stmt, idx, arg.c_str(), -1, SQLITE_TRANSIENT);
                } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                    sqlite3_bind_blob(stmt, idx, arg.data(), static_cast<int>(arg.size()), SQLITE_TRANSIENT);
                }
            }, params[i]);
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return StorageResult<void>::fail(sqlite3_errmsg(db));
        }
        totalChanges += sqlite3_changes(db);
    }

    sqlite3_finalize(stmt);
    return StorageResult<void>::ok(totalChanges);
}

std::shared_ptr<Transaction> SQLiteBackend::beginTransaction(IsolationLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return nullptr;
    try {
        return std::make_shared<SQLiteTransaction>(static_cast<sqlite3*>(db_), level);
    } catch (...) {
        return nullptr;
    }
}

StorageResult<void> SQLiteBackend::createTable(
    const std::string& name, const std::vector<std::string>& columns) {
    std::ostringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS " << name << " (";
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) sql << ", ";
        sql << columns[i];
    }
    sql << ")";
    return execute(sql.str());
}

StorageResult<void> SQLiteBackend::dropTable(const std::string& name) {
    return execute("DROP TABLE IF EXISTS " + name);
}

bool SQLiteBackend::tableExists(const std::string& name) {
    auto result = query(
        "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?",
        {name});
    if (result.success && result.value.has_value() && !result.value->empty()) {
        auto& row = result.value->front();
        if (!row.empty()) {
            return row[0].asInt() > 0;
        }
    }
    return false;
}

std::vector<std::string> SQLiteBackend::listTables() {
    std::vector<std::string> tables;
    auto result = query("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
    if (result.success && result.value.has_value()) {
        for (const auto& row : *result.value) {
            if (!row.empty()) {
                tables.push_back(row[0].asString());
            }
        }
    }
    return tables;
}

int64_t SQLiteBackend::lastInsertRowId() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return 0;
    return sqlite3_last_insert_rowid(static_cast<sqlite3*>(db_));
}

int64_t SQLiteBackend::changes() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!connected_) return 0;
    return sqlite3_changes(static_cast<sqlite3*>(db_));
}

std::string SQLiteBackend::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 2);
    for (char c : str) {
        if (c == '\'') result += "''";
        else result += c;
    }
    return result;
}

std::future<StorageResult<ResultSet>> SQLiteBackend::queryAsync(
    const std::string& sql, const QueryParams& params) {
    return std::async(std::launch::async, [this, sql, params]() {
        return this->query(sql, params);
    });
}

std::future<StorageResult<void>> SQLiteBackend::executeAsync(
    const std::string& sql, const QueryParams& params) {
    return std::async(std::launch::async, [this, sql, params]() {
        return this->execute(sql, params);
    });
}

std::string SQLiteBackend::getVersion() const {
    return sqlite3_libversion();
}

void SQLiteBackend::vacuum() {
    execute("VACUUM");
}

void SQLiteBackend::checkpoint() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (connected_) {
        sqlite3_wal_checkpoint_v2(static_cast<sqlite3*>(db_), nullptr, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    }
}

int64_t SQLiteBackend::getFileSize() const {
    if (config_.path == ":memory:") return 0;
    std::ifstream file(config_.path, std::ios::binary | std::ios::ate);
    return file.is_open() ? static_cast<int64_t>(file.tellg()) : 0;
}

void SQLiteBackend::applyPragmas() {
    sqlite3* db = static_cast<sqlite3*>(db_);
    if (config_.enableWAL) {
        sqlite3_exec(db, "PRAGMA journal_mode=WAL", nullptr, nullptr, nullptr);
    }
    if (config_.enableForeignKeys) {
        sqlite3_exec(db, "PRAGMA foreign_keys=ON", nullptr, nullptr, nullptr);
    }
    std::string busyTimeout = "PRAGMA busy_timeout=" + std::to_string(config_.busyTimeout);
    sqlite3_exec(db, busyTimeout.c_str(), nullptr, nullptr, nullptr);
    std::string cacheSize = "PRAGMA cache_size=" + std::to_string(config_.cacheSize);
    sqlite3_exec(db, cacheSize.c_str(), nullptr, nullptr, nullptr);
}

bool SQLiteBackend::prepareStatement(const std::string& sql, void** stmt) {
    sqlite3* db = static_cast<sqlite3*>(db_);
    return sqlite3_prepare_v2(db, sql.c_str(), -1,
        reinterpret_cast<sqlite3_stmt**>(stmt), nullptr) == SQLITE_OK;
}

bool SQLiteBackend::bindParameters(void* stmt, const QueryParams& params) {
    sqlite3_stmt* s = static_cast<sqlite3_stmt*>(stmt);
    for (size_t i = 0; i < params.size(); ++i) {
        int idx = static_cast<int>(i) + 1;
        bool ok = true;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                ok = sqlite3_bind_null(s, idx) == SQLITE_OK;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                ok = sqlite3_bind_int64(s, idx, arg) == SQLITE_OK;
            } else if constexpr (std::is_same_v<T, double>) {
                ok = sqlite3_bind_double(s, idx, arg) == SQLITE_OK;
            } else if constexpr (std::is_same_v<T, std::string>) {
                ok = sqlite3_bind_text(s, idx, arg.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                ok = sqlite3_bind_blob(s, idx, arg.data(), static_cast<int>(arg.size()), SQLITE_TRANSIENT) == SQLITE_OK;
            }
        }, params[i]);
        if (!ok) return false;
    }
    return true;
}

ResultSet SQLiteBackend::extractResults(void* stmt_raw) {
    sqlite3_stmt* stmt = static_cast<sqlite3_stmt*>(stmt_raw);
    ResultSet results;
    int colCount = sqlite3_column_count(stmt);
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Row row;
        for (int c = 0; c < colCount; ++c) {
            ColumnValue cv;
            cv.name = sqlite3_column_name(stmt, c);
            int type = sqlite3_column_type(stmt, c);
            switch (type) {
                case SQLITE_NULL: cv.value = nullptr; break;
                case SQLITE_INTEGER: cv.value = static_cast<int64_t>(sqlite3_column_int64(stmt, c)); break;
                case SQLITE_FLOAT: cv.value = sqlite3_column_double(stmt, c); break;
                case SQLITE_TEXT: {
                    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, c));
                    cv.value = std::string(text ? text : "");
                    break;
                }
                case SQLITE_BLOB: {
                    const uint8_t* data = static_cast<const uint8_t*>(sqlite3_column_blob(stmt, c));
                    int size = sqlite3_column_bytes(stmt, c);
                    cv.value = std::vector<uint8_t>(data, data + size);
                    break;
                }
            }
            row.push_back(std::move(cv));
        }
        results.push_back(std::move(row));
    }
    return results;
}

std::string SQLiteBackend::getErrorMessage() const {
    if (!db_) return "No database connection";
    return sqlite3_errmsg(static_cast<sqlite3*>(db_));
}

// ============================================================================
// ConnectionPool Implementation
// ============================================================================

ConnectionPool::ConnectionPool(const StorageConfig& config)
    : config_(config) {
    size_t poolSize = static_cast<size_t>(config.maxConnections > 0 ? config.maxConnections : 4);
    for (size_t i = 0; i < poolSize; ++i) {
        auto conn = createConnection();
        if (conn) {
            all_.push_back(conn);
            available_.push(conn);
        }
    }
}

ConnectionPool::~ConnectionPool() {
    clear();
}

std::shared_ptr<StorageBackend> ConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (shutdown_) return nullptr;
    
    cv_.wait(lock, [this]() { return !available_.empty() || shutdown_; });
    if (shutdown_ || available_.empty()) return nullptr;
    
    auto conn = available_.front();
    available_.pop();
    return conn;
}

void ConnectionPool::release(std::shared_ptr<StorageBackend> conn) {
    if (!conn) return;
    std::lock_guard<std::mutex> lock(mutex_);
    if (!shutdown_) {
        available_.push(conn);
        cv_.notify_one();
    }
}

size_t ConnectionPool::availableConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return available_.size();
}

size_t ConnectionPool::activeConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return all_.size() - available_.size();
}

size_t ConnectionPool::totalConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return all_.size();
}

void ConnectionPool::resize(size_t newSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    while (all_.size() < newSize) {
        auto conn = createConnection();
        if (conn) {
            all_.push_back(conn);
            available_.push(conn);
        }
    }
}

void ConnectionPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    shutdown_ = true;
    while (!available_.empty()) available_.pop();
    for (auto& conn : all_) {
        if (conn) conn->disconnect();
    }
    all_.clear();
    cv_.notify_all();
}

std::shared_ptr<StorageBackend> ConnectionPool::createConnection() {
    auto backend = std::make_shared<SQLiteBackend>(config_);
    if (backend->connect()) {
        return backend;
    }
    return nullptr;
}

// ============================================================================
// MemoryPersistence Implementation
// ============================================================================

MemoryPersistence::MemoryPersistence(std::shared_ptr<StorageBackend> backend)
    : backend_(std::move(backend)) {}

MemoryPersistence::~MemoryPersistence() = default;

bool MemoryPersistence::initialize() {
    if (initialized_) return true;
    if (!backend_ || !backend_->isConnected()) return false;
    initialized_ = true;
    return true;
}

void MemoryPersistence::ensureTable(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(tablesMutex_);
    if (existingTables_.count(tableName)) return;
    
    backend_->execute(
        "CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "id TEXT PRIMARY KEY, "
        "content TEXT NOT NULL, "
        "agent_id TEXT NOT NULL, "
        "room_id TEXT, "
        "embedding BLOB, "
        "metadata TEXT DEFAULT '{}', "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")");
    
    backend_->execute("CREATE INDEX IF NOT EXISTS idx_" + tableName + "_agent ON " + tableName + "(agent_id)");
    backend_->execute("CREATE INDEX IF NOT EXISTS idx_" + tableName + "_room ON " + tableName + "(room_id)");
    existingTables_.insert(tableName);
}

StorageResult<void> MemoryPersistence::saveMemory(
    const UUID& memoryId, const std::string& content,
    const std::string& agentId, const std::string& roomId,
    const std::string& tableName, const std::vector<float>& embedding,
    const std::string& metadata) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    ensureTable(tableName);
    
    std::vector<uint8_t> embBlob;
    if (!embedding.empty()) {
        embBlob.resize(embedding.size() * sizeof(float));
        std::memcpy(embBlob.data(), embedding.data(), embBlob.size());
    }
    
    return backend_->execute(
        "INSERT OR REPLACE INTO " + tableName + " (id, content, agent_id, room_id, embedding, metadata) VALUES (?, ?, ?, ?, ?, ?)",
        {memoryId, content, agentId, roomId,
         embBlob.empty() ? QueryParam(nullptr) : QueryParam(embBlob),
         metadata});
}

StorageResult<std::unordered_map<std::string, std::string>> MemoryPersistence::loadMemory(
    const UUID& memoryId, const std::string& tableName) {
    if (!initialized_) return StorageResult<std::unordered_map<std::string, std::string>>::fail("Not initialized");
    ensureTable(tableName);
    
    auto result = backend_->query(
        "SELECT id, content, agent_id, room_id, metadata FROM " + tableName + " WHERE id = ?",
        {memoryId});
    
    if (!result.success) return StorageResult<std::unordered_map<std::string, std::string>>::fail(result.error.value_or("Query failed"));
    if (!result.value.has_value() || result.value->empty()) {
        return StorageResult<std::unordered_map<std::string, std::string>>::fail("Memory not found");
    }
    
    std::unordered_map<std::string, std::string> mem;
    for (const auto& col : result.value->front()) {
        mem[col.name] = col.asString();
    }
    return StorageResult<std::unordered_map<std::string, std::string>>::ok(std::move(mem));
}

StorageResult<std::vector<std::unordered_map<std::string, std::string>>>
MemoryPersistence::loadMemoriesByAgent(
    const std::string& agentId, const std::string& tableName, int limit) {
    if (!initialized_) return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::fail("Not initialized");
    ensureTable(tableName);
    
    auto result = backend_->query(
        "SELECT id, content, agent_id, room_id, metadata FROM " + tableName +
        " WHERE agent_id = ? ORDER BY created_at DESC LIMIT ?",
        {agentId, static_cast<int64_t>(limit)});
    
    if (!result.success) return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::fail(result.error.value_or("Query failed"));
    
    std::vector<std::unordered_map<std::string, std::string>> memories;
    if (result.value.has_value()) {
        for (const auto& row : *result.value) {
            std::unordered_map<std::string, std::string> mem;
            for (const auto& col : row) {
                mem[col.name] = col.asString();
            }
            memories.push_back(std::move(mem));
        }
    }
    return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::ok(std::move(memories));
}

StorageResult<std::vector<std::unordered_map<std::string, std::string>>>
MemoryPersistence::loadMemoriesByRoom(
    const std::string& roomId, const std::string& tableName, int limit) {
    if (!initialized_) return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::fail("Not initialized");
    ensureTable(tableName);
    
    auto result = backend_->query(
        "SELECT id, content, agent_id, room_id, metadata FROM " + tableName +
        " WHERE room_id = ? ORDER BY created_at DESC LIMIT ?",
        {roomId, static_cast<int64_t>(limit)});
    
    if (!result.success) return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::fail(result.error.value_or("Query failed"));
    
    std::vector<std::unordered_map<std::string, std::string>> memories;
    if (result.value.has_value()) {
        for (const auto& row : *result.value) {
            std::unordered_map<std::string, std::string> mem;
            for (const auto& col : row) {
                mem[col.name] = col.asString();
            }
            memories.push_back(std::move(mem));
        }
    }
    return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::ok(std::move(memories));
}

StorageResult<void> MemoryPersistence::deleteMemory(
    const UUID& memoryId, const std::string& tableName) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    ensureTable(tableName);
    return backend_->execute("DELETE FROM " + tableName + " WHERE id = ?", {memoryId});
}

StorageResult<void> MemoryPersistence::deleteMemoriesByAgent(
    const std::string& agentId, const std::string& tableName) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    ensureTable(tableName);
    return backend_->execute("DELETE FROM " + tableName + " WHERE agent_id = ?", {agentId});
}

StorageResult<std::vector<std::unordered_map<std::string, std::string>>>
MemoryPersistence::searchMemories(
    const std::string& query, const std::string& tableName, int limit) {
    if (!initialized_) return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::fail("Not initialized");
    ensureTable(tableName);
    
    auto result = backend_->query(
        "SELECT id, content, agent_id, room_id, metadata FROM " + tableName +
        " WHERE content LIKE ? ORDER BY created_at DESC LIMIT ?",
        {std::string("%" + query + "%"), static_cast<int64_t>(limit)});
    
    if (!result.success) return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::fail(result.error.value_or("Query failed"));
    
    std::vector<std::unordered_map<std::string, std::string>> memories;
    if (result.value.has_value()) {
        for (const auto& row : *result.value) {
            std::unordered_map<std::string, std::string> mem;
            for (const auto& col : row) {
                mem[col.name] = col.asString();
            }
            memories.push_back(std::move(mem));
        }
    }
    return StorageResult<std::vector<std::unordered_map<std::string, std::string>>>::ok(std::move(memories));
}

StorageResult<void> MemoryPersistence::saveMemoriesBatch(
    const std::vector<std::tuple<UUID, std::string, std::string, std::string>>& memories,
    const std::string& tableName) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    ensureTable(tableName);
    
    std::vector<QueryParams> batches;
    for (const auto& [id, content, agentId, roomId] : memories) {
        batches.push_back({id, content, agentId, roomId, nullptr, std::string("{}")});
    }
    
    return backend_->executeBatch(
        "INSERT OR REPLACE INTO " + tableName + " (id, content, agent_id, room_id, embedding, metadata) VALUES (?, ?, ?, ?, ?, ?)",
        batches);
}

int64_t MemoryPersistence::getMemoryCount(const std::string& tableName) {
    ensureTable(tableName);
    auto result = backend_->query("SELECT COUNT(*) FROM " + tableName);
    if (result.success && result.value.has_value() && !result.value->empty()) {
        return result.value->front()[0].asInt();
    }
    return 0;
}

int64_t MemoryPersistence::getMemoryCountByAgent(const std::string& agentId, const std::string& tableName) {
    ensureTable(tableName);
    auto result = backend_->query("SELECT COUNT(*) FROM " + tableName + " WHERE agent_id = ?", {agentId});
    if (result.success && result.value.has_value() && !result.value->empty()) {
        return result.value->front()[0].asInt();
    }
    return 0;
}

void MemoryPersistence::vacuum() {
    if (backend_) backend_->execute("VACUUM");
}

void MemoryPersistence::optimize() {
    if (backend_) backend_->execute("ANALYZE");
}

// ============================================================================
// KeyValueStore Implementation
// ============================================================================

KeyValueStore::KeyValueStore(std::shared_ptr<StorageBackend> backend)
    : backend_(std::move(backend)) {}

KeyValueStore::~KeyValueStore() = default;

bool KeyValueStore::initialize() {
    if (initialized_) return true;
    if (!backend_ || !backend_->isConnected()) return false;
    
    auto result = backend_->execute(
        "CREATE TABLE IF NOT EXISTS kv_store ("
        "key TEXT PRIMARY KEY, "
        "value TEXT NOT NULL, "
        "expires_at DATETIME DEFAULT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")");
    
    if (!result.success) return false;
    initialized_ = true;
    return true;
}

StorageResult<void> KeyValueStore::set(const std::string& key, const std::string& value) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    return backend_->execute(
        "INSERT OR REPLACE INTO kv_store (key, value, updated_at) VALUES (?, ?, CURRENT_TIMESTAMP)",
        {key, value});
}

StorageResult<std::string> KeyValueStore::get(const std::string& key) {
    if (!initialized_) return StorageResult<std::string>::fail("Not initialized");
    auto result = backend_->query(
        "SELECT value FROM kv_store WHERE key = ? AND (expires_at IS NULL OR expires_at > CURRENT_TIMESTAMP)",
        {key});
    if (!result.success) return StorageResult<std::string>::fail(result.error.value_or("Query failed"));
    if (!result.value.has_value() || result.value->empty()) {
        return StorageResult<std::string>::fail("Key not found");
    }
    return StorageResult<std::string>::ok(result.value->front()[0].asString());
}

StorageResult<void> KeyValueStore::remove(const std::string& key) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    return backend_->execute("DELETE FROM kv_store WHERE key = ?", {key});
}

bool KeyValueStore::exists(const std::string& key) {
    if (!initialized_) return false;
    auto result = backend_->query(
        "SELECT 1 FROM kv_store WHERE key = ? AND (expires_at IS NULL OR expires_at > CURRENT_TIMESTAMP)",
        {key});
    return result.success && result.value.has_value() && !result.value->empty();
}

StorageResult<void> KeyValueStore::setMany(const std::unordered_map<std::string, std::string>& pairs) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    std::vector<QueryParams> batches;
    for (const auto& [k, v] : pairs) {
        batches.push_back({k, v});
    }
    return backend_->executeBatch(
        "INSERT OR REPLACE INTO kv_store (key, value, updated_at) VALUES (?, ?, CURRENT_TIMESTAMP)",
        batches);
}

StorageResult<std::unordered_map<std::string, std::string>> KeyValueStore::getMany(
    const std::vector<std::string>& keys) {
    if (!initialized_) return StorageResult<std::unordered_map<std::string, std::string>>::fail("Not initialized");
    std::unordered_map<std::string, std::string> results;
    for (const auto& key : keys) {
        auto r = get(key);
        if (r.success && r.value.has_value()) {
            results[key] = *r.value;
        }
    }
    return StorageResult<std::unordered_map<std::string, std::string>>::ok(std::move(results));
}

StorageResult<std::vector<std::pair<std::string, std::string>>> KeyValueStore::getByPrefix(
    const std::string& prefix) {
    if (!initialized_) return StorageResult<std::vector<std::pair<std::string, std::string>>>::fail("Not initialized");
    auto result = backend_->query(
        "SELECT key, value FROM kv_store WHERE key LIKE ? AND (expires_at IS NULL OR expires_at > CURRENT_TIMESTAMP)",
        {prefix + "%"});
    if (!result.success) return StorageResult<std::vector<std::pair<std::string, std::string>>>::fail(result.error.value_or("Query failed"));
    
    std::vector<std::pair<std::string, std::string>> pairs;
    if (result.value.has_value()) {
        for (const auto& row : *result.value) {
            if (row.size() >= 2) {
                pairs.emplace_back(row[0].asString(), row[1].asString());
            }
        }
    }
    return StorageResult<std::vector<std::pair<std::string, std::string>>>::ok(std::move(pairs));
}

StorageResult<void> KeyValueStore::removeByPrefix(const std::string& prefix) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    return backend_->execute("DELETE FROM kv_store WHERE key LIKE ?", {prefix + "%"});
}

StorageResult<void> KeyValueStore::setWithExpiry(
    const std::string& key, const std::string& value, std::chrono::seconds ttl) {
    if (!initialized_) return StorageResult<void>::fail("Not initialized");
    // SQLite's datetime() modifier parser rejects a malformed '+-N seconds'
    // string produced by concatenating a negative offset, which would silently
    // leave expires_at NULL and make the entry effectively permanent. Build the
    // signed modifier explicitly so non-positive TTLs deterministically expire
    // the key immediately (offset <= 0 -> already in the past).
    const int64_t seconds = static_cast<int64_t>(ttl.count());
    const std::string modifier =
        (seconds >= 0 ? std::string("+") : std::string("-")) +
        std::to_string(seconds >= 0 ? seconds : -seconds) + " seconds";
    return backend_->execute(
        "INSERT OR REPLACE INTO kv_store (key, value, expires_at, updated_at) VALUES (?, ?, datetime('now', ?), CURRENT_TIMESTAMP)",
        {key, value, modifier});
}

void KeyValueStore::cleanupExpired() {
    if (!initialized_) return;
    backend_->execute("DELETE FROM kv_store WHERE expires_at IS NOT NULL AND expires_at <= CURRENT_TIMESTAMP");
}

int64_t KeyValueStore::count() {
    if (!initialized_) return 0;
    auto result = backend_->query("SELECT COUNT(*) FROM kv_store WHERE expires_at IS NULL OR expires_at > CURRENT_TIMESTAMP");
    if (result.success && result.value.has_value() && !result.value->empty()) {
        return result.value->front()[0].asInt();
    }
    return 0;
}

int64_t KeyValueStore::countByPrefix(const std::string& prefix) {
    if (!initialized_) return 0;
    auto result = backend_->query(
        "SELECT COUNT(*) FROM kv_store WHERE key LIKE ? AND (expires_at IS NULL OR expires_at > CURRENT_TIMESTAMP)",
        {prefix + "%"});
    if (result.success && result.value.has_value() && !result.value->empty()) {
        return result.value->front()[0].asInt();
    }
    return 0;
}

// ============================================================================
// StorageManager Implementation (Singleton)
// ============================================================================

StorageManager& StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}

bool StorageManager::initialize(const StorageConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    
    config_ = config;
    pool_ = std::make_shared<ConnectionPool>(config);
    
    if (pool_->availableConnections() == 0) return false;
    
    auto conn = pool_->acquire();
    if (!conn) return false;
    
    memoryPersistence_ = std::make_shared<MemoryPersistence>(conn);
    if (!memoryPersistence_->initialize()) {
        pool_->release(conn);
        return false;
    }
    
    kvStore_ = std::make_shared<KeyValueStore>(conn);
    if (!kvStore_->initialize()) {
        pool_->release(conn);
        return false;
    }
    
    pool_->release(conn);
    initialized_ = true;
    return true;
}

bool StorageManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryPersistence_.reset();
    kvStore_.reset();
    if (pool_) {
        pool_->clear();
        pool_.reset();
    }
    initialized_ = false;
    return true;
}

std::shared_ptr<StorageBackend> StorageManager::getBackend() {
    if (!pool_) return nullptr;
    return pool_->acquire();
}

std::shared_ptr<ConnectionPool> StorageManager::getPool() {
    return pool_;
}

std::shared_ptr<MemoryPersistence> StorageManager::getMemoryPersistence() {
    return memoryPersistence_;
}

std::shared_ptr<KeyValueStore> StorageManager::getKeyValueStore() {
    return kvStore_;
}

// ============================================================================
// Convenience Functions
// ============================================================================

namespace storage {

bool init(const StorageConfig& config) {
    return StorageManager::getInstance().initialize(config);
}

bool shutdown() {
    return StorageManager::getInstance().shutdown();
}

std::shared_ptr<StorageBackend> backend() {
    return StorageManager::getInstance().getBackend();
}

std::shared_ptr<MemoryPersistence> memories() {
    return StorageManager::getInstance().getMemoryPersistence();
}

std::shared_ptr<KeyValueStore> kv() {
    return StorageManager::getInstance().getKeyValueStore();
}

bool set(const std::string& key, const std::string& value) {
    auto store = StorageManager::getInstance().getKeyValueStore();
    if (!store) return false;
    return store->set(key, value).success;
}

std::optional<std::string> get(const std::string& key) {
    auto store = StorageManager::getInstance().getKeyValueStore();
    if (!store) return std::nullopt;
    auto result = store->get(key);
    if (result.success && result.value.has_value()) return *result.value;
    return std::nullopt;
}

bool remove(const std::string& key) {
    auto store = StorageManager::getInstance().getKeyValueStore();
    if (!store) return false;
    return store->remove(key).success;
}

} // namespace storage
} // namespace elizaos
