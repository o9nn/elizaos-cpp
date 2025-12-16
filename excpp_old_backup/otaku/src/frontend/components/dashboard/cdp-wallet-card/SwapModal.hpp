#include "...constants/chains.hpp"
#include "...contexts/LoadingPanelContext.hpp"
#include "...contexts/ModalContext.hpp"
#include "...lib/elizaClient.hpp"
#include "...lib/number-format.hpp"
#include "..ui/button.hpp"
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



struct Token {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    std::optional<number | null> usdValue;
    std::optional<number | null> usdPrice;
    std::optional<string | null> contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
    std::optional<boolean; // Flag for CoinGecko tokens not in wallet> isExternal;
};

struct SwapModalContentProps {
    std::vector<Token> tokens;
    std::string userId;
    () => void onSuccess;
};

void SwapModalContent(auto { tokens, auto userId, SwapModalContentProps onSuccess });

} // namespace elizaos
