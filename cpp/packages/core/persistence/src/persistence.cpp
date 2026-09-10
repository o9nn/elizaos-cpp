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
#include <atomic>
#include <chrono>
#include <limits>

namespace elizaos {

struct SQLiteDatabaseState {
    mutable std::mutex mutex;
    std::condition_variable cv;
    sqlite3* db = nullptr;
    std::atomic<bool> connected{false};
    bool disconnecting = false;
    bool acceptingAsync = false;
    size_t outstandingAsync = 0;
    std::atomic<bool> disconnectRequested{false};
    std::atomic<size_t> activeTransactions{0};

    ~SQLiteDatabaseState() {
        if (db) {
            // The state is destroyed only after the backend, all accepted
            // asynchronous jobs, and every transaction have released it.
            sqlite3_close_v2(db);
        }
    }
};

namespace {

std::string sqliteError(sqlite3* db, int rc, const std::string& context) {
    const char* detail = db ? sqlite3_errmsg(db) : sqlite3_errstr(rc);
    return context + ": " + (detail ? detail : "unknown SQLite error");
}

int bindOne(sqlite3_stmt* stmt, int index, const QueryParam& param) {
    return std::visit([&](const auto& arg) -> int {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return sqlite3_bind_null(stmt, index);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return sqlite3_bind_int64(stmt, index, arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return sqlite3_bind_double(stmt, index, arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return sqlite3_bind_text(stmt, index, arg.data(),
                                     static_cast<int>(arg.size()), SQLITE_TRANSIENT);
        } else {
            if (arg.empty()) {
                // sqlite3_bind_blob(..., nullptr, 0, ...) binds SQL NULL. A
                // zeroblob preserves the distinction between NULL and BLOB(0).
                return sqlite3_bind_zeroblob(stmt, index, 0);
            }
            if (arg.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
                return SQLITE_TOOBIG;
            }
            return sqlite3_bind_blob(stmt, index, arg.data(),
                                     static_cast<int>(arg.size()), SQLITE_TRANSIENT);
        }
    }, param);
}

bool bindAll(sqlite3* db, sqlite3_stmt* stmt, const QueryParams& params,
             std::string& error) {
    const int expected = sqlite3_bind_parameter_count(stmt);
    if (params.size() != static_cast<size_t>(expected)) {
        error = "Expected " + std::to_string(expected) + " parameters, received " +
                std::to_string(params.size());
        return false;
    }
    for (size_t i = 0; i < params.size(); ++i) {
        const int rc = bindOne(stmt, static_cast<int>(i + 1), params[i]);
        if (rc != SQLITE_OK) {
            error = sqliteError(db, rc,
                "Failed to bind parameter " + std::to_string(i + 1));
            return false;
        }
    }
    return true;
}

ColumnValue readColumn(sqlite3_stmt* stmt, int column) {
    ColumnValue value;
    const char* name = sqlite3_column_name(stmt, column);
    value.name = name ? name : "";
    switch (sqlite3_column_type(stmt, column)) {
        case SQLITE_NULL:
            value.value = nullptr;
            break;
        case SQLITE_INTEGER:
            value.value = static_cast<int64_t>(sqlite3_column_int64(stmt, column));
            break;
        case SQLITE_FLOAT:
            value.value = sqlite3_column_double(stmt, column);
            break;
        case SQLITE_TEXT: {
            const auto* data = sqlite3_column_text(stmt, column);
            const int size = sqlite3_column_bytes(stmt, column);
            value.value = data && size > 0
                ? std::string(reinterpret_cast<const char*>(data), static_cast<size_t>(size))
                : std::string{};
            break;
        }
        case SQLITE_BLOB: {
            const auto* data = static_cast<const uint8_t*>(sqlite3_column_blob(stmt, column));
            const int size = sqlite3_column_bytes(stmt, column);
            value.value = data && size > 0
                ? std::vector<uint8_t>(data, data + size)
                : std::vector<uint8_t>{};
            break;
        }
        default:
            value.value = nullptr;
            break;
    }
    return value;
}

StorageResult<ResultSet> queryLocked(const std::shared_ptr<SQLiteDatabaseState>& state,
                                     const std::string& sql,
                                     const QueryParams& params,
                                     bool acceptedAsync = false) {
    std::lock_guard<std::mutex> lock(state->mutex);
    if (!state->connected || !state->db ||
        (!acceptedAsync && (state->disconnecting ||
                            state->disconnectRequested.load(std::memory_order_acquire)))) {
        return StorageResult<ResultSet>::fail("Not connected");
    }

    sqlite3* db = state->db;
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        const std::string error = sqliteError(db, rc, "Failed to prepare query");
        if (stmt) sqlite3_finalize(stmt);
        return StorageResult<ResultSet>::fail(error);
    }

    std::string error;
    if (!bindAll(db, stmt, params, error)) {
        sqlite3_finalize(stmt);
        return StorageResult<ResultSet>::fail(error);
    }

    ResultSet results;
    const int columnCount = sqlite3_column_count(stmt);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Row row;
        row.reserve(static_cast<size_t>(columnCount));
        for (int column = 0; column < columnCount; ++column) {
            row.push_back(readColumn(stmt, column));
        }
        results.push_back(std::move(row));
    }

