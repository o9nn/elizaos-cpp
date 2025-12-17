#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Type definitions
struct User {
    std::string id;
    std::string address;
    std::string name;
    std::string createdAt;
    std::optional<std::string> lastActive;
    double points;
    double rewardPoints;
    std::optional<bool> suspended;
    std::vector<TokenCreated> tokensCreated;
    std::vector<TokenHeld> tokensHeld;
    std::vector<Transaction> transactions;
    double totalVolume;
};

struct TokenCreated {
    std::string id;
    std::string name;
    std::string ticker;
    std::string mint;
    std::string createdAt;
};

struct TokenHeld {
    std::string mint;
    std::string name;
    std::string ticker;
    double balance;
};

struct Transaction {
    std::string id;
    std::string type;
    std::string token;
    std::string amount;
    std::string date;
};

void AdminUsersList();

void AdminUserDetails();

} // namespace elizaos
