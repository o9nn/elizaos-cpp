#include "queries.hpp"
#include <string>

string Q_MARKETS = std::string("\
  query Markets($chainIds: [Int!], $first: Int!) {\
    markets(\
      first: $first\
      orderBy: SupplyAssetsUsd\
      orderDirection: Desc\
      where: { chainId_in: $chainIds, whitelisted: true }\
    ) {\
      items {\
        uniqueKey\
        lltv\
        loanAsset { address symbol decimals }\
        collateralAsset { address symbol decimals }\
        state { supplyAssetsUsd borrowAssetsUsd utilization liquidityAssetsUsd supplyApy borrowApy }\
      }\
    }\
  }\
");
string Q_VAULTS = std::string("\
  query Vaults($chainIds: [Int!], $first: Int!) {\
    vaults(\
      first: $first\
      orderBy: TotalAssetsUsd\
      orderDirection: Desc\
      where: { chainId_in: $chainIds, whitelisted: true }\
    ) {\
      items {\
        address\
        name\
        asset { address symbol decimals }\
        state {\
          totalAssets\
          totalAssetsUsd\
          totalSupply\
          apy\
          dailyApy\
          weeklyApy\
          monthlyApy\
          yearlyApy\
        }\
      }\
    }\
  }\
");
string Q_VAULT_BY_ADDRESS = std::string("\
  query OneVault($address: String!, $chainId: Int!) {\
    vaultByAddress(address: $address, chainId: $chainId) {\
      address\
      name\
      asset { address symbol decimals }\
      state {\
        totalAssets\
        totalAssetsUsd\
        totalSupply\
        apy\
        dailyApy\
        weeklyApy\
        monthlyApy\
        yearlyApy\
        allocation {\
          supplyCap\
          supplyAssets\
          supplyAssetsUsd\
          market { uniqueKey }\
        }\
      }\
    }\
  }\
");
string Q_USER_MARKET_POSITIONS = std::string("\
  query UserPositions($chainId: Int!, $address: String!) {\
    userByAddress(chainId: $chainId, address: $address) {\
      marketPositions {\
        market { uniqueKey }\
      }\
    }\
  }\
");
string Q_USER_VAULT_POSITIONS = std::string("\
  query UserVaultPositions($chainId: Int!, $address: String!) {\
    userByAddress(chainId: $chainId, address: $address) {\
      vaultPositions {\
        vault {\
          address\
          name\
          asset { address symbol decimals }\
          state { dailyApy weeklyApy monthlyApy yearlyApy }\
        }\
        shares\
        assets\
      }\
    }\
  }\
");
string Q_MARKET_SUMMARY = std::string("\
  query MarketSummary($uniqueKey: String!, $chainId: Int!) {\
    marketByUniqueKey(uniqueKey: $uniqueKey, chainId: $chainId) {\
      uniqueKey\
      lltv\
      loanAsset { address symbol decimals }\
      collateralAsset { address symbol decimals }\
      state {\
        utilization\
        supplyAssetsUsd\
        borrowAssetsUsd\
        liquidityAssetsUsd\
        supplyApy\
        borrowApy\
      }\
    }\
  }\
");

void Main(void)
{
}

MAIN
