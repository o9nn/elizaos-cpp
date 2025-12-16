#include "...constants/chains.hpp"
#include "...contexts/LoadingPanelContext.hpp"
#include "...contexts/ModalContext.hpp"
#include "...lib/elizaClient.hpp"
#include "...lib/number-format.hpp"
#include "..ui/button.hpp"
#include "..ui/input.hpp"
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
    number | null usdValue;
    number | null usdPrice;
    string | null contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
};

struct SendModalContentProps {
    std::vector<Token> tokens;
    std::string userId;
    () => void onSuccess;
};

void SendModalContent(auto { tokens, auto userId, SendModalContentProps onSuccess });

} // namespace elizaos
