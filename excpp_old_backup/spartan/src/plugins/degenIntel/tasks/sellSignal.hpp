#include "..degenTrader/types.hpp"
#include "..degenTrader/utils/wallet.hpp"
#include "elizaos/core.hpp"
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



struct ISellSignalOutput {
    std::string recommended_sell;
    std::string recommend_sell_address;
    std::optional<double> marketcap;
    std::string reason;
    std::string sell_amount;
};

      // First refresh wallet data

      // Replace the cache lookup with direct wallet balance check

      // collect CA

      // Get token market data
      // FIXME: can we just get from the cache or the local birdeye functions?

      // Get all sentiments

      // Get sell recommendation from model

      // Validate token address format

      // Fetch marketcap data

        //console.log('birdeye resJson', resJson)

        // Continue without marketcap data rather than failing completely

      // Add logging before emitting

      // Emit sell signal event

      // Cache the signal

    // this.runtime.getSetting("BIRDEYE_API_KEY")


} // namespace elizaos
