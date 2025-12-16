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

const Q_MARKETS = `
  query Markets($chainIds: [Int!], $first: Int!) {
    markets(
      first: $first
      orderBy: SupplyAssetsUsd
      orderDirection: Desc
      where: { chainId_in: $chainIds, whitelisted: true }
    ) {
      items {
        uniqueKey
        lltv
        loanAsset { address symbol decimals }
        collateralAsset { address symbol decimals }
        state { supplyAssetsUsd borrowAssetsUsd utilization liquidityAssetsUsd supplyApy borrowApy }
      }
    }
  }
`;

const Q_VAULTS = `
  query Vaults($chainIds: [Int!], $first: Int!) {
    vaults(
      first: $first
      orderBy: TotalAssetsUsd
      orderDirection: Desc
      where: { chainId_in: $chainIds, whitelisted: true }
    ) {
      items {
        address
        name
        asset { address symbol decimals }
        state {
          totalAssets
          totalAssetsUsd
          totalSupply
          apy
          dailyApy
          weeklyApy
          monthlyApy
          yearlyApy
        }
      }
    }
  }
`;

const Q_VAULT_BY_ADDRESS = `
  query OneVault($address: String!, $chainId: Int!) {
    vaultByAddress(address: $address, chainId: $chainId) {
      address
      name
      asset { address symbol decimals }
      state {
        totalAssets
        totalAssetsUsd
        totalSupply
        apy
        dailyApy
        weeklyApy
        monthlyApy
        yearlyApy
        allocation {
          supplyCap
          supplyAssets
          supplyAssetsUsd
          market { uniqueKey }
        }
      }
    }
  }
`;

const Q_USER_MARKET_POSITIONS = `
  query UserPositions($chainId: Int!, $address: String!) {
    userByAddress(chainId: $chainId, address: $address) {
      marketPositions {
        market { uniqueKey }
      }
    }
  }
`;

const Q_USER_VAULT_POSITIONS = `
  query UserVaultPositions($chainId: Int!, $address: String!) {
    userByAddress(chainId: $chainId, address: $address) {
      vaultPositions {
        vault {
          address
          name
          asset { address symbol decimals }
          state { dailyApy weeklyApy monthlyApy yearlyApy }
        }
        shares
        assets
      }
    }
  }
`;

const Q_MARKET_SUMMARY = `
  query MarketSummary($uniqueKey: String!, $chainId: Int!) {
    marketByUniqueKey(uniqueKey: $uniqueKey, chainId: $chainId) {
      uniqueKey
      lltv
      loanAsset { address symbol decimals }
      collateralAsset { address symbol decimals }
      state {
        utilization
        supplyAssetsUsd
        borrowAssetsUsd
        liquidityAssetsUsd
        supplyApy
        borrowApy
      }
    }
  }
`;

} // namespace elizaos
