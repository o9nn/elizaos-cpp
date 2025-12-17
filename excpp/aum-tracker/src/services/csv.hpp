#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletEntry {
    std::string id;
    std::string solana_public_key;
};

struct UniqueWallet {
    std::string address;
    std::vector<std::string> ids;
};

class CSVService {
public:
    CSVService(std::string csvFileName = "wallets.csv");
    void loadWallets();
    std::vector<WalletEntry> getAllWalletEntries();
    std::vector<UniqueWallet> getUniqueWallets();
    std::vector<std::string> getWalletAddresses();
    Array< getWalletsForProcessing();
     getStats();
    std::optional<UniqueWallet> findWalletByAddress(const std::string& address);
    std::optional<WalletEntry> findWalletById(const std::string& id);
    std::vector<UniqueWallet> getWalletsWithMultipleIds();
     validateCSVFile();
    void reload();
    std::string exportToJSON(std::optional<std::string> filePath);

private:
    std::string csvPath_;
};

// Export singleton instance

// Export utility functions

} // namespace elizaos