    if (rc != SQLITE_DONE) {
        error = sqliteError(db, rc, "Failed to execute query");
    }
    const int finalizeRc = sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        return StorageResult<ResultSet>::fail(error);
    }
    if (finalizeRc != SQLITE_OK) {
        return StorageResult<ResultSet>::fail(
            sqliteError(db, finalizeRc, "Failed to finalize query"));
    }
    return StorageResult<ResultSet>::ok(std::move(results));
}

StorageResult<void> executeLocked(const std::shared_ptr<SQLiteDatabaseState>& state,
                                  const std::string& sql,
                                  const QueryParams& params,
                                  bool acceptedAsync = false) {
    std::lock_guard<std::mutex> lock(state->mutex);
    if (!state->connected || !state->db ||
        (!acceptedAsync && (state->disconnecting ||
                            state->disconnectRequested.load(std::memory_order_acquire)))) {
        return StorageResult<void>::fail("Not connected");
    }

    sqlite3* db = state->db;
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        const std::string error = sqliteError(db, rc, "Failed to prepare statement");
        if (stmt) sqlite3_finalize(stmt);
        return StorageResult<void>::fail(error);
    }

    std::string error;
    if (!bindAll(db, stmt, params, error)) {
        sqlite3_finalize(stmt);
        return StorageResult<void>::fail(error);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        error = sqliteError(db, rc, "Failed to execute statement");
    }
    const int64_t affected = (rc == SQLITE_DONE || rc == SQLITE_ROW)
        ? static_cast<int64_t>(sqlite3_changes(db)) : 0;
    const int finalizeRc = sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        return StorageResult<void>::fail(error);
    }
    if (finalizeRc != SQLITE_OK) {
        return StorageResult<void>::fail(
            sqliteError(db, finalizeRc, "Failed to finalize statement"));
    }
    return StorageResult<void>::ok(affected);
}

bool execSql(sqlite3* db, const std::string& sql, std::string& error) {
    char* rawError = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &rawError);
    if (rc == SQLITE_OK) {
        if (rawError) sqlite3_free(rawError);
        return true;
    }
    error = rawError ? rawError : sqliteError(db, rc, "SQLite execution failed");
    if (rawError) sqlite3_free(rawError);
    return false;
}

bool applyPragmasLocked(sqlite3* db, const StorageConfig& config) {
    std::vector<std::string> pragmas;
    if (config.enableWAL) {
        pragmas.push_back("PRAGMA journal_mode=" + config.journalMode);
    }
    pragmas.push_back(std::string("PRAGMA foreign_keys=") +
                      (config.enableForeignKeys ? "ON" : "OFF"));
    pragmas.push_back("PRAGMA busy_timeout=" + std::to_string(config.busyTimeout));
    pragmas.push_back("PRAGMA cache_size=" + std::to_string(config.cacheSize));
    pragmas.push_back(std::string("PRAGMA synchronous=") +
                      (config.syncMode ? "FULL" : "OFF"));
    for (const auto& pragma : pragmas) {
        std::string error;
        if (!execSql(db, pragma, error)) return false;
    }
    return true;
}

