/**
 * test_persistence_e2e.cpp — Comprehensive E2E tests for the Persistence module
 *
 * Covers:
 *   1. SQLiteBackend — connection, CRUD, transactions, batch ops
 *   2. TransactionScope — RAII commit/rollback
 *   3. ConnectionPool — concurrent access
 *   4. StorageResult — error handling
 *   5. Schema operations — table creation, listing, dropping
 *
 * Cross-fork parity: identical test file in both hurdcog and o9nn.
 */
#include <gtest/gtest.h>
#include "elizaos/persistence.hpp"
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <chrono>

using namespace elizaos;

// ============================================================================
// SQLiteBackend — Connection Tests
// ============================================================================

class PersistenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        StorageConfig config = StorageConfig::inMemory();
        backend_ = std::make_unique<SQLiteBackend>(config);
        ASSERT_TRUE(backend_->connect());
    }
    void TearDown() override {
        if (backend_ && backend_->isConnected()) {
            backend_->disconnect();
        }
    }
    std::unique_ptr<SQLiteBackend> backend_;
};

TEST_F(PersistenceTest, ConnectionEstablished) {
    EXPECT_TRUE(backend_->isConnected());
}

TEST_F(PersistenceTest, BackendType) {
    EXPECT_EQ(backend_->getBackendType(), "sqlite");
}

TEST_F(PersistenceTest, VersionNotEmpty) {
    EXPECT_FALSE(backend_->getVersion().empty());
}

TEST_F(PersistenceTest, Disconnect) {
    EXPECT_TRUE(backend_->disconnect());
    EXPECT_FALSE(backend_->isConnected());
}

TEST_F(PersistenceTest, Reconnect) {
    EXPECT_TRUE(backend_->disconnect());
    EXPECT_TRUE(backend_->connect());
    EXPECT_TRUE(backend_->isConnected());
}

// ============================================================================
// Schema Operations
// ============================================================================

TEST_F(PersistenceTest, CreateTable) {
    auto result = backend_->createTable("agents", {
        "id TEXT PRIMARY KEY",
        "name TEXT NOT NULL",
        "created_at INTEGER"
    });
    EXPECT_TRUE(result.success);
}

TEST_F(PersistenceTest, TableExists) {
    backend_->createTable("test_table", {"id INTEGER PRIMARY KEY", "value TEXT"});
    EXPECT_TRUE(backend_->tableExists("test_table"));
    EXPECT_FALSE(backend_->tableExists("nonexistent_table"));
}

TEST_F(PersistenceTest, ListTables) {
    backend_->createTable("table_a", {"id INTEGER PRIMARY KEY"});
    backend_->createTable("table_b", {"id INTEGER PRIMARY KEY"});
    auto tables = backend_->listTables();
    EXPECT_GE(tables.size(), 2u);
}

TEST_F(PersistenceTest, DropTable) {
    backend_->createTable("drop_me", {"id INTEGER PRIMARY KEY"});
    EXPECT_TRUE(backend_->tableExists("drop_me"));
    auto result = backend_->dropTable("drop_me");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(backend_->tableExists("drop_me"));
}

// ============================================================================
// CRUD Operations
// ============================================================================

TEST_F(PersistenceTest, InsertAndQuery) {
    backend_->createTable("goals", {
        "id TEXT PRIMARY KEY",
        "name TEXT NOT NULL",
        "priority INTEGER DEFAULT 0"
    });
    auto insertResult = backend_->execute(
        "INSERT INTO goals (id, name, priority) VALUES (?, ?, ?)",
        {"goal_1", "Complete KSM cycle", int64_t(3)}
    );
    EXPECT_TRUE(insertResult.success);
    auto queryResult = backend_->query(
        "SELECT name, priority FROM goals WHERE id = ?",
        {"goal_1"}
    );
    EXPECT_TRUE(queryResult.success);
    ASSERT_TRUE(queryResult.value.has_value());
    ASSERT_EQ(queryResult.value->size(), 1u);
    EXPECT_EQ((*queryResult.value)[0][0].asString(), "Complete KSM cycle");
    EXPECT_EQ((*queryResult.value)[0][1].asInt(), 3);
}

