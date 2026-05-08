#include <gtest/gtest.h>
#include "elizaos/classified.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace elizaos::classified;

class ClassifiedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// ==============================================================================
// CLASSIFIED DOCUMENT TESTS
// ==============================================================================

TEST_F(ClassifiedTest, CreateClassifiedDocument) {
    ClassifiedDocument doc("test-id", "Test Content", SecurityLevel::SECRET);
    
    EXPECT_EQ(doc.getId(), "test-id");
    EXPECT_EQ(doc.getContent(), "Test Content");
    EXPECT_EQ(doc.getSecurityLevel(), SecurityLevel::SECRET);
    EXPECT_FALSE(doc.getCreatedAt().time_since_epoch().count() == 0);
}

TEST_F(ClassifiedTest, DocumentSecurityLevels) {
    ClassifiedDocument unclassified("id1", "Public", SecurityLevel::UNCLASSIFIED);
    ClassifiedDocument confidential("id2", "Private", SecurityLevel::CONFIDENTIAL);
    ClassifiedDocument secret("id3", "Secret", SecurityLevel::SECRET);
    ClassifiedDocument topSecret("id4", "Top Secret", SecurityLevel::TOP_SECRET);
    
    EXPECT_EQ(unclassified.getSecurityLevel(), SecurityLevel::UNCLASSIFIED);
    EXPECT_EQ(confidential.getSecurityLevel(), SecurityLevel::CONFIDENTIAL);
    EXPECT_EQ(secret.getSecurityLevel(), SecurityLevel::SECRET);
    EXPECT_EQ(topSecret.getSecurityLevel(), SecurityLevel::TOP_SECRET);
}

TEST_F(ClassifiedTest, DocumentMetadata) {
    ClassifiedDocument doc("id", "Content", SecurityLevel::CONFIDENTIAL);
    
    doc.setMetadata("author", "John Doe");
    doc.setMetadata("department", "R&D");
    doc.setMetadata("project", "Project X");
    
    EXPECT_EQ(doc.getMetadata("author"), "John Doe");
    EXPECT_EQ(doc.getMetadata("department"), "R&D");
    EXPECT_EQ(doc.getMetadata("project"), "Project X");
    EXPECT_EQ(doc.getMetadata("nonexistent"), "");
}

TEST_F(ClassifiedTest, DocumentAccessLog) {
    ClassifiedDocument doc("id", "Content", SecurityLevel::SECRET);
    
    doc.logAccess("user1", AccessType::READ);
    doc.logAccess("user2", AccessType::WRITE);
    doc.logAccess("user1", AccessType::READ);
    
    auto accessLog = doc.getAccessLog();
    EXPECT_EQ(accessLog.size(), 3);
    
    EXPECT_EQ(accessLog[0].userId, "user1");
    EXPECT_EQ(accessLog[0].accessType, AccessType::READ);
    
    EXPECT_EQ(accessLog[1].userId, "user2");
    EXPECT_EQ(accessLog[1].accessType, AccessType::WRITE);
}

TEST_F(ClassifiedTest, DocumentTags) {
    ClassifiedDocument doc("id", "Content", SecurityLevel::CONFIDENTIAL);
    
    doc.addTag("sensitive");
    doc.addTag("financial");
    doc.addTag("quarterly");
    
    auto tags = doc.getTags();
    EXPECT_EQ(tags.size(), 3);
    EXPECT_NE(std::find(tags.begin(), tags.end(), "sensitive"), tags.end());
    EXPECT_NE(std::find(tags.begin(), tags.end(), "financial"), tags.end());
    EXPECT_NE(std::find(tags.begin(), tags.end(), "quarterly"), tags.end());
}

TEST_F(ClassifiedTest, DocumentUpdateContent) {
    ClassifiedDocument doc("id", "Original", SecurityLevel::SECRET);
    EXPECT_EQ(doc.getContent(), "Original");
    
    doc.updateContent("Updated Content");
    EXPECT_EQ(doc.getContent(), "Updated Content");
}