template<typename Result>
std::future<Result> readyFuture(Result result) {
    std::promise<Result> promise;
    promise.set_value(std::move(result));
    return promise.get_future();
}

class AsyncCompletion {
public:
    explicit AsyncCompletion(std::shared_ptr<SQLiteDatabaseState> state)
        : state_(std::move(state)) {}
    ~AsyncCompletion() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->outstandingAsync > 0) --state_->outstandingAsync;
        state_->cv.notify_all();
    }
private:
    std::shared_ptr<SQLiteDatabaseState> state_;
};

} // namespace

// ============================================================================
// TransactionScope Implementation
// ============================================================================

TransactionScope::TransactionScope(std::shared_ptr<Transaction> txn)
    : txn_(std::move(txn)), active_(txn_ && txn_->isActive()), committed_(false) {}

TransactionScope::~TransactionScope() {
    if (active_ && !committed_ && txn_ && txn_->isActive()) {
        txn_->rollback();
    }
}

TransactionScope::TransactionScope(TransactionScope&& other) noexcept
    : txn_(std::move(other.txn_)), active_(other.active_), committed_(other.committed_) {
    other.active_ = false;
    other.committed_ = false;
}

TransactionScope& TransactionScope::operator=(TransactionScope&& other) noexcept {
    if (this != &other) {
        if (active_ && !committed_ && txn_ && txn_->isActive()) {
            txn_->rollback();
        }
        txn_ = std::move(other.txn_);
        active_ = other.active_;
        committed_ = other.committed_;
        other.active_ = false;
        other.committed_ = false;
    }
    return *this;
}

void TransactionScope::commit() {
    if (active_ && txn_ && txn_->isActive() && txn_->commit()) {
        committed_ = true;
        active_ = false;
    }
}

void TransactionScope::rollback() {
    if (active_ && txn_ && txn_->isActive() && txn_->rollback()) {
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
    SQLiteTransaction(std::shared_ptr<SQLiteDatabaseState> state,
                      std::unique_lock<std::mutex>&& databaseLock,
                      IsolationLevel level)
        : state_(std::move(state)), databaseLock_(std::move(databaseLock)) {
        std::string beginSql = "BEGIN";
        switch (level) {
            case IsolationLevel::IMMEDIATE: beginSql = "BEGIN IMMEDIATE"; break;
            case IsolationLevel::EXCLUSIVE: beginSql = "BEGIN EXCLUSIVE"; break;
            default: break;
        }
        std::string error;
        if (!state_->connected || !state_->db || state_->disconnecting ||
            !execSql(state_->db, beginSql, error)) {
            databaseLock_.unlock();
            throw std::runtime_error("Failed to begin transaction: " +
                                     (error.empty() ? "Not connected" : error));
        }
        active_ = true;
        state_->activeTransactions.fetch_add(1, std::memory_order_release);
    }

    ~SQLiteTransaction() override {
        rollback();
    }

    bool commit() override {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (!active_) return false;
        std::string error;
        if (!execSql(state_->db, "COMMIT", error)) {
            // Preserve ownership and activity so the caller or RAII scope can
            // still roll the failed commit back safely.
            return false;
        }
        active_ = false;
        state_->activeTransactions.fetch_sub(1, std::memory_order_release);
        databaseLock_.unlock();
        state_->cv.notify_all();
        return true;
    }

    bool rollback() override {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (!active_) return false;
        std::string error;
        const bool ok = execSql(state_->db, "ROLLBACK", error);
        // Even on a rollback error, relinquish the transaction. SQLite may
        // already have aborted it, and retaining the database lock would
        // permanently deadlock disconnect/destruction.
        active_ = false;
        state_->activeTransactions.fetch_sub(1, std::memory_order_release);
        if (databaseLock_.owns_lock()) databaseLock_.unlock();
        state_->cv.notify_all();
        return ok;
    }

    bool isActive() const override {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        return active_;
    }

    StorageResult<ResultSet> execute(
        const std::string& sql, const QueryParams& params) override {
        std::lock_guard<std::mutex> lock(transactionMutex_);
        if (!active_ || !state_->connected || !state_->db) {
            return StorageResult<ResultSet>::fail("Transaction not active");
        }

        sqlite3* db = state_->db;
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            const std::string error = sqliteError(db, rc, "Failed to prepare transaction statement");
            if (stmt) sqlite3_finalize(stmt);
            return StorageResult<ResultSet>::fail(error);
        }

        std::string error;
        if (!bindAll(db, stmt, params, error)) {
            sqlite3_finalize(stmt);
            return StorageResult<ResultSet>::fail(error);
        }

        ResultSet results;
        const int columnCount = sqlite3_column_count(stmt);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            Row row;
            row.reserve(static_cast<size_t>(columnCount));
            for (int column = 0; column < columnCount; ++column) {
                row.push_back(readColumn(stmt, column));
            }
            results.push_back(std::move(row));
        }
        if (rc != SQLITE_DONE) {
            error = sqliteError(db, rc, "Failed to execute transaction statement");
        }
        const int finalizeRc = sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) return StorageResult<ResultSet>::fail(error);
        if (finalizeRc != SQLITE_OK) {
            return StorageResult<ResultSet>::fail(
                sqliteError(db, finalizeRc, "Failed to finalize transaction statement"));
        }
        return StorageResult<ResultSet>::ok(std::move(results));
    }