TEST_F(PersistenceTest, UpdateRecord) {
    backend_->createTable("state", {"key TEXT PRIMARY KEY", "value TEXT"});
    backend_->execute("INSERT INTO state (key, value) VALUES (?, ?)", {"mode", "discovery"});
    backend_->execute("UPDATE state SET value = ? WHERE key = ?", {"mastery", "mode"});
    auto result = backend_->query("SELECT value FROM state WHERE key = ?", {"mode"});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asString(), "mastery");
}

TEST_F(PersistenceTest, DeleteRecord) {
    backend_->createTable("temp", {"id INTEGER PRIMARY KEY", "data TEXT"});
    backend_->execute("INSERT INTO temp (id, data) VALUES (?, ?)", {int64_t(1), "test"});
    backend_->execute("DELETE FROM temp WHERE id = ?", {int64_t(1)});
    auto result = backend_->query("SELECT * FROM temp WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ(result.value->size(), 0u);
}

TEST_F(PersistenceTest, MultipleInserts) {
    backend_->createTable("items", {"id INTEGER PRIMARY KEY", "name TEXT"});
    for (int i = 0; i < 10; ++i) {
        backend_->execute("INSERT INTO items (id, name) VALUES (?, ?)",
                         {int64_t(i), "item_" + std::to_string(i)});
    }
    auto result = backend_->query("SELECT COUNT(*) FROM items", {});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asInt(), 10);
}

