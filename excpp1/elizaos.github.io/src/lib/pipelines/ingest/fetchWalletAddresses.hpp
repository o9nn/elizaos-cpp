#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "..walletLinking/fetchWalletDataFromGithub.hpp"
#include ".getActiveContributors.hpp"
#include ".types.hpp"
#include "context.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletAddressIngestResult {
    std::string username;
    std::variant<"updated", "no-wallets", "failed"> status;
};


} // namespace elizaos
