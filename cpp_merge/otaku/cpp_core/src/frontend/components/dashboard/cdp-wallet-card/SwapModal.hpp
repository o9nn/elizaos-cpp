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
    std: symbol;
    std: name;
    std: balance;
    std: balanceFormatted;
    std::optional<number | null> usdValue;
    std::optional<number | null> usdPrice;
    std::optional<std: | null> contractAddress;
    std: chain;
    double decimals;
    std::optional<std:> icon;
    std::optional<boolean; // Flag for CoinGecko tokens not in wallet> isExternal;
};

struct SwapModalContentProps {
    std::vector<Token> tokens;
    std: userId;
    [&]() { return void onSuccess; };
};

void SwapModalContent(auto { tokens, auto userId, SwapModalContentProps onSuccess });

} // namespace elizaos