TEST_F(PersistenceTest, NullValues) {
    backend_->createTable("nullable", {"id INTEGER PRIMARY KEY", "optional_field TEXT"});
    backend_->execute("INSERT INTO nullable (id, optional_field) VALUES (?, ?)",
                     {int64_t(1), nullptr});
    auto result = backend_->query("SELECT optional_field FROM nullable WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_TRUE((*result.value)[0][0].isNull());
}

TEST_F(PersistenceTest, SequentialInserts) {
    // Verify multiple sequential inserts produce distinct rows
    backend_->createTable("seq_test", {"id INTEGER PRIMARY KEY", "val TEXT"});
    backend_->execute("INSERT INTO seq_test (id, val) VALUES (?, ?)", {int64_t(1), "first"});
    backend_->execute("INSERT INTO seq_test (id, val) VALUES (?, ?)", {int64_t(2), "second"});
    auto result = backend_->query("SELECT id, val FROM seq_test ORDER BY id", {});
    ASSERT_TRUE(result.success && result.value.has_value());
    ASSERT_EQ(result.value->size(), 2u);
    EXPECT_EQ((*result.value)[0][0].asInt(), 1);
    EXPECT_EQ((*result.value)[1][0].asInt(), 2);
    EXPECT_EQ((*result.value)[0][1].asString(), "first");
    EXPECT_EQ((*result.value)[1][1].asString(), "second");
}

TEST_F(PersistenceTest, ChangesCount) {
    backend_->createTable("bulk", {"id INTEGER PRIMARY KEY", "status TEXT"});
    for (int i = 0; i < 5; ++i) {
        backend_->execute("INSERT INTO bulk (id, status) VALUES (?, ?)",
                         {int64_t(i), "pending"});
    }
    backend_->execute("UPDATE bulk SET status = ?", {"done"});
    EXPECT_EQ(backend_->changes(), 5);
}

// ============================================================================
// Transaction Tests
// ============================================================================

TEST_F(PersistenceTest, TransactionCommit) {
    backend_->createTable("txn_test", {"id INTEGER PRIMARY KEY", "val TEXT"});
    auto txn = backend_->beginTransaction();
    ASSERT_NE(txn, nullptr);
    txn->execute("INSERT INTO txn_test (id, val) VALUES (?, ?)", {int64_t(1), "committed"});
    EXPECT_TRUE(txn->commit());
    auto result = backend_->query("SELECT val FROM txn_test WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asString(), "committed");
}

TEST_F(PersistenceTest, TransactionRollback) {
    backend_->createTable("txn_rollback", {"id INTEGER PRIMARY KEY", "val TEXT"});
    backend_->execute("INSERT INTO txn_rollback (id, val) VALUES (?, ?)", {int64_t(1), "original"});
    auto txn = backend_->beginTransaction();
    txn->execute("UPDATE txn_rollback SET val = ? WHERE id = ?", {"modified", int64_t(1)});
    EXPECT_TRUE(txn->rollback());
    auto result = backend_->query("SELECT val FROM txn_rollback WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asString(), "original");
}

TEST_F(PersistenceTest, TransactionScopeCommit) {
    backend_->createTable("scope_test", {"id INTEGER PRIMARY KEY", "val TEXT"});
    {
        auto txn = backend_->beginTransaction();
        TransactionScope scope(txn);
        scope.execute("INSERT INTO scope_test (id, val) VALUES (?, ?)", {int64_t(1), "scoped"});
        scope.commit();
    }
    auto result = backend_->query("SELECT val FROM scope_test WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asString(), "scoped");
}

TEST_F(PersistenceTest, TransactionScopeAutoRollback) {
    backend_->createTable("scope_rollback", {"id INTEGER PRIMARY KEY", "val TEXT"});
    backend_->execute("INSERT INTO scope_rollback (id, val) VALUES (?, ?)", {int64_t(1), "original"});
    {
        auto txn = backend_->beginTransaction();
        TransactionScope scope(txn);
        scope.execute("UPDATE scope_rollback SET val = ? WHERE id = ?", {"modified", int64_t(1)});
        // Scope destructs without commit → auto-rollback
    }
    auto result = backend_->query("SELECT val FROM scope_rollback WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asString(), "original");
}

// ============================================================================
// Batch Operations
// ============================================================================

TEST_F(PersistenceTest, BatchInsert) {
    backend_->createTable("batch", {"id INTEGER PRIMARY KEY", "name TEXT"});
    std::vector<QueryParams> batches;
    for (int i = 0; i < 100; ++i) {
        batches.push_back({int64_t(i), "batch_" + std::to_string(i)});
    }
    auto result = backend_->executeBatch(
        "INSERT INTO batch (id, name) VALUES (?, ?)", batches);
    EXPECT_TRUE(result.success);
    auto count = backend_->query("SELECT COUNT(*) FROM batch", {});
    ASSERT_TRUE(count.success && count.value.has_value());
    EXPECT_EQ((*count.value)[0][0].asInt(), 100);
}

// ============================================================================
// Error Handling
// ============================================================================

TEST_F(PersistenceTest, InvalidSQL) {
    auto result = backend_->execute("INVALID SQL STATEMENT", {});
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error.has_value());
}

TEST_F(PersistenceTest, QueryNonexistentTable) {
    auto result = backend_->query("SELECT * FROM nonexistent", {});
    EXPECT_FALSE(result.success);
}

TEST_F(PersistenceTest, DuplicatePrimaryKey) {
    backend_->createTable("unique_test", {"id INTEGER PRIMARY KEY", "val TEXT"});
    backend_->execute("INSERT INTO unique_test (id, val) VALUES (?, ?)", {int64_t(1), "first"});
    auto result = backend_->execute("INSERT INTO unique_test (id, val) VALUES (?, ?)", {int64_t(1), "duplicate"});
    EXPECT_FALSE(result.success);
}

TEST_F(PersistenceTest, EscapeString) {
    std::string dangerous = "'; DROP TABLE agents; --";
    std::string escaped = backend_->escapeString(dangerous);
    EXPECT_NE(escaped, dangerous);
}

// ============================================================================
// StorageResult Tests
// ============================================================================

TEST(StorageResultTest, SuccessResult) {
    auto result = StorageResult<int>::ok(42);
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.value.has_value());
    EXPECT_EQ(*result.value, 42);
    EXPECT_FALSE(result.error.has_value());
}

TEST(StorageResultTest, FailureResult) {
    auto result = StorageResult<int>::fail("database locked");
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.value.has_value());
    EXPECT_TRUE(result.error.has_value());
    EXPECT_EQ(*result.error, "database locked");
}

TEST(StorageResultTest, VoidSuccess) {
    auto result = StorageResult<void>::ok();
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST(StorageResultTest, VoidFailure) {
    auto result = StorageResult<void>::fail("constraint violation");
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(static_cast<bool>(result));
}

// ============================================================================
// ColumnValue Tests
// ============================================================================

TEST(ColumnValueTest, IntValue) {
    ColumnValue cv;
    cv.name = "count";
    cv.value = int64_t(42);
    EXPECT_TRUE(cv.isInt());
    EXPECT_FALSE(cv.isString());
    EXPECT_EQ(cv.asInt(), 42);
}

TEST(ColumnValueTest, StringValue) {
    ColumnValue cv;
    cv.name = "name";
    cv.value = std::string("Dan");
    EXPECT_TRUE(cv.isString());
    EXPECT_EQ(cv.asString(), "Dan");
}

TEST(ColumnValueTest, DoubleValue) {
    ColumnValue cv;
    cv.name = "score";
    cv.value = 0.95;
    EXPECT_TRUE(cv.isDouble());
    EXPECT_NEAR(cv.asDouble(), 0.95, 0.001);
}

TEST(ColumnValueTest, NullValue) {
    ColumnValue cv;
    cv.name = "optional";
    cv.value = nullptr;
    EXPECT_TRUE(cv.isNull());
    EXPECT_EQ(cv.asInt(0), 0);
    EXPECT_EQ(cv.asString("default"), "default");
}

// ============================================================================
// Async Operations
// ============================================================================

TEST_F(PersistenceTest, AsyncQuery) {
    backend_->createTable("async_test", {"id INTEGER PRIMARY KEY", "val TEXT"});
    backend_->execute("INSERT INTO async_test (id, val) VALUES (?, ?)", {int64_t(1), "async"});
    auto future = backend_->queryAsync("SELECT val FROM async_test WHERE id = ?", {int64_t(1)});
    auto result = future.get();
    ASSERT_TRUE(result.success && result.value.has_value());
    EXPECT_EQ((*result.value)[0][0].asString(), "async");
}

TEST_F(PersistenceTest, AsyncExecute) {
    backend_->createTable("async_exec", {"id INTEGER PRIMARY KEY", "val TEXT"});
    auto future = backend_->executeAsync(
        "INSERT INTO async_exec (id, val) VALUES (?, ?)", {int64_t(1), "async_insert"});
    auto result = future.get();
    EXPECT_TRUE(result.success);
}

// ============================================================================
// Concurrent Access (Thread Safety)
// ============================================================================

TEST_F(PersistenceTest, ConcurrentReads) {
    backend_->createTable("concurrent", {"id INTEGER PRIMARY KEY", "val TEXT"});
    for (int i = 0; i < 10; ++i) {
        backend_->execute("INSERT INTO concurrent (id, val) VALUES (?, ?)",
                         {int64_t(i), "value_" + std::to_string(i)});
    }
    std::vector<std::future<bool>> futures;
    for (int i = 0; i < 5; ++i) {
        futures.push_back(std::async(std::launch::async, [this, i]() {
            auto result = backend_->query("SELECT val FROM concurrent WHERE id = ?", {int64_t(i)});
            return result.success && result.value.has_value() && result.value->size() == 1;
        }));
    }
    for (auto& f : futures) {
        EXPECT_TRUE(f.get());
    }
}
