#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using HomepageSortBy = std::string;

struct IPagination {
    double page;
    double totalPages;
    double total;
    bool hasMore;
};

struct ISwap {
    double amountIn;
    double amountOut;
    int direction;
    std::string id;
    double price;
    std::variant<std::string, std::chrono::system_clock::time_point> timestamp;
    std::string tokenMint;
    std::string txId;
    std::string type;
    std::string user;
};

struct ITokenHolder {
    std::string id;
    std::string mint;
    std::string address;
    double amount;
    double percentage;
    std::variant<std::string, std::chrono::system_clock::time_point> lastUpdated;
};

using ChartTable = {

using IToken = z::infer<typeof TokenSchema>;

using ConfigAccount = {

// Type definitions for global objects

  struct Window {
    std::optional<bool> isPhantom;
    std::optional<PublicKey> publicKey;
};


} // namespace elizaos