TEST_F(ClassifiedTest, DocumentUpdateSecurityLevel) {
    ClassifiedDocument doc("id", "Content", SecurityLevel::CONFIDENTIAL);
    EXPECT_EQ(doc.getSecurityLevel(), SecurityLevel::CONFIDENTIAL);
    
    doc.updateSecurityLevel(SecurityLevel::TOP_SECRET);
    EXPECT_EQ(doc.getSecurityLevel(), SecurityLevel::TOP_SECRET);
}

// ==============================================================================
// SECURITY CLEARANCE TESTS
// ==============================================================================

TEST_F(ClassifiedTest, CreateSecurityClearance) {
    SecurityClearance clearance("user1", SecurityLevel::SECRET);
    
    EXPECT_EQ(clearance.getUserId(), "user1");
    EXPECT_EQ(clearance.getLevel(), SecurityLevel::SECRET);
    EXPECT_TRUE(clearance.isActive());
}

TEST_F(ClassifiedTest, ClearanceExpiration) {
    auto now = std::chrono::system_clock::now();
    auto std::future = now + std::chrono::hours(24);
    auto past = now - std::chrono::hours(1);
    
    SecurityClearance activeClearance("user1", SecurityLevel::SECRET, std::future);
    EXPECT_TRUE(activeClearance.isActive());
    EXPECT_FALSE(activeClearance.isExpired());
    
    SecurityClearance expiredClearance("user2", SecurityLevel::SECRET, past);
    EXPECT_FALSE(expiredClearance.isActive());
    EXPECT_TRUE(expiredClearance.isExpired());
}

TEST_F(ClassifiedTest, ClearanceRevocation) {
    SecurityClearance clearance("user1", SecurityLevel::SECRET);
    EXPECT_TRUE(clearance.isActive());
    
    clearance.revoke();
    EXPECT_FALSE(clearance.isActive());
}

TEST_F(ClassifiedTest, ClearancePermissions) {
    SecurityClearance clearance("user1", SecurityLevel::SECRET);
    
    clearance.addPermission("read_documents");
    clearance.addPermission("write_reports");
    clearance.addPermission("approve_requests");
    
    EXPECT_TRUE(clearance.hasPermission("read_documents"));
    EXPECT_TRUE(clearance.hasPermission("write_reports"));
    EXPECT_TRUE(clearance.hasPermission("approve_requests"));
    EXPECT_FALSE(clearance.hasPermission("delete_records"));
}

TEST_F(ClassifiedTest, ClearanceRemovePermission) {
    SecurityClearance clearance("user1", SecurityLevel::SECRET);
    
    clearance.addPermission("read_documents");
    clearance.addPermission("write_reports");
    
    EXPECT_TRUE(clearance.hasPermission("read_documents"));
    
    clearance.removePermission("read_documents");
    EXPECT_FALSE(clearance.hasPermission("read_documents"));
    EXPECT_TRUE(clearance.hasPermission("write_reports"));
}

// ==============================================================================
// ACCESS CONTROL MANAGER TESTS
// ==============================================================================

TEST_F(ClassifiedTest, CreateAccessControlManager) {
    AccessControlManager manager;
    // Manager should be created successfully
    EXPECT_TRUE(true);
}

TEST_F(ClassifiedTest, GrantClearance) {
    AccessControlManager manager;
    
    bool granted = manager.grantClearance("user1", SecurityLevel::SECRET);
    EXPECT_TRUE(granted);
    
    EXPECT_TRUE(manager.hasClearance("user1"));
}

TEST_F(ClassifiedTest, RevokeClearance) {
    AccessControlManager manager;
    
    manager.grantClearance("user1", SecurityLevel::SECRET);
    EXPECT_TRUE(manager.hasClearance("user1"));
    
    bool revoked = manager.revokeClearance("user1");
    EXPECT_TRUE(revoked);
    EXPECT_FALSE(manager.hasClearance("user1"));
}

