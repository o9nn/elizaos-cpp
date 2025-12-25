#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "elizaos/elizas_list.hpp"
#include <memory>
#include <string>
#include <vector>

using namespace elizaos;
using namespace ::testing;

class ElizasListTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }
};

// ============================================================================
// ListItem Tests
// ============================================================================

TEST_F(ElizasListTest, ListItemCreation) {
    ListItem item;
    item.id = "item-001";
    item.name = "Test Item";
    item.description = "A test item for unit testing";
    item.category = "testing";

    EXPECT_EQ(item.id, "item-001");
    EXPECT_EQ(item.name, "Test Item");
    EXPECT_EQ(item.description, "A test item for unit testing");
    EXPECT_EQ(item.category, "testing");
}

TEST_F(ElizasListTest, ListItemTags) {
    ListItem item;
    item.id = "item-001";
    item.tags = {"urgent", "important", "review"};

    EXPECT_EQ(item.tags.size(), 3);
    EXPECT_THAT(item.tags, Contains("urgent"));
    EXPECT_THAT(item.tags, Contains("important"));
}

TEST_F(ElizasListTest, ListItemMetadata) {
    ListItem item;
    item.id = "item-001";
    item.metadata["priority"] = "high";
    item.metadata["source"] = "api";
    item.metadata["version"] = "1.0";

    EXPECT_EQ(item.metadata.size(), 3);
    EXPECT_EQ(item.metadata["priority"], "high");
}

TEST_F(ElizasListTest, ListItemTimestamps) {
    ListItem item;
    item.id = "item-001";
    item.createdAt = std::chrono::system_clock::now();
    item.updatedAt = item.createdAt;

    EXPECT_LE(item.createdAt, item.updatedAt);
}

// ============================================================================
// ListCategory Tests
// ============================================================================

TEST_F(ElizasListTest, ListCategoryCreation) {
    ListCategory category;
    category.id = "cat-001";
    category.name = "Tasks";
    category.description = "Task items";
    category.color = "#FF5733";

    EXPECT_EQ(category.id, "cat-001");
    EXPECT_EQ(category.name, "Tasks");
    EXPECT_EQ(category.color, "#FF5733");
}

TEST_F(ElizasListTest, ListCategoryWithItems) {
    ListCategory category;
    category.id = "cat-001";
    category.name = "Tasks";

    ListItem item1;
    item1.id = "item-001";
    item1.name = "Task 1";
    category.items.push_back(item1);

    ListItem item2;
    item2.id = "item-002";
    item2.name = "Task 2";
    category.items.push_back(item2);

    EXPECT_EQ(category.items.size(), 2);
}

// ============================================================================
// List Tests
// ============================================================================

TEST_F(ElizasListTest, ListCreation) {
    ElizaList list;
    list.id = "list-001";
    list.name = "My List";
    list.description = "A personal list";

    EXPECT_EQ(list.id, "list-001");
    EXPECT_EQ(list.name, "My List");
}

TEST_F(ElizasListTest, ListAddItem) {
    ElizaList list;
    list.id = "list-001";

    ListItem item;
    item.id = "item-001";
    item.name = "New Item";

    list.addItem(item);

    EXPECT_EQ(list.itemCount(), 1);
    EXPECT_TRUE(list.hasItem("item-001"));
}

TEST_F(ElizasListTest, ListRemoveItem) {
    ElizaList list;
    list.id = "list-001";

    ListItem item;
    item.id = "item-001";
    list.addItem(item);

    EXPECT_EQ(list.itemCount(), 1);

    list.removeItem("item-001");

    EXPECT_EQ(list.itemCount(), 0);
    EXPECT_FALSE(list.hasItem("item-001"));
}

TEST_F(ElizasListTest, ListGetItem) {
    ElizaList list;
    list.id = "list-001";

    ListItem item;
    item.id = "item-001";
    item.name = "Test Item";
    list.addItem(item);

    auto retrieved = list.getItem("item-001");
    EXPECT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->name, "Test Item");
}

TEST_F(ElizasListTest, ListUpdateItem) {
    ElizaList list;
    list.id = "list-001";

    ListItem item;
    item.id = "item-001";
    item.name = "Original Name";
    list.addItem(item);

    ListItem updated;
    updated.id = "item-001";
    updated.name = "Updated Name";
    list.updateItem(updated);

    auto retrieved = list.getItem("item-001");
    EXPECT_EQ(retrieved->name, "Updated Name");
}

