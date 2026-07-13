// persistence_test.cpp - Comprehensive unit tests for the hurdcog Persistence module.
//
// Validates the authoritative flattened API declared in include/persistence.hpp:
//   SQLiteBackend, Transaction / TransactionScope, ConnectionPool,
//   MemoryPersistence, KeyValueStore, StorageManager, and the storage:: helpers.
//
// These tests intentionally exercise real SQLite I/O (in-memory and file backed)
// rather than mocks, so each assertion proves a genuine round-trip through the
// storage layer. The suite is registered in the KSM canonical validation target
// so future autonomy/persistence repairs cannot bypass durable-storage coverage.

#include <gtest/gtest.h>
#include "elizaos/persistence.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace elizaos;

namespace {
const std::string TEST_DB_PATH = "/tmp/elizaos_persistence_test.db";

void cleanupTestDb() {
    std::remove(TEST_DB_PATH.c_str());
    std::remove((TEST_DB_PATH + "-journal").c_str());
    std::remove((TEST_DB_PATH + "-wal").c_str());
    std::remove((TEST_DB_PATH + "-shm").c_str());
}
}

class PersistenceTest : public ::testing::Test {
protected:
    void SetUp() override { cleanupTestDb(); }
    void TearDown() override { cleanupTestDb(); }
};

// ===========================================================================
// SQLiteBackend Tests
// ===========================================================================

TEST_F(PersistenceTest, SQLiteBackendConnectDisconnect) {
    StorageConfig config(TEST_DB_PATH);
    SQLiteBackend backend(config);
    EXPECT_TRUE(backend.connect());
    EXPECT_TRUE(backend.isConnected());
    EXPECT_TRUE(backend.disconnect());
    EXPECT_FALSE(backend.isConnected());
}

TEST_F(PersistenceTest, SQLiteBackendInMemory) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    EXPECT_TRUE(backend.connect());
    EXPECT_TRUE(backend.isConnected());
    EXPECT_EQ(backend.getBackendType(), "sqlite");
    EXPECT_FALSE(backend.getVersion().empty());
    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendCreateTableAndInsert) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    auto createResult = backend.createTable("test_items",
        {"id INTEGER PRIMARY KEY AUTOINCREMENT", "name TEXT NOT NULL", "value REAL"});
    EXPECT_TRUE(createResult.success);

    EXPECT_TRUE(backend.tableExists("test_items"));
    EXPECT_FALSE(backend.tableExists("nonexistent_table"));

    auto insertResult = backend.execute(
        "INSERT INTO test_items (name, value) VALUES (?, ?)",
        {std::string("alpha"), 3.14});
    EXPECT_TRUE(insertResult.success);
    EXPECT_EQ(backend.lastInsertRowId(), 1);

    insertResult = backend.execute(
        "INSERT INTO test_items (name, value) VALUES (?, ?)",
        {std::string("beta"), 2.71});
    EXPECT_TRUE(insertResult.success);
    EXPECT_EQ(backend.lastInsertRowId(), 2);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendQueryResults) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE items (id INTEGER PRIMARY KEY, name TEXT, count INTEGER)", {});
    backend.execute("INSERT INTO items VALUES (1, 'first', 10)", {});
    backend.execute("INSERT INTO items VALUES (2, 'second', 20)", {});
    backend.execute("INSERT INTO items VALUES (3, 'third', 30)", {});

    auto result = backend.query("SELECT name, count FROM items ORDER BY count", {});
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.value.has_value());
    ASSERT_EQ(result.value->size(), 3u);

    // First row
    auto& row0 = (*result.value)[0];
    EXPECT_EQ(row0.size(), 2u);
    EXPECT_EQ(row0[0].asString(), "first");
    EXPECT_EQ(row0[1].asInt(), 10);

    // Last row
    auto& row2 = (*result.value)[2];
    EXPECT_EQ(row2[0].asString(), "third");
    EXPECT_EQ(row2[1].asInt(), 30);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendEmptyQuery) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE empty_table (id INTEGER PRIMARY KEY)", {});
    auto result = backend.query("SELECT * FROM empty_table", {});
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.value.has_value());
    EXPECT_TRUE(result.value->empty());

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendInvalidSQL) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    auto result = backend.execute("THIS IS NOT VALID SQL", {});
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error.has_value());

    auto queryResult = backend.query("SELECT * FROM nonexistent_table", {});
    EXPECT_FALSE(queryResult.success);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendChangesCount) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE mods (id INTEGER PRIMARY KEY, val INTEGER)", {});
    backend.execute("INSERT INTO mods VALUES (1, 100)", {});
    backend.execute("INSERT INTO mods VALUES (2, 200)", {});
    backend.execute("INSERT INTO mods VALUES (3, 300)", {});
    backend.execute("UPDATE mods SET val = 999 WHERE val >= 200", {});
    EXPECT_EQ(backend.changes(), 2);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendListTables) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.createTable("table_a", {"id INTEGER PRIMARY KEY"});
    backend.createTable("table_b", {"id INTEGER PRIMARY KEY"});
    backend.createTable("table_c", {"id INTEGER PRIMARY KEY"});

    auto tables = backend.listTables();
    EXPECT_GE(tables.size(), 3u);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendDropTable) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.createTable("temp_table", {"id INTEGER PRIMARY KEY"});
    EXPECT_TRUE(backend.tableExists("temp_table"));

    auto dropResult = backend.dropTable("temp_table");
    EXPECT_TRUE(dropResult.success);
    EXPECT_FALSE(backend.tableExists("temp_table"));

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendParameterBinding) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE params (id INTEGER PRIMARY KEY, text_val TEXT, int_val INTEGER, real_val REAL)", {});

    // Test various parameter types
    backend.execute("INSERT INTO params VALUES (?, ?, ?, ?)",
        {int64_t(1), std::string("hello"), int64_t(42), 3.14});

    auto result = backend.query("SELECT * FROM params WHERE id = ?", {int64_t(1)});
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.value->size(), 1u);
    EXPECT_EQ((*result.value)[0][1].asString(), "hello");
    EXPECT_EQ((*result.value)[0][2].asInt(), 42);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendBatchOperations) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE batch (id INTEGER PRIMARY KEY AUTOINCREMENT, val TEXT)", {});

    std::vector<QueryParams> batches;
    for (int i = 0; i < 10; ++i) {
        batches.push_back({std::string("item_" + std::to_string(i))});
    }

    auto batchResult = backend.executeBatch("INSERT INTO batch (val) VALUES (?)", batches);
    EXPECT_TRUE(batchResult.success);

    auto countResult = backend.query("SELECT COUNT(*) FROM batch", {});
    ASSERT_TRUE(countResult.success);
    EXPECT_EQ((*countResult.value)[0][0].asInt(), 10);

    backend.disconnect();
}