private:
    std::shared_ptr<SQLiteDatabaseState> state_;
    std::unique_lock<std::mutex> databaseLock_;
    mutable std::mutex transactionMutex_;
    bool active_ = false;
};

// ============================================================================
// SQLiteBackend Implementation
// ============================================================================

SQLiteBackend::SQLiteBackend(const StorageConfig& config)
    : config_(config), state_(std::make_shared<SQLiteDatabaseState>()) {}

SQLiteBackend::~SQLiteBackend() {
    // Do not call blocking disconnect() here: a transaction can legitimately
    // outlive its backend object and may own the database mutex on this same
    // thread. The shared state closes itself after the last accepted user.
    state_.reset();
}

bool SQLiteBackend::connect() {
    std::lock_guard<std::mutex> lock(state_->mutex);
    if (state_->connected) {
        return !state_->disconnectRequested.load(std::memory_order_acquire);
    }
    if (state_->disconnecting) return false;

    sqlite3* db = nullptr;
    const int rc = sqlite3_open_v2(config_.path.c_str(), &db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
        if (db) sqlite3_close_v2(db);
        return false;
    }
    sqlite3_extended_result_codes(db, 1);
    if (!applyPragmasLocked(db, config_)) {
        sqlite3_close_v2(db);
        return false;
    }

    state_->db = db;
    state_->connected = true;
    state_->acceptingAsync = true;
    state_->disconnectRequested.store(false, std::memory_order_release);
    return true;
}

bool SQLiteBackend::disconnect() {
    state_->disconnectRequested.store(true, std::memory_order_release);
    // A transaction may hold the database mutex on this same thread. Waiting
    // here would self-deadlock, so reject disconnect and leave the handle open.
    if (state_->activeTransactions.load(std::memory_order_acquire) != 0) {
        state_->disconnectRequested.store(false, std::memory_order_release);
        return false;
    }
    std::unique_lock<std::mutex> lock(state_->mutex);
    if (state_->activeTransactions.load(std::memory_order_acquire) != 0) {
        state_->disconnectRequested.store(false, std::memory_order_release);
        return false;
    }
    if (!state_->connected) {
        state_->acceptingAsync = false;
        return true;
    }

    state_->disconnecting = true;
    state_->acceptingAsync = false;
    auto state = state_;
    state_->cv.wait(lock, [&state]() { return state->outstandingAsync == 0; });

    const int rc = sqlite3_close(state_->db);
    if (rc != SQLITE_OK) {
        state_->disconnecting = false;
        state_->acceptingAsync = true;
        state_->disconnectRequested.store(false, std::memory_order_release);
        return false;
    }
    state_->db = nullptr;
    state_->connected = false;
    state_->disconnecting = false;
    state_->cv.notify_all();
    return true;
}

bool SQLiteBackend::isConnected() const {
    return state_->connected.load(std::memory_order_acquire) &&
           !state_->disconnectRequested.load(std::memory_order_acquire);
}

StorageResult<ResultSet> SQLiteBackend::query(
    const std::string& sql, const QueryParams& params) {
    return queryLocked(state_, sql, params);
}

