#include <gtest/gtest.h>
#include "elizaos/hat.hpp"
#include <thread>
#include <chrono>
#include <vector>

using namespace elizaos::hat;

class HATProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// ==============================================================================
// HAT TOKEN CREATION TESTS
// ==============================================================================

TEST_F(HATProtocolTest, IssueBasicToken) {
    std::vector<std::string> permissions = {"read", "write"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_FALSE(tokenId.empty());
    EXPECT_NE(tokenId.find("HAT_"), std::string::npos);
}

TEST_F(HATProtocolTest, IssueMultipleTokens) {
    std::vector<std::string> permissions1 = {"read"};
    std::vector<std::string> permissions2 = {"write"};
    
    std::string token1 = issueHATToken("agent1", "user1", permissions1);
    std::string token2 = issueHATToken("agent2", "user2", permissions2);
    
    EXPECT_FALSE(token1.empty());
    EXPECT_FALSE(token2.empty());
    EXPECT_NE(token1, token2);
}

TEST_F(HATProtocolTest, IssueTokenWithMultiplePermissions) {
    std::vector<std::string> permissions = {
        "read", "write", "delete", "admin"
    };
    
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    EXPECT_FALSE(tokenId.empty());
    
    // Verify all permissions
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
    EXPECT_TRUE(checkHATPermission(tokenId, "write"));
    EXPECT_TRUE(checkHATPermission(tokenId, "delete"));
    EXPECT_TRUE(checkHATPermission(tokenId, "admin"));
}

TEST_F(HATProtocolTest, IssueTokenWithNoPermissions) {
    std::vector<std::string> permissions;
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_FALSE(tokenId.empty());
    EXPECT_FALSE(checkHATPermission(tokenId, "read"));
    EXPECT_FALSE(checkHATPermission(tokenId, "write"));
}

// ==============================================================================
// TOKEN VALIDATION TESTS
// ==============================================================================

TEST_F(HATProtocolTest, ValidateValidToken) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(validateHATToken(tokenId));
}

TEST_F(HATProtocolTest, ValidateInvalidToken) {
    EXPECT_FALSE(validateHATToken("invalid-token-id"));
    EXPECT_FALSE(validateHATToken(""));
    EXPECT_FALSE(validateHATToken("HAT_999999_999999"));
}

TEST_F(HATProtocolTest, ValidateRevokedToken) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(validateHATToken(tokenId));
    
    revokeHATToken(tokenId);
    
    EXPECT_FALSE(validateHATToken(tokenId));
}

// ==============================================================================
// PERMISSION CHECKING TESTS
// ==============================================================================

TEST_F(HATProtocolTest, CheckExistingPermission) {
    std::vector<std::string> permissions = {"read", "write"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
    EXPECT_TRUE(checkHATPermission(tokenId, "write"));
}

TEST_F(HATProtocolTest, CheckNonexistentPermission) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_FALSE(checkHATPermission(tokenId, "write"));
    EXPECT_FALSE(checkHATPermission(tokenId, "delete"));
    EXPECT_FALSE(checkHATPermission(tokenId, "admin"));
}

TEST_F(HATProtocolTest, CheckPermissionOnInvalidToken) {
    EXPECT_FALSE(checkHATPermission("invalid-token", "read"));
    EXPECT_FALSE(checkHATPermission("", "write"));
}

TEST_F(HATProtocolTest, CheckPermissionAfterRevocation) {
    std::vector<std::string> permissions = {"read", "write"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
    
    revokeHATToken(tokenId);
    
    EXPECT_FALSE(checkHATPermission(tokenId, "read"));
    EXPECT_FALSE(checkHATPermission(tokenId, "write"));
}

// ==============================================================================
// TOKEN REVOCATION TESTS
// ==============================================================================

TEST_F(HATProtocolTest, RevokeValidToken) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(validateHATToken(tokenId));
    
    revokeHATToken(tokenId);
    
    EXPECT_FALSE(validateHATToken(tokenId));
}

TEST_F(HATProtocolTest, RevokeInvalidToken) {
    // Should not crash or throw
    revokeHATToken("invalid-token");
    revokeHATToken("");
}

TEST_F(HATProtocolTest, RevokeAlreadyRevokedToken) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    revokeHATToken(tokenId);
    EXPECT_FALSE(validateHATToken(tokenId));
    
    // Revoking again should not cause issues
    revokeHATToken(tokenId);
    EXPECT_FALSE(validateHATToken(tokenId));
}

// ==============================================================================
// MULTI-AGENT TESTS
// ==============================================================================

TEST_F(HATProtocolTest, MultipleAgentsMultipleUsers) {
    std::vector<std::string> permissions1 = {"read"};
    std::vector<std::string> permissions2 = {"write"};
    std::vector<std::string> permissions3 = {"admin"};
    
    std::string token1 = issueHATToken("agent1", "user1", permissions1);
    std::string token2 = issueHATToken("agent2", "user2", permissions2);
    std::string token3 = issueHATToken("agent3", "user3", permissions3);
    
    EXPECT_TRUE(validateHATToken(token1));
    EXPECT_TRUE(validateHATToken(token2));
    EXPECT_TRUE(validateHATToken(token3));
    
    EXPECT_TRUE(checkHATPermission(token1, "read"));
    EXPECT_FALSE(checkHATPermission(token1, "write"));
    
    EXPECT_TRUE(checkHATPermission(token2, "write"));
    EXPECT_FALSE(checkHATPermission(token2, "read"));
    
    EXPECT_TRUE(checkHATPermission(token3, "admin"));
    EXPECT_FALSE(checkHATPermission(token3, "read"));
}