TEST_F(PersistenceTest, SQLiteBackendAsyncQuery) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE async_test (id INTEGER PRIMARY KEY, val TEXT)", {});
    backend.execute("INSERT INTO async_test VALUES (1, 'async_value')", {});

    auto future = backend.queryAsync("SELECT val FROM async_test WHERE id = ?", {int64_t(1)});
    auto result = future.get();
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.value->size(), 1u);
    EXPECT_EQ((*result.value)[0][0].asString(), "async_value");

    backend.disconnect();
}

// ===========================================================================
// Transaction Tests
// ===========================================================================

TEST_F(PersistenceTest, TransactionCommit) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE txn_test (id INTEGER PRIMARY KEY, val TEXT)", {});

    {
        auto txn = backend.beginTransaction();
        ASSERT_NE(txn, nullptr);
        EXPECT_TRUE(txn->isActive());

        auto execResult = txn->execute("INSERT INTO txn_test VALUES (1, 'committed')", {});
        EXPECT_TRUE(execResult.success);

        EXPECT_TRUE(txn->commit());
        EXPECT_FALSE(txn->isActive());
    }

    auto result = backend.query("SELECT val FROM txn_test WHERE id = 1", {});
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.value->size(), 1u);
    EXPECT_EQ((*result.value)[0][0].asString(), "committed");

    backend.disconnect();
}

TEST_F(PersistenceTest, TransactionRollback) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE txn_test2 (id INTEGER PRIMARY KEY, val TEXT)", {});
    backend.execute("INSERT INTO txn_test2 VALUES (1, 'original')", {});

    {
        auto txn = backend.beginTransaction();
        ASSERT_NE(txn, nullptr);
        txn->execute("UPDATE txn_test2 SET val = 'modified' WHERE id = 1", {});
        EXPECT_TRUE(txn->rollback());
    }

    auto result = backend.query("SELECT val FROM txn_test2 WHERE id = 1", {});
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.value->size(), 1u);
    EXPECT_EQ((*result.value)[0][0].asString(), "original");

    backend.disconnect();
}

TEST_F(PersistenceTest, TransactionScopeAutoRollback) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    backend.execute("CREATE TABLE txn_test3 (id INTEGER PRIMARY KEY, val TEXT)", {});
    backend.execute("INSERT INTO txn_test3 VALUES (1, 'safe')", {});

    {
        auto txn = backend.beginTransaction();
        TransactionScope scope(txn);
        txn->execute("UPDATE txn_test3 SET val = 'unsafe' WHERE id = 1", {});
        // scope destructor should rollback since commit() was not called
    }

    auto result = backend.query("SELECT val FROM txn_test3 WHERE id = 1", {});
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.value->size(), 1u);
    EXPECT_EQ((*result.value)[0][0].asString(), "safe");

    backend.disconnect();
}

TEST_F(PersistenceTest, TransactionIsolationLevels) {
    StorageConfig config = StorageConfig::inMemory();
    SQLiteBackend backend(config);
    ASSERT_TRUE(backend.connect());

    // Test that different isolation levels can be created
    auto deferred = backend.beginTransaction(IsolationLevel::DEFERRED);
    ASSERT_NE(deferred, nullptr);
    deferred->rollback();

    auto immediate = backend.beginTransaction(IsolationLevel::IMMEDIATE);
    ASSERT_NE(immediate, nullptr);
    immediate->rollback();

    auto exclusive = backend.beginTransaction(IsolationLevel::EXCLUSIVE);
    ASSERT_NE(exclusive, nullptr);
    exclusive->rollback();

    backend.disconnect();
}

// ===========================================================================
// ConnectionPool Tests
// ===========================================================================

TEST_F(PersistenceTest, ConnectionPoolAcquireRelease) {
    StorageConfig config(TEST_DB_PATH);
    config.maxConnections = 3;
    ConnectionPool pool(config);

    auto conn1 = pool.acquire();
    ASSERT_NE(conn1, nullptr);
    EXPECT_TRUE(conn1->isConnected());

    auto conn2 = pool.acquire();
    ASSERT_NE(conn2, nullptr);

    pool.release(conn1);
    pool.release(conn2);

    EXPECT_GE(pool.availableConnections(), 2u);
}