TEST_F(ClassifiedTest, CheckAccessPermission) {
    AccessControlManager manager;
    
    manager.grantClearance("user1", SecurityLevel::SECRET);
    manager.grantClearance("user2", SecurityLevel::CONFIDENTIAL);
    
    // User with SECRET clearance can access SECRET documents
    EXPECT_TRUE(manager.checkAccess("user1", SecurityLevel::SECRET));
    
    // User with SECRET clearance can access lower level documents
    EXPECT_TRUE(manager.checkAccess("user1", SecurityLevel::CONFIDENTIAL));
    EXPECT_TRUE(manager.checkAccess("user1", SecurityLevel::UNCLASSIFIED));
    
    // User with CONFIDENTIAL clearance cannot access SECRET documents
    EXPECT_FALSE(manager.checkAccess("user2", SecurityLevel::SECRET));
    
    // User with CONFIDENTIAL clearance can access CONFIDENTIAL documents
    EXPECT_TRUE(manager.checkAccess("user2", SecurityLevel::CONFIDENTIAL));
}

TEST_F(ClassifiedTest, CheckAccessWithoutClearance) {
    AccessControlManager manager;
    
    // User without clearance cannot access std::any classified documents
    EXPECT_FALSE(manager.checkAccess("user1", SecurityLevel::UNCLASSIFIED));
    EXPECT_FALSE(manager.checkAccess("user1", SecurityLevel::CONFIDENTIAL));
    EXPECT_FALSE(manager.checkAccess("user1", SecurityLevel::SECRET));
}

TEST_F(ClassifiedTest, GetClearanceLevel) {
    AccessControlManager manager;
    
    manager.grantClearance("user1", SecurityLevel::TOP_SECRET);
    manager.grantClearance("user2", SecurityLevel::CONFIDENTIAL);
    
    EXPECT_EQ(manager.getClearanceLevel("user1"), SecurityLevel::TOP_SECRET);
    EXPECT_EQ(manager.getClearanceLevel("user2"), SecurityLevel::CONFIDENTIAL);
    EXPECT_EQ(manager.getClearanceLevel("user3"), SecurityLevel::UNCLASSIFIED);
}

TEST_F(ClassifiedTest, ListUsersWithClearance) {
    AccessControlManager manager;
    
    manager.grantClearance("user1", SecurityLevel::SECRET);
    manager.grantClearance("user2", SecurityLevel::CONFIDENTIAL);
    manager.grantClearance("user3", SecurityLevel::TOP_SECRET);
    
    auto users = manager.listUsersWithClearance();
    EXPECT_EQ(users.size(), 3);
    
    EXPECT_NE(std::find(users.begin(), users.end(), "user1"), users.end());
    EXPECT_NE(std::find(users.begin(), users.end(), "user2"), users.end());
    EXPECT_NE(std::find(users.begin(), users.end(), "user3"), users.end());
}

TEST_F(ClassifiedTest, ListUsersWithMinimumLevel) {
    AccessControlManager manager;
    
    manager.grantClearance("user1", SecurityLevel::CONFIDENTIAL);
    manager.grantClearance("user2", SecurityLevel::SECRET);
    manager.grantClearance("user3", SecurityLevel::TOP_SECRET);
    
    auto secretUsers = manager.listUsersWithMinimumLevel(SecurityLevel::SECRET);
    EXPECT_EQ(secretUsers.size(), 2);
    EXPECT_NE(std::find(secretUsers.begin(), secretUsers.end(), "user2"), secretUsers.end());
    EXPECT_NE(std::find(secretUsers.begin(), secretUsers.end(), "user3"), secretUsers.end());
}

TEST_F(ClassifiedTest, UpdateClearanceLevel) {
    AccessControlManager manager;
    
    manager.grantClearance("user1", SecurityLevel::CONFIDENTIAL);
    EXPECT_EQ(manager.getClearanceLevel("user1"), SecurityLevel::CONFIDENTIAL);
    
    bool updated = manager.updateClearanceLevel("user1", SecurityLevel::SECRET);
    EXPECT_TRUE(updated);
    EXPECT_EQ(manager.getClearanceLevel("user1"), SecurityLevel::SECRET);
}

