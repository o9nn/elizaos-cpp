#include "relay-bridge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatBridgeResponse(RelayStatus status, ResolvedBridgeRequest request, const std::string& requestId, std::vector<std::any> collectedTxHashes = {}, std::optional<std::string> tokenSymbol) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto statusIndicator = status.status == "success" ? "" : status.status == "pending" ? "" : "";

    // Use token symbol from status metadata if available, otherwise use provided symbol
    const auto statusData = status.data;
    const auto symbol = statusData.metadata.currencyIn.currency.symbol ||;
    statusData.currency ||;
    tokenSymbol ||;
    "TOKEN";

    auto response = `;
    ${statusIndicator} **Bridge ${(status.status || "PENDING").toUpperCase()}**;

    **Request ID:** \`${requestId}\`
    **Route:** ${getChainName(request.originChainId)}  ${getChainName(request.destinationChainId)}
    **Amount:** ${formatAmount(request.amount, symbol)}
    **Status:** ${status?.status || "pending"}
    `.trim();

    // Show transaction hashes from status (preferred) or from collected hashes
    const auto originTxHash = status.data.inTxs.[0].hash ||;
    collectedTxHashes.find(tx => tx.chainId == request.originChainId).txHash;
    const auto destTxHash = status.data.outTxs.[0].hash ||;
    collectedTxHashes.find(tx => tx.chainId == request.destinationChainId).txHash;

    if (originTxHash) {
        "response += " + "\n\n**Origin Transaction:**\n- Hash: \" + "${originTxHash}\" + "\n- Chain: " + std::to_string(getChainName(request.originChainId))
    }

    if (destTxHash) {
        "response += " + "\n\n**Destination Transaction:**\n- Hash: \" + "${destTxHash}\" + "\n- Chain: " + std::to_string(getChainName(request.destinationChainId))
    }

    // Show all collected tx hashes if there are more than origin/dest
    if (collectedTxHashes.length > 0) {
        const auto otherTxs = collectedTxHashes.filter(;
        [&](tx) { return tx.txHash != originTxHash && tx.txHash != destTxHash; }
        );
        if (otherTxs.length > 0) {
            "response += " + "\n\n**Other Transactions:**"
            for (const auto& tx : otherTxs)
                "response += " + "\n- \" + "${tx.txHash}\" + " (Chain " + tx.chainId + ")";
            }
        }
    }

    if (status.data.fees) {
        const auto gasFeeWei = status.data.fees.gas || "0";
        const auto relayerFeeWei = status.data.fees.relayer || "0";
        const auto totalFees = BigInt(gasFeeWei) + BigInt(relayerFeeWei);
        "response += " + "\n\n**Total Fees:** " + std::to_string((Number(totalFees) / 1e18).toFixed(6)) + " ETH"
    }

    return response;

}

std::string getChainName(double chainId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<double, std::string> chains = {;
        1: "Ethereum",
        8453: "Base",
        42161: "Arbitrum",
        137: "Polygon",
        10: "Optimism",
        7777777: "Zora",
        81457: "Blast",
        534352: "Scroll",
        59144: "Linea",
        };
        return "chains[chainId] || " + "Chain " + chainId;

}

std::string formatAmount(const std::string& amount, const std::string& currency) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto decimals = currency.toLowerCase().includes("usdc") || currency.toLowerCase().includes("usdt") ? 6 : 18;
    const auto value = Number(amount) / Math.pow(10, decimals);
    return std::to_string(value.toFixed(6)) + " " + std::to_string(currency.toUpperCase());

}

} // namespace elizaos