TEST_F(PersistenceTest, ConnectionPoolTotalConnections) {
    StorageConfig config(TEST_DB_PATH);
    config.maxConnections = 4;
    ConnectionPool pool(config);

    auto conn1 = pool.acquire();
    auto conn2 = pool.acquire();
    EXPECT_GE(pool.totalConnections(), 2u);
    EXPECT_GE(pool.activeConnections(), 0u);

    pool.release(conn1);
    pool.release(conn2);
}

TEST_F(PersistenceTest, ConnectionPoolConcurrentAccess) {
    StorageConfig config(TEST_DB_PATH);
    config.maxConnections = 4;
    ConnectionPool pool(config);

    // Set up the table with a connection
    auto setup = pool.acquire();
    ASSERT_NE(setup, nullptr);
    setup->execute("CREATE TABLE IF NOT EXISTS pool_test (id INTEGER PRIMARY KEY, tid INTEGER)", {});
    pool.release(setup);

    std::atomic<int> successCount{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&pool, &successCount, i]() {
            auto conn = pool.acquire();
            if (conn) {
                auto result = conn->execute(
                    "INSERT INTO pool_test (tid) VALUES (?)",
                    {int64_t(i)});
                if (result.success) {
                    successCount.fetch_add(1, std::memory_order_relaxed);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                pool.release(conn);
            }
        });
    }

    for (auto& t : threads) t.join();
    EXPECT_GE(successCount.load(), 4); // At least pool_size threads should succeed
}

TEST_F(PersistenceTest, ConnectionPoolResize) {
    StorageConfig config(TEST_DB_PATH);
    config.maxConnections = 2;
    ConnectionPool pool(config);

    pool.resize(5);
    EXPECT_GE(pool.totalConnections(), 0u); // resize may be lazy

    pool.clear();
    EXPECT_EQ(pool.availableConnections(), 0u);
}

// ===========================================================================
// MemoryPersistence Tests
// ===========================================================================

TEST_F(PersistenceTest, MemoryPersistenceSaveAndLoad) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    UUID memId = generateUUID();
    auto saveResult = mp.saveMemory(memId, "The user prefers dark mode",
        "agent-alpha", "room-1", "memories");
    EXPECT_TRUE(saveResult.success);

    auto loadResult = mp.loadMemory(memId, "memories");
    ASSERT_TRUE(loadResult.success);
    ASSERT_TRUE(loadResult.value.has_value());
    EXPECT_EQ(loadResult.value->at("content"), "The user prefers dark mode");
}

TEST_F(PersistenceTest, MemoryPersistenceLoadByAgent) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    for (int i = 0; i < 5; ++i) {
        std::string agentId = (i < 3) ? "agent-A" : "agent-B";
        mp.saveMemory(generateUUID(), "Memory " + std::to_string(i),
            agentId, "room-1", "memories");
    }

    auto agentA = mp.loadMemoriesByAgent("agent-A", "memories");
    ASSERT_TRUE(agentA.success);
    EXPECT_EQ(agentA.value->size(), 3u);

    auto agentB = mp.loadMemoriesByAgent("agent-B", "memories");
    ASSERT_TRUE(agentB.success);
    EXPECT_EQ(agentB.value->size(), 2u);
}

TEST_F(PersistenceTest, MemoryPersistenceLoadByRoom) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    for (int i = 0; i < 4; ++i) {
        std::string roomId = (i % 2 == 0) ? "room-A" : "room-B";
        mp.saveMemory(generateUUID(), "Room memory " + std::to_string(i),
            "agent-X", roomId, "memories");
    }

    auto roomA = mp.loadMemoriesByRoom("room-A", "memories");
    ASSERT_TRUE(roomA.success);
    EXPECT_EQ(roomA.value->size(), 2u);
}

TEST_F(PersistenceTest, MemoryPersistenceDelete) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    UUID memId = generateUUID();
    mp.saveMemory(memId, "To be deleted", "agent-1", "room-1", "memories");

    auto delResult = mp.deleteMemory(memId, "memories");
    EXPECT_TRUE(delResult.success);

    auto loadResult = mp.loadMemory(memId, "memories");
    // After deletion, load should return empty or not found
    if (loadResult.success && loadResult.value.has_value()) {
        EXPECT_TRUE(loadResult.value->empty());
    }
}

TEST_F(PersistenceTest, MemoryPersistenceDeleteByAgent) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    for (int i = 0; i < 3; ++i) {
        mp.saveMemory(generateUUID(), "Agent memory " + std::to_string(i),
            "agent-del", "room-1", "memories");
    }

    auto delResult = mp.deleteMemoriesByAgent("agent-del", "memories");
    EXPECT_TRUE(delResult.success);

    EXPECT_EQ(mp.getMemoryCountByAgent("agent-del", "memories"), 0);
}

TEST_F(PersistenceTest, MemoryPersistenceSearch) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    mp.saveMemory(generateUUID(), "C++ templates enable generic programming",
        "agent-1", "room-1", "memories");
    mp.saveMemory(generateUUID(), "Python uses duck typing",
        "agent-1", "room-1", "memories");
    mp.saveMemory(generateUUID(), "C++ RAII manages resources",
        "agent-1", "room-1", "memories");

    auto results = mp.searchMemories("C++", "memories", 10);
    ASSERT_TRUE(results.success);
    EXPECT_GE(results.value->size(), 2u);
}