StorageResult<void> SQLiteBackend::execute(
    const std::string& sql, const QueryParams& params) {
    return executeLocked(state_, sql, params);
}

StorageResult<void> SQLiteBackend::executeBatch(
    const std::string& sql, const std::vector<QueryParams>& paramBatches) {
    std::lock_guard<std::mutex> lock(state_->mutex);
    if (!state_->connected || !state_->db || state_->disconnecting ||
        state_->disconnectRequested.load(std::memory_order_acquire)) {
        return StorageResult<void>::fail("Not connected");
    }
    if (paramBatches.empty()) return StorageResult<void>::ok(0);

    sqlite3* db = state_->db;
    std::string error;
    if (!execSql(db, "SAVEPOINT elizaos_execute_batch", error)) {
        return StorageResult<void>::fail("Failed to start atomic batch: " + error);
    }

    auto rollbackBatch = [&]() {
        std::string ignored;
        execSql(db, "ROLLBACK TO elizaos_execute_batch", ignored);
        execSql(db, "RELEASE elizaos_execute_batch", ignored);
    };

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        error = sqliteError(db, rc, "Failed to prepare batch statement");
        if (stmt) sqlite3_finalize(stmt);
        rollbackBatch();
        return StorageResult<void>::fail(error);
    }

    int64_t totalChanges = 0;
    for (const auto& params : paramBatches) {
        rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            error = sqliteError(db, rc, "Failed to reset batch statement");
            sqlite3_finalize(stmt);
            rollbackBatch();
            return StorageResult<void>::fail(error);
        }
        rc = sqlite3_clear_bindings(stmt);
        if (rc != SQLITE_OK) {
            error = sqliteError(db, rc, "Failed to clear batch bindings");
            sqlite3_finalize(stmt);
            rollbackBatch();
            return StorageResult<void>::fail(error);
        }
        if (!bindAll(db, stmt, params, error)) {
            sqlite3_finalize(stmt);
            rollbackBatch();
            return StorageResult<void>::fail(error);
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            error = sqliteError(db, rc, "Failed to execute batch item");
            sqlite3_finalize(stmt);
            rollbackBatch();
            return StorageResult<void>::fail(error);
        }
        totalChanges += static_cast<int64_t>(sqlite3_changes(db));
    }

    const int finalizeRc = sqlite3_finalize(stmt);
    if (finalizeRc != SQLITE_OK) {
        error = sqliteError(db, finalizeRc, "Failed to finalize batch statement");
        rollbackBatch();
        return StorageResult<void>::fail(error);
    }
    if (!execSql(db, "RELEASE elizaos_execute_batch", error)) {
        rollbackBatch();
        return StorageResult<void>::fail("Failed to commit atomic batch: " + error);
    }
    return StorageResult<void>::ok(totalChanges);
}

std::shared_ptr<Transaction> SQLiteBackend::beginTransaction(IsolationLevel level) {
    std::unique_lock<std::mutex> lock(state_->mutex);
    if (!state_->connected || !state_->db || state_->disconnecting ||
        state_->disconnectRequested.load(std::memory_order_acquire)) return nullptr;
    try {
        return std::make_shared<SQLiteTransaction>(state_, std::move(lock), level);
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
        "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?", {name});
    return result.success && result.value && !result.value->empty() &&
           !result.value->front().empty() && result.value->front()[0].asInt() > 0;
}

std::vector<std::string> SQLiteBackend::listTables() {
    std::vector<std::string> tables;
    auto result = query("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name");
    if (result.success && result.value) {
        for (const auto& row : *result.value) {
            if (!row.empty()) tables.push_back(row[0].asString());
        }
    }
    return tables;
}

int64_t SQLiteBackend::lastInsertRowId() {
    std::lock_guard<std::mutex> lock(state_->mutex);
    return state_->connected && state_->db && !state_->disconnecting &&
           !state_->disconnectRequested.load(std::memory_order_acquire)
        ? sqlite3_last_insert_rowid(state_->db) : 0;
}

