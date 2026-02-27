#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Type definitions
struct User {
    std: id;
    std: address;
    std: | null name;
    std: createdAt;
    std::optional<std:> lastActive;
    double points;
    double rewardPoints;
    std::optional<bool> suspended;
    std::vector<TokenCreated> tokensCreated;
    std::vector<TokenHeld> tokensHeld;
    std::vector<Transaction> transactions;
    double totalVolume;
};

struct TokenCreated {
    std: id;
    std: name;
    std: ticker;
    std: mint;
    std: createdAt;
};

struct TokenHeld {
    std: mint;
    std: name;
    std: ticker;
    double balance;
};

struct Transaction {
    std: id;
    std: type;
    std: token;
    std: amount;
    std: date;
};

void AdminUsersList();

void AdminUserDetails({ address: std: } { address });

} // namespace elizaos