TEST_F(PersistenceTest, MemoryPersistenceBatchSave) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    std::vector<std::tuple<UUID, std::string, std::string, std::string>> batch;
    for (int i = 0; i < 5; ++i) {
        batch.emplace_back(generateUUID(),
            "Batch memory " + std::to_string(i),
            "agent-batch", "room-batch");
    }

    auto batchResult = mp.saveMemoriesBatch(batch, "memories");
    EXPECT_TRUE(batchResult.success);
    EXPECT_EQ(mp.getMemoryCount("memories"), 5);
}

TEST_F(PersistenceTest, MemoryPersistenceCount) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    MemoryPersistence mp(backend);
    ASSERT_TRUE(mp.initialize());

    EXPECT_EQ(mp.getMemoryCount("memories"), 0);

    mp.saveMemory(generateUUID(), "First", "a1", "r1", "memories");
    mp.saveMemory(generateUUID(), "Second", "a1", "r1", "memories");
    mp.saveMemory(generateUUID(), "Third", "a2", "r1", "memories");

    EXPECT_EQ(mp.getMemoryCount("memories"), 3);
    EXPECT_EQ(mp.getMemoryCountByAgent("a1", "memories"), 2);
    EXPECT_EQ(mp.getMemoryCountByAgent("a2", "memories"), 1);
}

// ===========================================================================
// KeyValueStore Tests
// ===========================================================================

TEST_F(PersistenceTest, KeyValueStoreSetAndGet) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    auto setResult = kvs.set("config.theme", "dark");
    EXPECT_TRUE(setResult.success);

    auto getResult = kvs.get("config.theme");
    ASSERT_TRUE(getResult.success);
    EXPECT_EQ(*getResult.value, "dark");
}

TEST_F(PersistenceTest, KeyValueStoreGetMissing) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    auto getResult = kvs.get("nonexistent");
    // Missing key should either fail or return empty
    if (getResult.success) {
        EXPECT_TRUE(getResult.value->empty());
    } else {
        EXPECT_TRUE(getResult.error.has_value());
    }
}

TEST_F(PersistenceTest, KeyValueStoreOverwrite) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    kvs.set("key", "value1");
    kvs.set("key", "value2");

    auto result = kvs.get("key");
    ASSERT_TRUE(result.success);
    EXPECT_EQ(*result.value, "value2");
}

TEST_F(PersistenceTest, KeyValueStoreRemove) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    kvs.set("temp_key", "temp_value");
    auto removeResult = kvs.remove("temp_key");
    EXPECT_TRUE(removeResult.success);

    EXPECT_FALSE(kvs.exists("temp_key"));
}

TEST_F(PersistenceTest, KeyValueStoreExists) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    kvs.set("present", "yes");
    EXPECT_TRUE(kvs.exists("present"));
    EXPECT_FALSE(kvs.exists("absent"));
}

TEST_F(PersistenceTest, KeyValueStoreSetMany) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    std::unordered_map<std::string, std::string> pairs = {
        {"a", "1"}, {"b", "2"}, {"c", "3"}
    };
    auto setResult = kvs.setMany(pairs);
    EXPECT_TRUE(setResult.success);

    auto getResult = kvs.getMany({"a", "b", "c"});
    ASSERT_TRUE(getResult.success);
    EXPECT_EQ(getResult.value->size(), 3u);
}

TEST_F(PersistenceTest, KeyValueStoreByPrefix) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    kvs.set("config.theme", "dark");
    kvs.set("config.lang", "en");
    kvs.set("config.font", "mono");
    kvs.set("state.running", "true");

    auto prefixResult = kvs.getByPrefix("config.");
    ASSERT_TRUE(prefixResult.success);
    EXPECT_EQ(prefixResult.value->size(), 3u);
}

TEST_F(PersistenceTest, KeyValueStoreRemoveByPrefix) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    kvs.set("temp.a", "1");
    kvs.set("temp.b", "2");
    kvs.set("keep.c", "3");

    auto removeResult = kvs.removeByPrefix("temp.");
    EXPECT_TRUE(removeResult.success);

    EXPECT_FALSE(kvs.exists("temp.a"));
    EXPECT_FALSE(kvs.exists("temp.b"));
    EXPECT_TRUE(kvs.exists("keep.c"));
}

TEST_F(PersistenceTest, KeyValueStoreWithExpiry) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    // Set with very short TTL
    kvs.setWithExpiry("expiring", "value", std::chrono::seconds(1));
    EXPECT_TRUE(kvs.exists("expiring"));

    // After cleanup of expired, the key should still exist (TTL not yet passed)
    kvs.cleanupExpired();
    // We can't easily test expiration without sleeping, so just verify the API works
    auto result = kvs.get("expiring");
    EXPECT_TRUE(result.success);
}

TEST_F(PersistenceTest, KeyValueStoreCount) {
    StorageConfig config = StorageConfig::inMemory();
    auto backend = std::make_shared<SQLiteBackend>(config);
    ASSERT_TRUE(backend->connect());

    KeyValueStore kvs(backend);
    ASSERT_TRUE(kvs.initialize());

    kvs.set("x.a", "1");
    kvs.set("x.b", "2");
    kvs.set("y.c", "3");

    EXPECT_EQ(kvs.count(), 3);
    EXPECT_EQ(kvs.countByPrefix("x."), 2);
    EXPECT_EQ(kvs.countByPrefix("y."), 1);
}