// ============================================================================
// List Filtering Tests
// ============================================================================

TEST_F(ElizasListTest, ListFilterByCategory) {
    ElizaList list;

    ListItem item1;
    item1.id = "item-001";
    item1.category = "work";
    list.addItem(item1);

    ListItem item2;
    item2.id = "item-002";
    item2.category = "personal";
    list.addItem(item2);

    ListItem item3;
    item3.id = "item-003";
    item3.category = "work";
    list.addItem(item3);

    auto filtered = list.filterByCategory("work");
    EXPECT_EQ(filtered.size(), 2);
}

TEST_F(ElizasListTest, ListFilterByTag) {
    ElizaList list;

    ListItem item1;
    item1.id = "item-001";
    item1.tags = {"urgent"};
    list.addItem(item1);

    ListItem item2;
    item2.id = "item-002";
    item2.tags = {"important"};
    list.addItem(item2);

    ListItem item3;
    item3.id = "item-003";
    item3.tags = {"urgent", "important"};
    list.addItem(item3);

    auto filtered = list.filterByTag("urgent");
    EXPECT_EQ(filtered.size(), 2);
}

TEST_F(ElizasListTest, ListSearch) {
    ElizaList list;

    ListItem item1;
    item1.id = "item-001";
    item1.name = "Buy groceries";
    list.addItem(item1);

    ListItem item2;
    item2.id = "item-002";
    item2.name = "Call dentist";
    list.addItem(item2);

    ListItem item3;
    item3.id = "item-003";
    item3.name = "Buy new laptop";
    list.addItem(item3);

    auto results = list.search("buy");
    EXPECT_EQ(results.size(), 2);
}

// ============================================================================
// List Sorting Tests
// ============================================================================

TEST_F(ElizasListTest, ListSortByName) {
    ElizaList list;

    ListItem item1;
    item1.id = "item-001";
    item1.name = "Zebra";
    list.addItem(item1);

    ListItem item2;
    item2.id = "item-002";
    item2.name = "Apple";
    list.addItem(item2);

    ListItem item3;
    item3.id = "item-003";
    item3.name = "Mango";
    list.addItem(item3);

    list.sortByName(SortOrder::ASCENDING);

    auto items = list.getAllItems();
    EXPECT_EQ(items[0].name, "Apple");
    EXPECT_EQ(items[1].name, "Mango");
    EXPECT_EQ(items[2].name, "Zebra");
}

TEST_F(ElizasListTest, ListSortByCreatedAt) {
    ElizaList list;

    auto now = std::chrono::system_clock::now();

    ListItem item1;
    item1.id = "item-001";
    item1.createdAt = now - std::chrono::hours(24);
    list.addItem(item1);

    ListItem item2;
    item2.id = "item-002";
    item2.createdAt = now;
    list.addItem(item2);

    list.sortByCreatedAt(SortOrder::DESCENDING);

    auto items = list.getAllItems();
    EXPECT_EQ(items[0].id, "item-002"); // Most recent first
}

// ============================================================================
// List Statistics Tests
// ============================================================================

TEST_F(ElizasListTest, ListStatistics) {
    ElizaList list;

    for (int i = 0; i < 5; ++i) {
        ListItem item;
        item.id = "item-" + std::to_string(i);
        item.category = (i % 2 == 0) ? "even" : "odd";
        list.addItem(item);
    }

    auto stats = list.getStatistics();

    EXPECT_EQ(stats.totalItems, 5);
    EXPECT_EQ(stats.categoryCounts["even"], 3);
    EXPECT_EQ(stats.categoryCounts["odd"], 2);
}

// ============================================================================
// List Serialization Tests
// ============================================================================

TEST_F(ElizasListTest, ListToJson) {
    ElizaList list;
    list.id = "list-001";
    list.name = "Test List";

    ListItem item;
    item.id = "item-001";
    item.name = "Test Item";
    list.addItem(item);

    std::string json = list.toJson();

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("list-001"), std::string::npos);
    EXPECT_NE(json.find("Test List"), std::string::npos);
}

TEST_F(ElizasListTest, ListFromJson) {
    std::string json = R"({
        "id": "list-001",
        "name": "Parsed List",
        "items": [
            {"id": "item-001", "name": "Item 1"}
        ]
    })";

    auto result = ElizaList::fromJson(json);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.list.id, "list-001");
    EXPECT_EQ(result.list.name, "Parsed List");
    EXPECT_EQ(result.list.itemCount(), 1);
}
