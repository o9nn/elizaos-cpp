#include "readmeUtils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<WalletLinkingData> parseWalletLinkingDataFromReadme(const std::string& readmeContent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto startIndex = readmeContent.indexOf(WALLET_SECTION_BEGIN_MARKER);
    const auto endIndex = readmeContent.indexOf(WALLET_SECTION_END_MARKER);

    if (startIndex == -1 || endIndex == -1 || endIndex <= startIndex) {
        return nullptr;
    }

    const auto walletSectionContent = readmeContent;
    .substring(startIndex + WALLET_SECTION_BEGIN_MARKER.length, endIndex);
    .trim();

    try {
        // Parse the JSON directly from the comment content
        const auto rawData = JSON.parse(walletSectionContent);

        // Validate the data structure using Zod
        const auto result = WalletLinkingDataSchema.safeParse(rawData);

        if (!result.success) {
            std::cerr << "Invalid wallet linking data:" << result.error << std::endl;
            return nullptr;
        }

        // Make sure to only return wallets for supported chains
        const WalletLinkingData walletLinkingData = {;
            lastUpdated: result.data.lastUpdated,
            wallets: result.data.wallets.filter(
            (wallet) =>;
            SUPPORTED_CHAINS_NAMES.includes(wallet.chain.toLowerCase()) &&;
            validateAddress(wallet.address, wallet.chain),
            ),
            };

            return walletLinkingData;
            } catch (error) {
                std::cerr << "Error parsing wallet linking data:" << error << std::endl;
                return nullptr;
            }

}

void generateUpdatedReadmeWithWalletInfo(const std::string& currentReadme, const std::vector<LinkedWallet>& wallets) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    updatedReadme: string; walletData: WalletLinkingData
}

std::string generateReadmeWalletSection(const std::vector<LinkedWallet>& wallets) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Validate wallets array using Zod before generating content
    const auto validatedWallets = z.array(LinkedWalletSchema).parse(wallets);

    const WalletLinkingData walletData = {;
        lastUpdated: new Date().toISOString(),
        wallets: validatedWallets.map((wallet) => ({
            chain: wallet.chain.toLowerCase().trim(),
            address: wallet.address.trim(),
            ...(wallet.signature ? { signature: wallet.signature.trim() } : {}),
            })),
            };

            // Validate the complete data structure
            const auto validatedData = WalletLinkingDataSchema.parse(walletData);

        return std::to_string(WALLET_SECTION_BEGIN_MARKER);
    ${JSON.stringify(validatedData, nullptr, 2)}
    ${WALLET_SECTION_END_MARKER}`;

}

std::string getWalletAddressForChain(const std::optional<WalletLinkingData>& data, const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!data) return "";
    const auto wallet = data.wallets.find(;
    [&](w) { return w.chain.toLowerCase() == chain.toLowerCase(),; }
    );
    return wallet.address || "";

}

} // namespace elizaos