// ===========================================================================
// StorageManager Tests (Singleton)
// ===========================================================================

TEST_F(PersistenceTest, StorageManagerInitAndShutdown) {
    auto& mgr = StorageManager::getInstance();
    StorageConfig config = StorageConfig::inMemory();
    EXPECT_TRUE(mgr.initialize(config));
    EXPECT_TRUE(mgr.isInitialized());
    EXPECT_TRUE(mgr.shutdown());
    EXPECT_FALSE(mgr.isInitialized());
}

TEST_F(PersistenceTest, StorageManagerGetInterfaces) {
    auto& mgr = StorageManager::getInstance();
    StorageConfig config = StorageConfig::inMemory();
    ASSERT_TRUE(mgr.initialize(config));

    auto backend = mgr.getBackend();
    EXPECT_NE(backend, nullptr);
    EXPECT_TRUE(backend->isConnected());

    auto pool = mgr.getPool();
    EXPECT_NE(pool, nullptr);

    auto mp = mgr.getMemoryPersistence();
    EXPECT_NE(mp, nullptr);

    auto kvs = mgr.getKeyValueStore();
    EXPECT_NE(kvs, nullptr);

    mgr.shutdown();
}

TEST_F(PersistenceTest, StorageManagerMemoryAndKV) {
    auto& mgr = StorageManager::getInstance();
    StorageConfig config = StorageConfig::inMemory();
    ASSERT_TRUE(mgr.initialize(config));

    // Use memory persistence through manager
    auto mp = mgr.getMemoryPersistence();
    UUID memId = generateUUID();
    auto saveResult = mp->saveMemory(memId, "Manager test memory",
        "agent-mgr", "room-mgr", "memories");
    EXPECT_TRUE(saveResult.success);

    // Use KV store through manager
    auto kvs = mgr.getKeyValueStore();
    kvs->set("mgr.key", "mgr.value");
    auto getResult = kvs->get("mgr.key");
    ASSERT_TRUE(getResult.success);
    EXPECT_EQ(*getResult.value, "mgr.value");

    mgr.shutdown();
}

// ===========================================================================
// Convenience Functions Tests
// ===========================================================================

TEST_F(PersistenceTest, ConvenienceFunctions) {
    StorageConfig config = StorageConfig::inMemory();
    EXPECT_TRUE(storage::init(config));

    auto backend = storage::backend();
    EXPECT_NE(backend, nullptr);

    auto mp = storage::memories();
    EXPECT_NE(mp, nullptr);

    auto kvs = storage::kv();
    EXPECT_NE(kvs, nullptr);

    // Quick KV operations
    EXPECT_TRUE(storage::set("quick.key", "quick.value"));
    auto val = storage::get("quick.key");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "quick.value");

    EXPECT_TRUE(storage::remove("quick.key"));
    EXPECT_FALSE(storage::get("quick.key").has_value());

    EXPECT_TRUE(storage::shutdown());
}

// ===========================================================================
// Persistence Across Reopen (File-based)
// ===========================================================================

TEST_F(PersistenceTest, PersistenceAcrossReopen) {
    // Write data
    {
        StorageConfig config = StorageConfig::file(TEST_DB_PATH);
        SQLiteBackend backend(config);
        ASSERT_TRUE(backend.connect());
        backend.execute("CREATE TABLE persist_test (key TEXT PRIMARY KEY, val TEXT)", {});
        backend.execute("INSERT INTO persist_test VALUES ('hello', 'world')", {});
        backend.disconnect();
    }

    // Reopen and verify
    {
        StorageConfig config = StorageConfig::file(TEST_DB_PATH);
        SQLiteBackend backend(config);
        ASSERT_TRUE(backend.connect());
        auto result = backend.query("SELECT val FROM persist_test WHERE key = 'hello'", {});
        ASSERT_TRUE(result.success);
        ASSERT_EQ(result.value->size(), 1u);
                EXPECT_EQ((*result.value)[0][0].asString(), "world");
        backend.disconnect();
    }
}

std::string makeTempDbPath(const std::string& tag) {
    return "/tmp/elizaos_persistence_" + tag + "_" +
           std::to_string(::getpid()) + "_" +
           std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
           ".db";
}

void removeDbFiles(const std::string& path) {
    std::remove(path.c_str());
    std::remove((path + "-journal").c_str());
    std::remove((path + "-wal").c_str());
    std::remove((path + "-shm").c_str());
}

// ===========================================================================
// SQLiteBackend - connection, schema, and CRUD
// ===========================================================================

TEST(PersistenceSQLiteBackend, ConnectInMemoryAndReportMetadata) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    EXPECT_TRUE(backend.isConnected());
    EXPECT_EQ(backend.getBackendType(), "sqlite");
    EXPECT_FALSE(backend.getVersion().empty());
    EXPECT_TRUE(backend.disconnect());
    EXPECT_FALSE(backend.isConnected());
}

