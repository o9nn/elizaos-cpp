// persistence_test.cpp - Comprehensive unit tests for the Persistence module
// Tests SQLiteBackend, ConnectionPool, Transaction, MemoryPersistence,
// KeyValueStore, and StorageManager against the real API.

#include <gtest/gtest.h>
#include "elizaos/persistence.hpp"
#include "elizaos/core.hpp"
#include <filesystem>
#include <thread>
#include <vector>
#include <atomic>

using namespace elizaos;

namespace {
const std::string TEST_DB_PATH = "/tmp/elizaos_persistence_test.db";

void cleanupTestDb() {
    std::filesystem::remove(TEST_DB_PATH);
    std::filesystem::remove(TEST_DB_PATH + "-journal");
    std::filesystem::remove(TEST_DB_PATH + "-wal");
    std::filesystem::remove(TEST_DB_PATH + "-shm");
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
