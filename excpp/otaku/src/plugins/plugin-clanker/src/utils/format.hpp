#include ".types.hpp"
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

;

).format(amount);
}

%`;
}

...${address.slice(-4)}`;
}





 (${info.symbol})`];

  // Only show address for non-native tokens
  const isNativeEth = info.address === NATIVE_TOKEN_ADDRESSES;
  if (!isNativeEth) {
    lines.push(`Address: ${info.address}`);
  } else {
    lines.push(`Type: Native ETH on Base`);
  }

  if (info.price !== undefined) {
    lines.push(`Price: ${formatUsd(info.price)}`);
  }

  if (info.marketCap !== undefined) {
    lines.push(`Market Cap: ${formatCompactUsd(Number(info.marketCap))}`);
  }

  if (info.liquidity !== undefined) {
    lines.push(`Liquidity: ${formatCompactUsd(Number(info.liquidity))}`);
  }

  if (info.holders !== undefined) {
    lines.push(`Holders: ${info.holders.toLocaleString()}`);
  }

  if (info.volume24h !== undefined) {
    lines.push(`24h Volume: ${formatCompactUsd(Number(info.volume24h))}`);
  }

  return lines.join("\n");
}

 gwei`;
}

).format(amount);
}

} // namespace elizaos