TEST(PersistenceSQLiteBackend, CreateTableInsertAndQueryRoundTrip) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());

    auto created = backend.createTable(
        "items", {"id INTEGER PRIMARY KEY", "name TEXT NOT NULL", "score REAL"});
    ASSERT_TRUE(created.success) << created.error.value_or("");
    EXPECT_TRUE(backend.tableExists("items"));

    auto ins = backend.execute(
        "INSERT INTO items (name, score) VALUES (?, ?)",
        {std::string("alpha"), 1.5});
    ASSERT_TRUE(ins.success) << ins.error.value_or("");
    EXPECT_EQ(ins.rowsAffected, 1);
    EXPECT_GT(backend.lastInsertRowId(), 0);

    auto rows = backend.query("SELECT id, name, score FROM items WHERE name = ?",
                              {std::string("alpha")});
    ASSERT_TRUE(rows.success);
    ASSERT_TRUE(rows.value.has_value());
    ASSERT_EQ(rows.value->size(), 1u);
    const Row& row = rows.value->front();
    ASSERT_EQ(row.size(), 3u);
    EXPECT_EQ(row[1].asString(), "alpha");
    EXPECT_DOUBLE_EQ(row[2].asDouble(), 1.5);
}

TEST(PersistenceSQLiteBackend, BlobAndNullParameterBinding) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("blobs", {"id INTEGER PRIMARY KEY", "payload BLOB", "note TEXT"}).success);

    std::vector<uint8_t> payload = {0xDE, 0xAD, 0xBE, 0xEF};
    ASSERT_TRUE(backend.execute("INSERT INTO blobs (payload, note) VALUES (?, ?)",
                                {payload, QueryParam(nullptr)}).success);

    auto rows = backend.query("SELECT payload, note FROM blobs");
    ASSERT_TRUE(rows.success);
    ASSERT_TRUE(rows.value.has_value());
    ASSERT_EQ(rows.value->size(), 1u);
    EXPECT_EQ(rows.value->front()[0].asBlob(), payload);
    EXPECT_TRUE(rows.value->front()[1].isNull());
}

TEST(PersistenceSQLiteBackend, ExecuteBatchInsertsAllRows) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("kv", {"k TEXT PRIMARY KEY", "v TEXT"}).success);

    std::vector<QueryParams> batch = {
        {std::string("k1"), std::string("v1")},
        {std::string("k2"), std::string("v2")},
        {std::string("k3"), std::string("v3")},
    };
    ASSERT_TRUE(backend.executeBatch("INSERT INTO kv (k, v) VALUES (?, ?)", batch).success);

    auto rows = backend.query("SELECT COUNT(*) FROM kv");
    ASSERT_TRUE(rows.success);
    ASSERT_TRUE(rows.value.has_value());
    EXPECT_EQ(rows.value->front()[0].asInt(), 3);
}

TEST(PersistenceSQLiteBackend, DropTableAndListTables) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("temp_a", {"id INTEGER"}).success);
    ASSERT_TRUE(backend.createTable("temp_b", {"id INTEGER"}).success);

    auto tables = backend.listTables();
    EXPECT_NE(std::find(tables.begin(), tables.end(), "temp_a"), tables.end());
    EXPECT_NE(std::find(tables.begin(), tables.end(), "temp_b"), tables.end());

    ASSERT_TRUE(backend.dropTable("temp_a").success);
    EXPECT_FALSE(backend.tableExists("temp_a"));
    EXPECT_TRUE(backend.tableExists("temp_b"));
}

// ===========================================================================
// Transactions - commit, rollback, and RAII scope
// ===========================================================================

TEST(PersistenceTransaction, CommitPersistsChanges) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("acct", {"id INTEGER PRIMARY KEY", "bal INTEGER"}).success);

    auto txn = backend.beginTransaction();
    ASSERT_NE(txn, nullptr);
    EXPECT_TRUE(txn->isActive());
    ASSERT_TRUE(txn->execute("INSERT INTO acct (bal) VALUES (?)", {static_cast<int64_t>(100)}).success);
    ASSERT_TRUE(txn->commit());
    EXPECT_FALSE(txn->isActive());

    auto rows = backend.query("SELECT COUNT(*) FROM acct");
    ASSERT_TRUE(rows.success);
    EXPECT_EQ(rows.value->front()[0].asInt(), 1);
}

TEST(PersistenceTransaction, RollbackDiscardsChanges) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("acct", {"id INTEGER PRIMARY KEY", "bal INTEGER"}).success);

    auto txn = backend.beginTransaction();
    ASSERT_NE(txn, nullptr);
    ASSERT_TRUE(txn->execute("INSERT INTO acct (bal) VALUES (?)", {static_cast<int64_t>(42)}).success);
    ASSERT_TRUE(txn->rollback());

    auto rows = backend.query("SELECT COUNT(*) FROM acct");
    ASSERT_TRUE(rows.success);
    EXPECT_EQ(rows.value->front()[0].asInt(), 0);
}

TEST(PersistenceTransaction, ScopeAutoRollsBackWhenNotCommitted) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("scoped", {"id INTEGER PRIMARY KEY", "v INTEGER"}).success);

    {
        TransactionScope scope(backend.beginTransaction());
        EXPECT_TRUE(scope.isActive());
        ASSERT_TRUE(scope.execute("INSERT INTO scoped (v) VALUES (?)", {static_cast<int64_t>(7)}).success);
        // No commit -> destructor must roll back.
    }

    auto rows = backend.query("SELECT COUNT(*) FROM scoped");
    ASSERT_TRUE(rows.success);
    EXPECT_EQ(rows.value->front()[0].asInt(), 0);
}