TEST_F(HATProtocolTest, SameAgentMultipleUsers) {
    std::vector<std::string> permissions = {"read", "write"};
    
    std::string token1 = issueHATToken("agent1", "user1", permissions);
    std::string token2 = issueHATToken("agent1", "user2", permissions);
    std::string token3 = issueHATToken("agent1", "user3", permissions);
    
    EXPECT_NE(token1, token2);
    EXPECT_NE(token2, token3);
    EXPECT_NE(token1, token3);
    
    EXPECT_TRUE(validateHATToken(token1));
    EXPECT_TRUE(validateHATToken(token2));
    EXPECT_TRUE(validateHATToken(token3));
}

TEST_F(HATProtocolTest, SameUserMultipleAgents) {
    std::vector<std::string> permissions = {"read"};
    
    std::string token1 = issueHATToken("agent1", "user1", permissions);
    std::string token2 = issueHATToken("agent2", "user1", permissions);
    std::string token3 = issueHATToken("agent3", "user1", permissions);
    
    EXPECT_NE(token1, token2);
    EXPECT_NE(token2, token3);
    EXPECT_NE(token1, token3);
    
    EXPECT_TRUE(validateHATToken(token1));
    EXPECT_TRUE(validateHATToken(token2));
    EXPECT_TRUE(validateHATToken(token3));
}

// ==============================================================================
// PERMISSION SCENARIOS TESTS
// ==============================================================================

TEST_F(HATProtocolTest, ReadOnlyPermission) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
    EXPECT_FALSE(checkHATPermission(tokenId, "write"));
    EXPECT_FALSE(checkHATPermission(tokenId, "delete"));
    EXPECT_FALSE(checkHATPermission(tokenId, "admin"));
}

TEST_F(HATProtocolTest, ReadWritePermission) {
    std::vector<std::string> permissions = {"read", "write"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
    EXPECT_TRUE(checkHATPermission(tokenId, "write"));
    EXPECT_FALSE(checkHATPermission(tokenId, "delete"));
    EXPECT_FALSE(checkHATPermission(tokenId, "admin"));
}

TEST_F(HATProtocolTest, AdminPermission) {
    std::vector<std::string> permissions = {"admin"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_TRUE(checkHATPermission(tokenId, "admin"));
    EXPECT_FALSE(checkHATPermission(tokenId, "read"));
    EXPECT_FALSE(checkHATPermission(tokenId, "write"));
}

TEST_F(HATProtocolTest, FullPermissions) {
    std::vector<std::string> permissions = {
        "read", "write", "delete", "admin", "execute", "manage"
    };
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    for (const auto& perm : permissions) {
        EXPECT_TRUE(checkHATPermission(tokenId, perm));
    }
}

// ==============================================================================
// CONCURRENT ACCESS TESTS
// ==============================================================================

TEST_F(HATProtocolTest, ConcurrentTokenIssuance) {
    std::vector<std::thread> threads;
    std::vector<std::string> tokens(10);
    std::vector<std::string> permissions = {"read", "write"};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&tokens, &permissions, i]() {
            std::string agentId = "agent" + std::to_string(i);
            std::string userId = "user" + std::to_string(i);
            tokens[i] = issueHATToken(agentId, userId, permissions);
        });
    }
    
    for (auto& std::thread : threads) {
        thread.join();
    }
    
    // All tokens should be unique and valid
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_FALSE(tokens[i].empty());
        EXPECT_TRUE(validateHATToken(tokens[i]));
        
        for (size_t j = i + 1; j < tokens.size(); ++j) {
            EXPECT_NE(tokens[i], tokens[j]);
        }
    }
}

TEST_F(HATProtocolTest, ConcurrentTokenValidation) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    std::atomic<int> validCount{0};
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&tokenId, &validCount]() {
            for (int j = 0; j < 100; ++j) {
                if (validateHATToken(tokenId)) {
                    validCount++;
                }
            }
        });
    }
    
    for (auto& std::thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(validCount.load(), 1000);
}

TEST_F(HATProtocolTest, ConcurrentPermissionChecks) {
    std::vector<std::string> permissions = {"read", "write", "delete"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    std::atomic<int> readCount{0};
    std::atomic<int> writeCount{0};
    std::atomic<int> deleteCount{0};
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 100; ++j) {
                if (checkHATPermission(tokenId, "read")) readCount++;
                if (checkHATPermission(tokenId, "write")) writeCount++;
                if (checkHATPermission(tokenId, "delete")) deleteCount++;
            }
        });
    }
    
    for (auto& std::thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(readCount.load(), 1000);
    EXPECT_EQ(writeCount.load(), 1000);
    EXPECT_EQ(deleteCount.load(), 1000);
}

