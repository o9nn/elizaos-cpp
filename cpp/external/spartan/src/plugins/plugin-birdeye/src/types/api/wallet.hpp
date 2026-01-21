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

// Wallet Portfolio Types
struct WalletPortfolioParams {
    std::string wallet;
};

struct WalletPortfolioResponse {
    bool success;
    { data;
    std::optional<std::string> wallet;
    std::optional<double> totalUsd;
    Array<{ items;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<double> decimals;
    std::optional<std::string> balance;
    std::optional<double> uiAmount;
    std::optional<std::string> chainId;
    std::optional<std::string> logoURI;
    std::optional<double> priceUsd;
    std::optional<double> valueUsd;
};

// Wallet Token Balance Types
struct WalletTokenBalanceParams {
    std::string wallet;
    std::string token_address;
};

struct WalletTokenBalanceResponse {
    bool success;
    { data;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<double> decimals;
    std::optional<double> balance;
    std::optional<double> uiAmount;
    std::optional<std::string> chainId;
    std::optional<double> priceUsd;
    std::optional<double> valueUsd;
};

// Wallet Transaction History Types
struct WalletTransactionHistoryParams {
    std::string wallet;
    std::optional<double> limit;
    std::optional<std::string> before;
};

struct WalletTransactionHistoryResponse {
    bool success;
    { data;
    std::optional<std::string> txHash;
    std::optional<double> blockNumber;
    std::optional<std::string> blockTime;
    std::optional<bool> status;
    std::optional<std::string> from;
    std::optional<std::string> to;
    std::optional<double> gasUsed;
    std::optional<double> gasPrice;
    std::optional<std::string> fee;
    std::optional<double> feeUsd;
    std::optional<std::string> value;
    std::optional<{> contractLabel;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<std::string> mainAction;
    std::optional<Array<{> balanceChange;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> logoURI;
    std::optional<std::string> address;
    std::optional<double> amount;
    std::optional<double> decimals;
};

// Wallet Networks Types
struct WalletNetworksResponse {
    bool success;
    { data;
    std::optional<std::vector<std::string>> chains;
};

// Wallet Portfolio Multichain Types
struct WalletPortfolioMultichainParams {
    std::string wallet;
};

struct WalletPortfolioMultichainResponse {
    bool success;
    { data;
    Array<{ items;
    std::optional<std::string> chain;
    std::optional<std::string> address;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> decimals;
    std::optional<double> price;
    std::optional<double> priceChange24h;
    std::optional<double> value;
    std::optional<double> amount;
    std::optional<double> total;
    std::optional<double> totalValue;
};

// Wallet Transaction History Multichain Types
struct WalletTransactionHistoryMultichainParams {
    std::string wallet;
};

struct WalletTransactionHistoryMultichainResponse {
    bool success;
    { data;
    std::optional<std::string> txHash;
    std::optional<double> blockNumber;
    std::optional<std::string> blockTime;
    std::optional<bool> status;
    std::optional<std::string> from;
    std::optional<std::string> to;
    std::optional<double> gasUsed;
    std::optional<double> gasPrice;
    std::optional<std::string> fee;
    std::optional<double> feeUsd;
    std::optional<std::string> value;
    std::optional<{> contractLabel;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<std::string> mainAction;
    std::optional<Array<{> balanceChange;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> logoURI;
    std::optional<std::string> address;
    std::optional<double> amount;
    std::optional<double> decimals;
};

// Wallet Transaction Simulation Types
struct WalletSimulationParams {
    std::optional<std::string> from;
    std::optional<std::string> to;
    std::optional<std::string> data;
    std::optional<std::string> value;
};

struct WalletSimulationResponse {
    bool success;
    { data;
    Array<{ balanceChange;
    std::optional<double> index;
    std::optional<double> before;
    std::optional<double> after;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<std::string> logoURI;
    std::optional<double> decimals;
    std::optional<double> gasUsed;
};


} // namespace elizaos