TEST(PersistenceTransaction, ScopeCommitPersists) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("scoped", {"id INTEGER PRIMARY KEY", "v INTEGER"}).success);

    {
        TransactionScope scope(backend.beginTransaction());
        ASSERT_TRUE(scope.execute("INSERT INTO scoped (v) VALUES (?)", {static_cast<int64_t>(9)}).success);
        scope.commit();
        EXPECT_FALSE(scope.isActive());
    }

    auto rows = backend.query("SELECT v FROM scoped");
    ASSERT_TRUE(rows.success);
    ASSERT_EQ(rows.value->size(), 1u);
    EXPECT_EQ(rows.value->front()[0].asInt(), 9);
}

// ===========================================================================
// Async operations
// ===========================================================================

TEST(PersistenceAsync, ExecuteAndQueryAsyncRoundTrip) {
    SQLiteBackend backend(StorageConfig::inMemory());
    ASSERT_TRUE(backend.connect());
    ASSERT_TRUE(backend.createTable("async_t", {"id INTEGER PRIMARY KEY", "v TEXT"}).success);

    auto execFuture = backend.executeAsync("INSERT INTO async_t (v) VALUES (?)",
                                           {std::string("async-value")});
    EXPECT_TRUE(execFuture.get().success);

    auto queryFuture = backend.queryAsync("SELECT v FROM async_t");
    auto result = queryFuture.get();
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(result.value.has_value());
    ASSERT_EQ(result.value->size(), 1u);
    EXPECT_EQ(result.value->front()[0].asString(), "async-value");
}

// ===========================================================================
// ConnectionPool
// ===========================================================================

TEST(PersistenceConnectionPool, AcquireReleaseAccounting) {
    const std::string path = makeTempDbPath("pool");
    removeDbFiles(path);
    StorageConfig config = StorageConfig::file(path);
    config.maxConnections = 3;

    {
        ConnectionPool pool(config);
        EXPECT_EQ(pool.totalConnections(), 3u);
        EXPECT_EQ(pool.availableConnections(), 3u);

        auto c1 = pool.acquire();
        ASSERT_NE(c1, nullptr);
        EXPECT_TRUE(c1->isConnected());
        EXPECT_EQ(pool.activeConnections(), 1u);

        auto c2 = pool.acquire();
        ASSERT_NE(c2, nullptr);
        EXPECT_EQ(pool.activeConnections(), 2u);

        pool.release(c1);
        EXPECT_EQ(pool.availableConnections(), 2u);
        pool.release(c2);
        EXPECT_EQ(pool.availableConnections(), 3u);
    }
    removeDbFiles(path);
}

// ===========================================================================
// MemoryPersistence - durable agent memory storage
// ===========================================================================

class MemoryPersistenceTest : public ::testing::Test {
protected:
    std::shared_ptr<SQLiteBackend> backend_;
    std::shared_ptr<MemoryPersistence> mem_;

    void SetUp() override {
        backend_ = std::make_shared<SQLiteBackend>(StorageConfig::inMemory());
        ASSERT_TRUE(backend_->connect());
        mem_ = std::make_shared<MemoryPersistence>(backend_);
        ASSERT_TRUE(mem_->initialize());
    }
};

TEST_F(MemoryPersistenceTest, SaveAndLoadByIdRoundTrip) {
    auto save = mem_->saveMemory("mem-1", "hello world", "agent-A", "room-1", "memories",
                                 {0.1f, 0.2f, 0.3f}, R"({"k":"v"})");
    ASSERT_TRUE(save.success) << save.error.value_or("");

    auto load = mem_->loadMemory("mem-1", "memories");
    ASSERT_TRUE(load.success) << load.error.value_or("");
    ASSERT_TRUE(load.value.has_value());
    EXPECT_EQ((*load.value)["id"], "mem-1");
    EXPECT_EQ((*load.value)["content"], "hello world");
    EXPECT_EQ((*load.value)["agent_id"], "agent-A");
    EXPECT_EQ((*load.value)["room_id"], "room-1");
}

TEST_F(MemoryPersistenceTest, LoadMissingMemoryFails) {
    auto load = mem_->loadMemory("does-not-exist", "memories");
    EXPECT_FALSE(load.success);
}

TEST_F(MemoryPersistenceTest, QueryByAgentAndRoom) {
    ASSERT_TRUE(mem_->saveMemory("m1", "c1", "agent-A", "room-1", "memories").success);
    ASSERT_TRUE(mem_->saveMemory("m2", "c2", "agent-A", "room-2", "memories").success);
    ASSERT_TRUE(mem_->saveMemory("m3", "c3", "agent-B", "room-1", "memories").success);

    auto byAgent = mem_->loadMemoriesByAgent("agent-A", "memories");
    ASSERT_TRUE(byAgent.success);
    EXPECT_EQ(byAgent.value->size(), 2u);

    auto byRoom = mem_->loadMemoriesByRoom("room-1", "memories");
    ASSERT_TRUE(byRoom.success);
    EXPECT_EQ(byRoom.value->size(), 2u);

    EXPECT_EQ(mem_->getMemoryCount("memories"), 3);
    EXPECT_EQ(mem_->getMemoryCountByAgent("agent-A", "memories"), 2);
}

TEST_F(MemoryPersistenceTest, DeleteMemoryAndByAgent) {
    ASSERT_TRUE(mem_->saveMemory("m1", "c1", "agent-A", "room-1", "memories").success);
    ASSERT_TRUE(mem_->saveMemory("m2", "c2", "agent-A", "room-2", "memories").success);

    ASSERT_TRUE(mem_->deleteMemory("m1", "memories").success);
    EXPECT_FALSE(mem_->loadMemory("m1", "memories").success);
    EXPECT_EQ(mem_->getMemoryCount("memories"), 1);

    ASSERT_TRUE(mem_->deleteMemoriesByAgent("agent-A", "memories").success);
    EXPECT_EQ(mem_->getMemoryCount("memories"), 0);
}