int64_t SQLiteBackend::changes() {
    std::lock_guard<std::mutex> lock(state_->mutex);
    return state_->connected && state_->db && !state_->disconnecting &&
           !state_->disconnectRequested.load(std::memory_order_acquire)
        ? sqlite3_changes(state_->db) : 0;
}

std::string SQLiteBackend::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 2);
    for (char c : str) result += (c == '\'') ? "''" : std::string(1, c);
    return result;
}

std::future<StorageResult<ResultSet>> SQLiteBackend::queryAsync(
    const std::string& sql, const QueryParams& params) {
    {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (!state_->connected || !state_->db || state_->disconnecting ||
            state_->disconnectRequested.load(std::memory_order_acquire) ||
            !state_->acceptingAsync) {
            return readyFuture(StorageResult<ResultSet>::fail("Not connected"));
        }
        ++state_->outstandingAsync;
    }
    try {
        auto state = state_;
        return std::async(std::launch::async, [state, sql, params]() {
            AsyncCompletion completion(state);
            return queryLocked(state, sql, params, true);
        });
    } catch (const std::exception& ex) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        --state_->outstandingAsync;
        state_->cv.notify_all();
        return readyFuture(StorageResult<ResultSet>::fail(
            std::string("Failed to launch asynchronous query: ") + ex.what()));
    } catch (...) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        --state_->outstandingAsync;
        state_->cv.notify_all();
        return readyFuture(StorageResult<ResultSet>::fail(
            "Failed to launch asynchronous query"));
    }
}

std::future<StorageResult<void>> SQLiteBackend::executeAsync(
    const std::string& sql, const QueryParams& params) {
    {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (!state_->connected || !state_->db || state_->disconnecting ||
            state_->disconnectRequested.load(std::memory_order_acquire) ||
            !state_->acceptingAsync) {
            return readyFuture(StorageResult<void>::fail("Not connected"));
        }
        ++state_->outstandingAsync;
    }
    try {
        auto state = state_;
        return std::async(std::launch::async, [state, sql, params]() {
            AsyncCompletion completion(state);
            return executeLocked(state, sql, params, true);
        });
    } catch (const std::exception& ex) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        --state_->outstandingAsync;
        state_->cv.notify_all();
        return readyFuture(StorageResult<void>::fail(
            std::string("Failed to launch asynchronous execute: ") + ex.what()));
    } catch (...) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        --state_->outstandingAsync;
        state_->cv.notify_all();
        return readyFuture(StorageResult<void>::fail(
            "Failed to launch asynchronous execute"));
    }
}

std::string SQLiteBackend::getVersion() const {
    return sqlite3_libversion();
}

void SQLiteBackend::vacuum() {
    execute("VACUUM");
}

void SQLiteBackend::checkpoint() {
    std::lock_guard<std::mutex> lock(state_->mutex);
    if (state_->connected && state_->db && !state_->disconnecting &&
        !state_->disconnectRequested.load(std::memory_order_acquire)) {
        sqlite3_wal_checkpoint_v2(state_->db, nullptr, SQLITE_CHECKPOINT_PASSIVE,
                                  nullptr, nullptr);
    }
}

int64_t SQLiteBackend::getFileSize() const {
    if (config_.path == ":memory:") return 0;
    std::ifstream file(config_.path, std::ios::binary | std::ios::ate);
    return file.is_open() ? static_cast<int64_t>(file.tellg()) : 0;
}

// ============================================================================
// ConnectionPool Implementation
// ============================================================================

ConnectionPool::ConnectionPool(const StorageConfig& config)
    : config_(config),
      targetSize_(static_cast<size_t>(config.maxConnections > 0
                                     ? config.maxConnections : 4)) {
    for (size_t i = 0; i < targetSize_; ++i) {
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

    const auto timeout = std::chrono::milliseconds(
        std::max(0, config_.connectionTimeout));
    if (!cv_.wait_for(lock, timeout,
                      [this]() { return !available_.empty() || shutdown_; })) {
        return nullptr;
    }
    if (shutdown_ || available_.empty()) return nullptr;

    auto conn = available_.front();
    available_.pop();
    leased_.insert(conn.get());
    return conn;
}

void ConnectionPool::release(std::shared_ptr<StorageBackend> conn) {
    if (!conn) return;
    bool disconnect = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (leased_.erase(conn.get()) == 0) return;
        const auto it = std::find(all_.begin(), all_.end(), conn);
        if (shutdown_ || it == all_.end() || all_.size() > targetSize_) {
            if (it != all_.end()) all_.erase(it);
            disconnect = true;
        } else {
            available_.push(conn);
            cv_.notify_one();
        }
    }
    if (disconnect) conn->disconnect();
}

