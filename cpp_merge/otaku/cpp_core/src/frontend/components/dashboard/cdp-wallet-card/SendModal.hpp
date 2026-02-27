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
    std: symbol;
    std: name;
    std: balance;
    std: balanceFormatted;
    number | null usdValue;
    number | null usdPrice;
    std: | null contractAddress;
    std: chain;
    double decimals;
    std::optional<std:> icon;
};

struct SendModalContentProps {
    std::vector<Token> tokens;
    std: userId;
    [&]() { return void onSuccess; };
};

void SendModalContent(auto { tokens, auto userId, SendModalContentProps onSuccess });

} // namespace elizaos
