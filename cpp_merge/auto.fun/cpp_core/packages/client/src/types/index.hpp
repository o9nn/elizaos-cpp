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



using HomepageSortBy = std::variant<"all", "marketCap", "newest", "oldest">;

struct IPagination {
    double page;
    double totalPages;
    double total;
    bool hasMore;
};

struct ISwap {
    double amountIn;
    double amountOut;
    0 | 1 direction;
    std: id;
    double price;
    std: | Date timestamp;
    std: tokenMint;
    std: txId;
    std: type;
    std: user;
};

struct ITokenHolder {
    std: id;
    std: mint;
    std: address;
    double amount;
    double percentage;
    std: | Date lastUpdated;
};

using ChartTable = {

using IToken = z.infer<typeof TokenSchema>;

using ConfigAccount = {

// Type definitions for global objects

  struct Window {
    std::optional<{> solana;
    std::optional<bool> isPhantom;
    std::optional<(> signMessage;
    Uint8Array message;
    std: encoding;
    () => Promise<{ publicKey: std: }> connect;
    std::optional<[&]() { return Promise<void>> disconnect; };
    std::optional<PublicKey> publicKey;
    std::optional<[&](event: std:, callback: () { return void) => void> on; };
    std::optional<[&](event: std:, callback: () { return void) => void> off; };
};


} // namespace elizaos