TEST_F(HATProtocolTest, ConcurrentRevocation) {
    std::vector<std::string> tokens;
    std::vector<std::string> permissions = {"read"};
    
    // Issue multiple tokens
    for (int i = 0; i < 10; ++i) {
        std::string agentId = "agent" + std::to_string(i);
        std::string userId = "user" + std::to_string(i);
        tokens.push_back(issueHATToken(agentId, userId, permissions));
    }
    
    // Revoke them concurrently
    std::vector<std::thread> threads;
    for (const auto& token : tokens) {
        threads.emplace_back([token]() {
            revokeHATToken(token);
        });
    }
    
    for (auto& std::thread : threads) {
        thread.join();
    }
    
    // All tokens should be invalid
    for (const auto& token : tokens) {
        EXPECT_FALSE(validateHATToken(token));
    }
}

// ==============================================================================
// EDGE CASE TESTS
// ==============================================================================

TEST_F(HATProtocolTest, EmptyAgentId) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("", "user1", permissions);
    
    EXPECT_FALSE(tokenId.empty());
    EXPECT_TRUE(validateHATToken(tokenId));
}

TEST_F(HATProtocolTest, EmptyUserId) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent1", "", permissions);
    
    EXPECT_FALSE(tokenId.empty());
    EXPECT_TRUE(validateHATToken(tokenId));
}

TEST_F(HATProtocolTest, EmptyPermissionString) {
    std::vector<std::string> permissions = {""};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    EXPECT_FALSE(tokenId.empty());
    EXPECT_TRUE(checkHATPermission(tokenId, ""));
    EXPECT_FALSE(checkHATPermission(tokenId, "read"));
}

TEST_F(HATProtocolTest, SpecialCharactersInIds) {
    std::vector<std::string> permissions = {"read"};
    std::string tokenId = issueHATToken("agent@#$%", "user!@#", permissions);
    
    EXPECT_FALSE(tokenId.empty());
    EXPECT_TRUE(validateHATToken(tokenId));
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
}

TEST_F(HATProtocolTest, VeryLongPermissionList) {
    std::vector<std::string> permissions;
    for (int i = 0; i < 1000; ++i) {
        permissions.push_back("permission" + std::to_string(i));
    }
    
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    EXPECT_FALSE(tokenId.empty());
    EXPECT_TRUE(validateHATToken(tokenId));
    
    // Check a few permissions
    EXPECT_TRUE(checkHATPermission(tokenId, "permission0"));
    EXPECT_TRUE(checkHATPermission(tokenId, "permission500"));
    EXPECT_TRUE(checkHATPermission(tokenId, "permission999"));
}

// ==============================================================================
// INTEGRATION TESTS
// ==============================================================================

TEST_F(HATProtocolTest, CompleteWorkflow) {
    // Issue token
    std::vector<std::string> permissions = {"read", "write", "execute"};
    std::string tokenId = issueHATToken("agent1", "user1", permissions);
    
    // Validate token
    EXPECT_TRUE(validateHATToken(tokenId));
    
    // Check permissions
    EXPECT_TRUE(checkHATPermission(tokenId, "read"));
    EXPECT_TRUE(checkHATPermission(tokenId, "write"));
    EXPECT_TRUE(checkHATPermission(tokenId, "execute"));
    EXPECT_FALSE(checkHATPermission(tokenId, "admin"));
    
    // Revoke token
    revokeHATToken(tokenId);
    
    // Verify revocation
    EXPECT_FALSE(validateHATToken(tokenId));
    EXPECT_FALSE(checkHATPermission(tokenId, "read"));
}

TEST_F(HATProtocolTest, MultiUserWorkflow) {
    // Issue tokens for multiple users
    std::vector<std::string> readPerms = {"read"};
    std::vector<std::string> writePerms = {"read", "write"};
    std::vector<std::string> adminPerms = {"read", "write", "admin"};
    
    std::string readerToken = issueHATToken("agent1", "reader", readPerms);
    std::string writerToken = issueHATToken("agent1", "writer", writePerms);
    std::string adminToken = issueHATToken("agent1", "admin", adminPerms);
    
    // Verify permissions
    EXPECT_TRUE(checkHATPermission(readerToken, "read"));
    EXPECT_FALSE(checkHATPermission(readerToken, "write"));
    
    EXPECT_TRUE(checkHATPermission(writerToken, "read"));
    EXPECT_TRUE(checkHATPermission(writerToken, "write"));
    EXPECT_FALSE(checkHATPermission(writerToken, "admin"));
    
    EXPECT_TRUE(checkHATPermission(adminToken, "read"));
    EXPECT_TRUE(checkHATPermission(adminToken, "write"));
    EXPECT_TRUE(checkHATPermission(adminToken, "admin"));
    
    // Revoke writer token
    revokeHATToken(writerToken);
    
    // Verify other tokens still valid
    EXPECT_TRUE(validateHATToken(readerToken));
    EXPECT_FALSE(validateHATToken(writerToken));
    EXPECT_TRUE(validateHATToken(adminToken));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return testing::RUN_ALL_TESTS();
}