size_t ConnectionPool::availableConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return available_.size();
}

size_t ConnectionPool::activeConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return leased_.size();
}

size_t ConnectionPool::totalConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return all_.size();
}

void ConnectionPool::resize(size_t newSize) {
    std::vector<std::shared_ptr<StorageBackend>> toDisconnect;
    std::unique_lock<std::mutex> lock(mutex_);
    if (shutdown_) return;
    targetSize_ = newSize;

    while (all_.size() > targetSize_ && !available_.empty()) {
        auto conn = available_.front();
        available_.pop();
        const auto it = std::find(all_.begin(), all_.end(), conn);
        if (it != all_.end()) all_.erase(it);
        toDisconnect.push_back(std::move(conn));
    }
    while (all_.size() < targetSize_) {
        lock.unlock();
        auto conn = createConnection();
        lock.lock();
        if (shutdown_) {
            if (conn) toDisconnect.push_back(std::move(conn));
            break;
        }
        if (!conn) break;
        if (all_.size() < targetSize_) {
            all_.push_back(conn);
            available_.push(conn);
        } else {
            toDisconnect.push_back(std::move(conn));
        }
    }
    cv_.notify_all();
    lock.unlock();
    for (auto& conn : toDisconnect) conn->disconnect();
}

void ConnectionPool::clear() {
    std::vector<std::shared_ptr<StorageBackend>> idle;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
        targetSize_ = 0;
        while (!available_.empty()) {
            auto conn = available_.front();
            available_.pop();
            const auto it = std::find(all_.begin(), all_.end(), conn);
            if (it != all_.end()) all_.erase(it);
            idle.push_back(std::move(conn));
        }
        cv_.notify_all();
    }
    // Never close a checked-out connection. It stays usable until release(),
    // which removes and disconnects it because the pool is shut down.
    for (auto& conn : idle) conn->disconnect();
}

std::shared_ptr<StorageBackend> ConnectionPool::createConnection() {
    auto backend = std::make_shared<SQLiteBackend>(config_);
    return backend->connect() ? backend : nullptr;
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
    
    if (pool_->availableConnections() == 0) {
        pool_.reset();
        return false;
    }
    
    auto conn = pool_->acquire();
    if (!conn) return false;
    
    memoryPersistence_ = std::make_shared<MemoryPersistence>(conn);
    if (!memoryPersistence_->initialize()) {
        pool_->release(conn);
        memoryPersistence_.reset();
        pool_->clear();
        pool_.reset();
        return false;
    }
    
    kvStore_ = std::make_shared<KeyValueStore>(conn);
    if (!kvStore_->initialize()) {
        pool_->release(conn);
        kvStore_.reset();
        memoryPersistence_.reset();
        pool_->clear();
        pool_.reset();
        return false;
    }
    
    pool_->release(conn);
    initialized_ = true;
    return true;
}

bool StorageManager::shutdown() {
    std::shared_ptr<ConnectionPool> pool;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        memoryPersistence_.reset();
        kvStore_.reset();
        pool = std::move(pool_);
        initialized_ = false;
    }
    if (pool) pool->clear();
    return true;
}

std::shared_ptr<StorageBackend> StorageManager::getBackend() {
    std::shared_ptr<ConnectionPool> pool;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pool = pool_;
    }
    return pool ? pool->acquire() : nullptr;
}

std::shared_ptr<ConnectionPool> StorageManager::getPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    return pool_;
}

std::shared_ptr<MemoryPersistence> StorageManager::getMemoryPersistence() {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryPersistence_;
}

std::shared_ptr<KeyValueStore> StorageManager::getKeyValueStore() {
    std::lock_guard<std::mutex> lock(mutex_);
    return kvStore_;
}

bool StorageManager::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return initialized_;
}

StorageConfig StorageManager::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_;
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
