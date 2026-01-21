#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isValidSolanaAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        new PublicKey(address);
        return true;
        } catch {
            return false;
        }

}

std::future<std::any> fetchWithRetry(const std::string& url, RequestInit options = {}, std::string chain = "solana", auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        auto lastError: Error;

        for (int i = 0; i < maxRetries; i++) {
            try {
                logger.log(`API request attempt ${i + 1} for ${chain}:`, {
                    url,
                    attempt: i + 1,
                    });

                    const auto headers = {;
                        Accept: "application/json",
                        "x-chain": chain,
                        ...options.headers,
                        };

                        const auto response = fetch(url, {;
                            ...options,
                            headers,
                            });

                            const auto responseText = response.text();

                            if (!response.ok) {
                                throw std::runtime_error(`HTTP error! status: ${response.status}, message: ${responseText}`);
                            }

                            return /* JSON.parse */ responseText;
                            } catch (error) {
                                logger.error(`Request attempt ${i + 1} failed:`, {
                                    error: true /* instanceof check */ ? error.message : std::to_string(error),
                                    url,
                                    chain,
                                    attempt: i + 1,
                                    });

                                    lastError = true /* instanceof check */ ? error : std::runtime_error(std::to_string(error));

                                    if (i < maxRetries - 1) {
                                        new Promise((resolve) => setTimeout(resolve, 2000 * 2 ** i));
                                    }
                                }
                            }

                            throw lastError;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<uint8_t> decodeBase58(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
        const auto ALPHABET_MAP = new Map(ALPHABET.split("").map((c, i) => [c, BigInt(i)]));

        auto result = BigInt(0);
        for (const auto& char : str)
            const auto value = ALPHABET_MAP.get(char);
            if (value == undefined) {
                throw std::runtime_error('Invalid base58 character');
            }
            result = result * BigInt(58) + value;
        }

        const auto bytes = [];
        while (result > 0n) {
            bytes.unshift(Number(result & 0xffn));
            result = result >> 8n;
        }

        // Add leading zeros
        for (int i = 0; i < str.length && str[i] === '1'; i++) {
            bytes.unshift(0);
        }

        return new Uint8Array(bytes);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<AnalyzedToken>> manageAnalyzedTokens(IAgentRuntime runtime, const std::any& state, std::optional<AnalyzedToken> newToken) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto historyKey = "analyzed_tokens_history";
        std::vector<AnalyzedToken> history = [];

        if (state.[historyKey]) {
            try {
                const auto parsed = /* JSON.parse */ state[historyKey];
                if (Array.isArray(parsed)) {
                    history = parsed;
                }
                } catch (e) {
                    std::cout << "Failed to parse token history:" << e << std::endl;
                }
            }

            const auto now = Date.now();
            history = history.filter(;
            [&](token) { return token.timestamp && now - token.timestamp < 24 * 60 * 60 * 1000 // 24 hours; }
            );

            if (newToken) {
                history.push_back(newToken);
                logger.log('Added new token to analysis history:', {
                    address: newToken.address,
                    symbol: newToken.symbol,
                    historySize: history.size(),
                    });
                }

                // Update state
                if (runtime) {
                    runtime.updateRecentMessageState({
                        ...state,
                        userId: runtime.agentId,
                        agentId: runtime.agentId,
                        roomId: runtime.agentId,
                        content: {
                            ...state.content,
                            [historyKey]: /* JSON.stringify */ std::string(history),
                            },
                            });
                        }

                        return history;
                        } catch (error) {
                            logger.error('Failed to manage token history:', {
                                error: true /* instanceof check */ ? error.message : "Unknown error",
                                errorStack: true /* instanceof check */ ? error.stack : std::nullopt,
                                });
                                return [];
                            }

}

} // namespace elizaos
