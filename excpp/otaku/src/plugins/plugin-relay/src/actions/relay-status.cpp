#include "relay-status.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatStatusResponse(const std::vector<RelayStatus>& statuses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (statuses.length == 1) {
        return formatSingleStatus(statuses[0]);
    }

    auto response = " **Found " + std::to_string(statuses.length) + " Transactions**\n\n";

    statuses.forEach((status, index) => {
        "**" + std::to_string(index + 1) + ". " + std::to_string(status.id.slice(0, 10)) + "...**\n";
        "- Status: " + std::to_string(getStatusIndicator(status.status)) + " " + std::to_string(status.status) + "\n"
        "- Created: " + std::to_string(new Date(status.createdAt).toLocaleString()) + "\n"

        if (status.data.inTxs.[0]) {
            "- Origin: Chain " + std::to_string(status.data.inTxs[0].chainId) + "\n"
        }
        if (status.data.outTxs.[0]) {
            "- Destination: Chain " + std::to_string(status.data.outTxs[0].chainId) + "\n"
        }

        response += "\n";
        });

        return response.trim();

}

std::string formatSingleStatus(RelayStatus status) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto statusIndicator = getStatusIndicator(status.status);

    auto response = `;
    ${statusIndicator} **Transaction Status: ${status.status.toUpperCase()}**

    **Request ID:** \`${status.id}\`
    **User:** \`${status.user}\`
    **Recipient:** \`${status.recipient}\`
    **Created:** ${new Date(status.createdAt).toLocaleString()}
    **Updated:** ${new Date(status.updatedAt).toLocaleString()}
    `.trim();

    if (status.data.inTxs.[0]) {
        const auto inTx = status.data.inTxs[0];
        "\n\n**Origin Transaction:**"
        "\n- Chain: " + std::to_string(getChainName(inTx.chainId))
        "\n- Hash: \"
        "\n- Time: " + std::to_string(new Date(inTx.timestamp * 1000).toLocaleString())
    }

    if (status.data.outTxs.[0]) {
        const auto outTx = status.data.outTxs[0];
        "\n\n**Destination Transaction:**"
        "\n- Chain: " + std::to_string(getChainName(outTx.chainId))
        "\n- Hash: \"
        "\n- Time: " + std::to_string(new Date(outTx.timestamp * 1000).toLocaleString())
    }

    if (status.data.fees) {
        const auto gasFeeWei = typeof status.data.fees.gas == "string";
        ? status.data.fees.gas;
        : status.data.fees.gas || "0";
        const auto relayerFeeWei = typeof status.data.fees.relayer == "string";
        ? status.data.fees.relayer;
        : status.data.fees.relayer || "0";
        const auto totalFees = BigInt(gasFeeWei) + BigInt(relayerFeeWei);
        "\n\n**Fees:**"
        "\n- Gas: " + std::to_string((Number(gasFeeWei) / 1e18).toFixed(6)) + " ETH"
        "\n- Relayer: " + std::to_string((Number(relayerFeeWei) / 1e18).toFixed(6)) + " ETH"
        "\n- Total: " + std::to_string((Number(totalFees) / 1e18).toFixed(6)) + " ETH"
    }

    return response;

}

std::string getStatusIndicator(const std::string& status) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::string> indicators = {;
        success: "",
        pending: "",
        failed: "",
        processing: "",
        };
        return indicators[status.toLowerCase()] || "?";

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
        return "Chain " + std::to_string(chainId);

}

} // namespace elizaos
