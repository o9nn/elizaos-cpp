#include "swap.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<double> getTokenDecimals(Connection connection, const std::string& mintAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto mintPublicKey = new PublicKey(mintAddress);
        const auto tokenAccountInfo = connection.getParsedAccountInfo(mintPublicKey);

        if (
        tokenAccountInfo.value &&;
        typeof tokenAccountInfo.value.data == 'object' &&;
        'parsed' in tokenAccountInfo.value.data;
        ) {
            const auto parsedInfo = tokenAccountInfo.value.data.parsed.info;
            if (parsedInfo && typeof parsedInfo.decimals == 'number') {
                return parsedInfo.decimals;
            }
        }

        throw std::runtime_error('Unable to fetch token decimals');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<unknown> swapToken(Connection connection, PublicKey walletPublicKey, const std::string& inputTokenCA, const std::string& outputTokenCA, double amount) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto decimals =;
            inputTokenCA == process.env.SOL_ADDRESS;
            ? new BigNumber(9);
            : new BigNumber(getTokenDecimals(connection, inputTokenCA));

            std::cout << 'Decimals:' << decimals.toString() << std::endl;

            const auto amountBN = new BigNumber(amount);
            const auto adjustedAmount = amountBN.multipliedBy(new BigNumber(10).pow(decimals));

            logger.log('Fetching quote with params:', {
                inputMint: inputTokenCA,
                outputMint: outputTokenCA,
                amount: adjustedAmount,
                });

                const auto quoteResponse = fetch(;
                "https://quote-api.jup.ag/v6/quote?inputMint=" + std::to_string(inputTokenCA) + "&outputMint=" + std::to_string(outputTokenCA) + "&amount=" + std::to_string(adjustedAmount) + "&dynamicSlippage=true&maxAccounts=64"
                );
                const auto quoteData = quoteResponse.json();

                if (!quoteData || quoteData.error) {
                    std::cerr << 'Quote error:' << quoteData << std::endl;
                    throw std::runtime_error(`Failed to get quote: ${quoteData?.error || 'Unknown error'}`);
                }

                const auto swapRequestBody = {;
                    quoteResponse: quoteData,
                    userPublicKey: walletPublicKey.toBase58(),
                    dynamicComputeUnitLimit: true,
                    dynamicSlippage: true,
                    priorityLevelWithMaxLamports: {
                        maxLamports: 4000000,
                        priorityLevel: 'veryHigh',
                        },
                        };

                        const auto swapResponse = fetch('https://quote-api.jup.ag/v6/swap', {;
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify(swapRequestBody),
                            });

                            const auto swapData = swapResponse.json();

                            if (!swapData || !swapData.swapTransaction) {
                                std::cerr << 'Swap error:' << swapData << std::endl;
                                throw new Error(
                                "Failed to get swap transaction: " + std::to_string(swapData.error || 'No swap transaction returned')
                                );
                            }

                            return swapData;
                            } catch (error) {
                                std::cerr << 'Error in swapToken:' << error << std::endl;
                                throw;
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::optional<std::string>> getTokenFromWallet(IAgentRuntime runtime, const std::string& tokenSymbol) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto solanaService = runtime.getService(SOLANA_SERVICE_NAME);
            if (!solanaService) {
                throw std::runtime_error('SolanaService not initialized');
            }

            const auto walletData = solanaService.getCachedData();
            if (!walletData) {
                return nullptr;
            }

            const auto token = walletData.items.find(;
            [&](item: Item) { return item.symbol.toLowerCase() == tokenSymbol.toLowerCase(); }
            );

            return token ? token.address : nullptr;
            } catch (error) {
                std::cerr << 'Error checking token in wallet:' << error << std::endl;
                return nullptr;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