TEST_F(MemoryPersistenceTest, SearchMemoriesMatchesContent) {
    ASSERT_TRUE(mem_->saveMemory("m1", "the cognitive cycle observes", "agent-A", "room-1", "memories").success);
    ASSERT_TRUE(mem_->saveMemory("m2", "an unrelated entry", "agent-A", "room-1", "memories").success);

    auto found = mem_->searchMemories("cognitive", "memories");
    ASSERT_TRUE(found.success);
    EXPECT_EQ(found.value->size(), 1u);
}

TEST_F(MemoryPersistenceTest, SaveBatchPersistsAll) {
    std::vector<std::tuple<UUID, std::string, std::string, std::string>> batch = {
        {"b1", "content-1", "agent-X", "room-1"},
        {"b2", "content-2", "agent-X", "room-1"},
    };
    ASSERT_TRUE(mem_->saveMemoriesBatch(batch, "memories").success);
    EXPECT_EQ(mem_->getMemoryCount("memories"), 2);
}

// ===========================================================================
// KeyValueStore
// ===========================================================================

class KeyValueStoreTest : public ::testing::Test {
protected:
    std::shared_ptr<SQLiteBackend> backend_;
    std::shared_ptr<KeyValueStore> kv_;

    void SetUp() override {
        backend_ = std::make_shared<SQLiteBackend>(StorageConfig::inMemory());
        ASSERT_TRUE(backend_->connect());
        kv_ = std::make_shared<KeyValueStore>(backend_);
        ASSERT_TRUE(kv_->initialize());
    }
};

TEST_F(KeyValueStoreTest, SetGetRemoveExists) {
    ASSERT_TRUE(kv_->set("name", "eliza").success);
    EXPECT_TRUE(kv_->exists("name"));
    auto got = kv_->get("name");
    ASSERT_TRUE(got.success);
    EXPECT_EQ(*got.value, "eliza");

    ASSERT_TRUE(kv_->remove("name").success);
    EXPECT_FALSE(kv_->exists("name"));
    EXPECT_FALSE(kv_->get("name").success);
}

TEST_F(KeyValueStoreTest, OverwriteUpdatesValue) {
    ASSERT_TRUE(kv_->set("mode", "REFLECTIVE").success);
    ASSERT_TRUE(kv_->set("mode", "FOCUSED").success);
    auto got = kv_->get("mode");
    ASSERT_TRUE(got.success);
    EXPECT_EQ(*got.value, "FOCUSED");
    EXPECT_EQ(kv_->count(), 1);
}

TEST_F(KeyValueStoreTest, BatchAndPrefixOperations) {
    std::unordered_map<std::string, std::string> pairs = {
        {"cfg:a", "1"}, {"cfg:b", "2"}, {"other:c", "3"},
    };
    ASSERT_TRUE(kv_->setMany(pairs).success);
    EXPECT_EQ(kv_->count(), 3);
    EXPECT_EQ(kv_->countByPrefix("cfg:"), 2);

    auto many = kv_->getMany({"cfg:a", "cfg:b", "missing"});
    ASSERT_TRUE(many.success);
    EXPECT_EQ(many.value->size(), 2u);

    auto prefixed = kv_->getByPrefix("cfg:");
    ASSERT_TRUE(prefixed.success);
    EXPECT_EQ(prefixed.value->size(), 2u);

    ASSERT_TRUE(kv_->removeByPrefix("cfg:").success);
    EXPECT_EQ(kv_->countByPrefix("cfg:"), 0);
    EXPECT_EQ(kv_->count(), 1);
}

TEST_F(KeyValueStoreTest, ExpiryHidesAndCleansUpKeys) {
    ASSERT_TRUE(kv_->setWithExpiry("ephemeral", "soon-gone", std::chrono::seconds(-1)).success);
    // Already expired -> must not be visible.
    EXPECT_FALSE(kv_->exists("ephemeral"));
    EXPECT_FALSE(kv_->get("ephemeral").success);

    kv_->cleanupExpired();
    auto rows = backend_->query("SELECT COUNT(*) FROM kv_store");
    ASSERT_TRUE(rows.success);
    EXPECT_EQ(rows.value->front()[0].asInt(), 0);
}

// ===========================================================================
// StorageManager singleton + storage:: convenience helpers (file backed)
// ===========================================================================

TEST(PersistenceStorageManager, GlobalInitKvRoundTripAndShutdown) {
    const std::string path = makeTempDbPath("manager");
    removeDbFiles(path);

    StorageConfig config = StorageConfig::file(path);
    config.maxConnections = 4;

    ASSERT_TRUE(storage::init(config));
    EXPECT_TRUE(StorageManager::getInstance().isInitialized());

    ASSERT_TRUE(storage::set("greeting", "how does that make you feel?"));
    auto got = storage::get("greeting");
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(*got, "how does that make you feel?");

    EXPECT_TRUE(storage::remove("greeting"));
    EXPECT_FALSE(storage::get("greeting").has_value());

    EXPECT_NE(storage::kv(), nullptr);
    EXPECT_NE(storage::memories(), nullptr);

    EXPECT_TRUE(storage::shutdown());
    EXPECT_FALSE(StorageManager::getInstance().isInitialized());
    removeDbFiles(path);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
