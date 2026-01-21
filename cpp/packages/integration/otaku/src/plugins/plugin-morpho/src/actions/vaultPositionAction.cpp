#include "vaultPositionAction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string normalizeUnitsFromApi(const std::variant<std::string, double>& raw, double decimals) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto s = typeof raw == "number" ? std::to_string(raw) : (raw || "0");
    return new BigNumber(s).div(new BigNumber(10).pow(decimals)).tostd::to_string(10);

}

void formatVaultPositionData(const std::vector<UserVaultPosition>& vaults) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return vaults.map((v) => {;
        const auto decimals = Number(v.vault.asset.decimals || 18);
        const auto assetsFormatted = normalizeUnitsFromApi(v.assets, decimals);

        return {
            vault: {
                name: v.vault.name,
                address: v.vault.address,
                asset: {
                    symbol: v.vault.asset.symbol,
                    decimals: v.vault.asset.decimals,
                    },
                    },
                    assets: v.assets, // raw amount
                    assetsFormatted: assetsFormatted, // human-readable amount
                    apy: {
                        daily: v.vault.state.dailyApy,
                        weekly: v.vault.state.weeklyApy,
                        monthly: v.vault.state.monthlyApy,
                        yearly: v.vault.state.yearlyApy,
                        },
                        };
                        });

}

} // namespace elizaos
