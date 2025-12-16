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

import type { BIRDEYE_SUPPORTED_CHAINS } from "../utils";

// Types
using BirdeyeSupportedChain = (typeof BIRDEYE_SUPPORTED_CHAINS)[number];

struct BaseAddress {
    std::optional<"wallet" | "token" | "contract"> type;
    std::optional<std::string> symbol;
    std::string address;
    BirdeyeSupportedChain chain;
};


interface WalletAddress extends BaseAddress {
    type: "wallet";
}

interface TokenAddress extends BaseAddress {
    type: "token";
}

interface ContractAddress extends BaseAddress {
    type: "contract";
}

} // namespace elizaos
