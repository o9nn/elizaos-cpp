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
#include "constants/chains.hpp"
#include "contexts/LoadingPanelContext.hpp"
#include "contexts/ModalContext.hpp"
#include "lib/elizaClient.hpp"
#include "lib/number-format.hpp"
#include "ui/button.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Token {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    std::optional<std::optional<double>> usdValue;
    std::optional<std::optional<double>> usdPrice;
    std::optional<std::string> contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
    std::optional<bool> isExternal;
};

struct SwapModalContentProps {
    std::vector<Token> tokens;
    std::string userId;
};

void SwapModalContent(auto userId);

} // namespace elizaos
