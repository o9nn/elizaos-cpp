#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletBalance {
    std::string wallet_address;
    std::string wallet_id;
    double sol_balance;
    std::string tokens;
    std::string last_updated;
    std::variant<"success", "error", "pending"> fetch_status;
    std::optional<std::string> error_message;
    double retry_count;
};

struct TokenPrice {
    std::string mint;
    std::string symbol;
    std::string name;
    double price;
    std::optional<double> price_change_24h;
    std::optional<double> market_cap;
    std::optional<std::string> image_url;
    std::string last_updated;
    std::variant<"jupiter", "coingecko", "fallback", "jupiter-metadata"> source;
};

struct FetchLog {
    std::optional<double> id;
    std::string wallet_address;
    std::string timestamp;
    std::variant<"balance", "tokens", "prices", "token-metadata"> operation;
    std::variant<"success", "error", "timeout"> status;
    std::optional<std::string> error_details;
    double response_time_ms;
    std::optional<std::string> details;
};

struct SystemMetric {
    std::string metric_name;
    std::string metric_value;
    std::string last_updated;
};

struct PortfolioSnapshot {
    std::optional<double> id;
    std::string wallet_address;
    std::string snapshot_timestamp;
    double sol_balance;
    std::string tokens;
    double total_usd_value;
    std::variant<"hourly", "daily", "manual"> snapshot_type;
};

struct WalletPNL {
    std::string wallet_address;
    double initial_value_usd;
    double current_value_usd;
    double realized_pnl_usd;
    double unrealized_pnl_usd;
    double total_pnl_usd;
    double total_pnl_percentage;
    std::string first_snapshot_date;
    std::string last_updated;
};

struct TokenPNL {
    std::optional<double> id;
    std::string wallet_address;
    std::string token_mint;
    double initial_amount;
    double current_amount;
    double initial_price_usd;
    double current_price_usd;
    double realized_pnl_usd;
    double unrealized_pnl_usd;
    double total_pnl_usd;
    std::string last_updated;
};

class DatabaseService {
public:
    DatabaseService();
    static DatabaseService getInstance();
    void initializeDatabase();
    void initializeSystemMetrics();
    void upsertWalletBalance(WalletBalance balance);
    std::optional<WalletBalance> getWalletBalance(const std::string& address);
    std::vector<WalletBalance> getAllWalletBalances();
    std::vector<WalletBalance> getWalletBalancesByStatus(const std::string& status);
    void upsertTokenPrice(TokenPrice price);
    std::optional<TokenPrice> getTokenPrice(const std::string& mint);
    std::vector<TokenPrice> getAllTokenPrices();
    std::vector<TokenPrice> getStaleTokenPrices(double olderThanMinutes = 30);
    std::vector<FetchLog> getRecentFetchLogs(double limit = 100);
    std::vector<FetchLog> getFetchLogsByWallet(const std::string& walletAddress, double limit = 50);
    std::vector<FetchLog> getErrorLogs(double limit = 50);
    void setSystemMetric(const std::string& name, const std::string& value);
    std::optional<SystemMetric> getSystemMetric(const std::string& name);
    std::vector<SystemMetric> getAllSystemMetrics();
     getPortfolioOverview();
    Array< getTopTokensByValue(double limit = 20);
    void cleanup();
    std::vector<PortfolioSnapshot> getPortfolioSnapshots(const std::string& walletAddress, double limit = 100);
    std::optional<PortfolioSnapshot> getLatestPortfolioSnapshot(const std::string& walletAddress);
    void upsertWalletPNL(WalletPNL pnl);
    std::optional<WalletPNL> getWalletPNL(const std::string& walletAddress);
    std::vector<WalletPNL> getAllWalletPNL();
    std::optional<TokenPNL> getTokenPNL(const std::string& walletAddress, const std::string& tokenMint);
    std::vector<TokenPNL> getWalletTokenPNL(const std::string& walletAddress);
    std::vector<TokenPNL> getTokenPNLByMint(const std::string& tokenMint);
     getCombinedPNL();
    std::optional<WalletPNL> calculateWalletPNL(const std::string& walletAddress);
    double calculateCurrentWalletValue(const std::string& walletAddress);
    void close();
     healthCheck();

private:
    Database db_;
};

// Export singleton instance

} // namespace elizaos