// ==============================================================================
// DOCUMENT REPOSITORY TESTS
// ==============================================================================

TEST_F(ClassifiedTest, CreateDocumentRepository) {
    DocumentRepository repo;
    // Repository should be created successfully
    EXPECT_TRUE(true);
}

TEST_F(ClassifiedTest, StoreDocument) {
    DocumentRepository repo;
    ClassifiedDocument doc("doc1", "Content", SecurityLevel::SECRET);
    
    bool stored = repo.storeDocument(doc);
    EXPECT_TRUE(stored);
    EXPECT_TRUE(repo.hasDocument("doc1"));
}

TEST_F(ClassifiedTest, RetrieveDocument) {
    DocumentRepository repo;
    ClassifiedDocument doc("doc1", "Test Content", SecurityLevel::SECRET);
    
    repo.storeDocument(doc);
    
    auto retrieved = repo.retrieveDocument("doc1");
    EXPECT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getId(), "doc1");
    EXPECT_EQ(retrieved->getContent(), "Test Content");
    EXPECT_EQ(retrieved->getSecurityLevel(), SecurityLevel::SECRET);
}

TEST_F(ClassifiedTest, RetrieveNonexistentDocument) {
    DocumentRepository repo;
    
    auto retrieved = repo.retrieveDocument("nonexistent");
    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(ClassifiedTest, DeleteDocument) {
    DocumentRepository repo;
    ClassifiedDocument doc("doc1", "Content", SecurityLevel::SECRET);
    
    repo.storeDocument(doc);
    EXPECT_TRUE(repo.hasDocument("doc1"));
    
    bool deleted = repo.deleteDocument("doc1");
    EXPECT_TRUE(deleted);
    EXPECT_FALSE(repo.hasDocument("doc1"));
}

TEST_F(ClassifiedTest, SearchDocumentsBySecurityLevel) {
    DocumentRepository repo;
    
    repo.storeDocument(ClassifiedDocument("doc1", "Content1", SecurityLevel::UNCLASSIFIED));
    repo.storeDocument(ClassifiedDocument("doc2", "Content2", SecurityLevel::CONFIDENTIAL));
    repo.storeDocument(ClassifiedDocument("doc3", "Content3", SecurityLevel::SECRET));
    repo.storeDocument(ClassifiedDocument("doc4", "Content4", SecurityLevel::SECRET));
    
    auto secretDocs = repo.searchBySecurityLevel(SecurityLevel::SECRET);
    EXPECT_EQ(secretDocs.size(), 2);
}

TEST_F(ClassifiedTest, SearchDocumentsByTag) {
    DocumentRepository repo;
    
    ClassifiedDocument doc1("doc1", "Content1", SecurityLevel::SECRET);
    doc1.addTag("financial");
    doc1.addTag("quarterly");
    
    ClassifiedDocument doc2("doc2", "Content2", SecurityLevel::SECRET);
    doc2.addTag("financial");
    doc2.addTag("annual");
    
    ClassifiedDocument doc3("doc3", "Content3", SecurityLevel::SECRET);
    doc3.addTag("technical");
    
    repo.storeDocument(doc1);
    repo.storeDocument(doc2);
    repo.storeDocument(doc3);
    
    auto financialDocs = repo.searchByTag("financial");
    EXPECT_EQ(financialDocs.size(), 2);
    
    auto technicalDocs = repo.searchByTag("technical");
    EXPECT_EQ(technicalDocs.size(), 1);
}

TEST_F(ClassifiedTest, ListAllDocuments) {
    DocumentRepository repo;
    
    repo.storeDocument(ClassifiedDocument("doc1", "Content1", SecurityLevel::UNCLASSIFIED));
    repo.storeDocument(ClassifiedDocument("doc2", "Content2", SecurityLevel::CONFIDENTIAL));
    repo.storeDocument(ClassifiedDocument("doc3", "Content3", SecurityLevel::SECRET));
    
    auto allDocs = repo.listAllDocuments();
    EXPECT_EQ(allDocs.size(), 3);
}

// ==============================================================================
// INTEGRATION TESTS
// ==============================================================================

TEST_F(ClassifiedTest, CompleteWorkflow) {
    AccessControlManager acm;
    DocumentRepository repo;
    
    // Grant clearances
    acm.grantClearance("alice", SecurityLevel::TOP_SECRET);
    acm.grantClearance("bob", SecurityLevel::CONFIDENTIAL);
    
    // Create and store documents
    ClassifiedDocument topSecretDoc("ts1", "Top Secret Data", SecurityLevel::TOP_SECRET);
    ClassifiedDocument secretDoc("s1", "Secret Data", SecurityLevel::SECRET);
    ClassifiedDocument confidentialDoc("c1", "Confidential Data", SecurityLevel::CONFIDENTIAL);
    
    repo.storeDocument(topSecretDoc);
    repo.storeDocument(secretDoc);
    repo.storeDocument(confidentialDoc);
    
    // Check access permissions
    EXPECT_TRUE(acm.checkAccess("alice", SecurityLevel::TOP_SECRET));
    EXPECT_TRUE(acm.checkAccess("alice", SecurityLevel::SECRET));
    EXPECT_TRUE(acm.checkAccess("alice", SecurityLevel::CONFIDENTIAL));
    
    EXPECT_FALSE(acm.checkAccess("bob", SecurityLevel::TOP_SECRET));
    EXPECT_FALSE(acm.checkAccess("bob", SecurityLevel::SECRET));
    EXPECT_TRUE(acm.checkAccess("bob", SecurityLevel::CONFIDENTIAL));
    
    // Retrieve documents with access control
    if (acm.checkAccess("alice", SecurityLevel::TOP_SECRET)) {
        auto doc = repo.retrieveDocument("ts1");
        EXPECT_TRUE(doc.has_value());
        doc->logAccess("alice", AccessType::READ);
    }
    
    if (!acm.checkAccess("bob", SecurityLevel::SECRET)) {
        auto doc = repo.retrieveDocument("s1");
        EXPECT_TRUE(doc.has_value());
        // Bob should not be able to access this document
    }
}

TEST_F(ClassifiedTest, ConcurrentAccess) {
    AccessControlManager acm;
    std::atomic<int> successCount{0};
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&acm, &successCount, i]() {
            std::string userId = "user" + std::to_string(i);
            bool granted = acm.grantClearance(userId, SecurityLevel::SECRET);
            if (granted && acm.hasClearance(userId)) {
                successCount++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount.load(), 10);
}

TEST_F(ClassifiedTest, ConcurrentDocumentOperations) {
    DocumentRepository repo;
    std::atomic<int> storeCount{0};
    std::atomic<int> retrieveCount{0};
    
    // Store initial documents
    for (int i = 0; i < 10; ++i) {
        std::string docId = "doc" + std::to_string(i);
        repo.storeDocument(ClassifiedDocument(docId, "Content", SecurityLevel::SECRET));
    }
    
    std::vector<std::thread> threads;
    
    // Concurrent retrieval
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&repo, &retrieveCount, i]() {
            std::string docId = "doc" + std::to_string(i);
            auto doc = repo.retrieveDocument(docId);
            if (doc.has_value()) {
                retrieveCount++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(retrieveCount.load(), 10);
}

// ==============================================================================
// SECURITY LEVEL COMPARISON TESTS
// ==============================================================================

TEST_F(ClassifiedTest, SecurityLevelHierarchy) {
    EXPECT_LT(static_cast<int>(SecurityLevel::UNCLASSIFIED), 
              static_cast<int>(SecurityLevel::CONFIDENTIAL));
    EXPECT_LT(static_cast<int>(SecurityLevel::CONFIDENTIAL), 
              static_cast<int>(SecurityLevel::SECRET));
    EXPECT_LT(static_cast<int>(SecurityLevel::SECRET), 
              static_cast<int>(SecurityLevel::TOP_SECRET));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
