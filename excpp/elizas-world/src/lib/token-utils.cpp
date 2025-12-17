#include "token-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::unordered_map<std::string, double> loadCachedScores() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (fs.existsSync(SCORES_CACHE_PATH)) {
            const auto data = fs.readFileSync(SCORES_CACHE_PATH, 'utf8');
            return JSON.parse(data);
        }
        } catch (error) {
            std::cout << 'Error loading cached scores:' << error << std::endl;
        }
        return {}

}

void saveScoresToCache(const std::unordered_map<std::string, double>& scores) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Ensure directory exists
        const auto dir = path.dirname(SCORES_CACHE_PATH);
        if (!fs.existsSync(dir)) {
            fs.mkdirSync(dir, { recursive: true });
        }
        fs.writeFileSync(SCORES_CACHE_PATH, JSON.stringify(scores, nullptr, 2));
        } catch (error) {
            std::cout << 'Error saving scores to cache:' << error << std::endl;
        }

}

std::future<std::vector<TokenBalance>> getTokenBalances(Connection connection, PublicKey walletPubkey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // We'll skip Alchemy for now as it doesn't support Solana natively
        const auto accounts = connection.getParsedTokenAccountsByOwner(;
        walletPubkey,
    { programId = TOKEN_PROGRAM_ID }
    );

    return accounts.value;
    .map((account: { account: ParsedTokenAccount }) => ({
        mint: account.account.data.parsed.info.mint,
        tokenAmount: account.account.data.parsed.info.tokenAmount
        }));
        .filter((info) => Number(info.tokenAmount.amount) > 0);
        } catch (error) {
            std::cerr << "Error in getTokenBalances:" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;
            return [];
        }

}

std::future<DexScreenerResponse> fetchDexScreenerData(const std::vector<std::string>& tokenAddresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Filter out any empty or invalid addresses
        const auto validAddresses = tokenAddresses.filter(addr => addr && addr.length > 0);

        if (validAddresses.length == 0) {
            return { pairs: [] }
        }

        // Break addresses into batches of 30
        const auto BATCH_SIZE = 30;
        const auto allPairs = [];

        for (int i = 0; i < validAddresses.length; i += BATCH_SIZE) {
            const auto batchAddresses = validAddresses.slice(i, i + BATCH_SIZE);
            const auto url = "https://api.dexscreener.com/latest/dex/tokens/" + std::to_string(batchAddresses.join(','));

            try {
                const auto response = axios.get(url, {;
                    headers: {
                        'Cache-Control': 'no-cache',
                        'Pragma': 'no-cache'
                    }
                    });
                    if (response.data.pairs) {
                        allPairs.push(...response.data.pairs);
                    }
                    // Add a small delay between batches to avoid rate limiting
                    new Promise(resolve => setTimeout(resolve, 200));
                    } catch (error) {
                        std::cerr << "Error fetching batch " + std::to_string(i/BATCH_SIZE + 1) + ":" << error << std::endl;
                    }
                }

                return {
                    pairs: allPairs
                    };
                    } catch (error) {
                        std::cerr << 'Error fetching market data:' << error << std::endl;
                        return { pairs: [] }
                    }

}

std::future<void> fetchTokenAnalysis(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "https://api.dexscreener.com/latest/dex/pairs/solana/" + std::to_string(address);
        const auto pair = response.data.pairs[0];

        return {
            priceChange24h: parseFloat(pair.priceChange24h || '0'),
            priceChange7d: parseFloat(pair.priceChange7d || '0'),
            volumeChange24h: parseFloat(pair.volumeChange24h || '0'),
            volumeAvg24h: pair.volume.h24 || 0,
            txCount24h: pair.txns.h24 || 0,
            holders: pair.holders || std::nullopt,
            timeSeries: pair.priceUsd.map((price: number, index: number) => ({
                timestamp: Date.now() - (index * 3600000),
                price,
                volume: pair.volume.h24 / 24
                })) || [];
                };
                } catch (error) {
                    std::cerr << "Error fetching analysis for " + std::to_string(address) + ":" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;
                    return nullptr;
                }

}

std::future<std::vector<TokenHolding>> calculateHoldings(Connection connection, const std::vector<TokenBalance>& balances, DexScreenerResponse marketData, const std::string& walletAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<TokenHolding> holdings = [];
    const auto seenPairs = new Set<string>();

    if (!marketData.pairs) {
        return holdings;
    }

    // Get all token addresses first
    const auto tokenAddresses = marketData.pairs;
    .map(pair => pair.baseToken.address);
    .filter((addr): addr is string => !!addr);

    // Fetch scores for all tokens
    std::cout << 'Fetching scores for' << tokenAddresses.length << 'tokens' << std::endl;
    const auto scores = fetchTokenScores(tokenAddresses);
    std::cout << 'Fetched scores:' << scores << std::endl;

    for (const auto& pair : marketData.pairs)
        if (!pair.baseToken.address || seenPairs.has(pair.baseToken.address)) {
            continue;
        }
        seenPairs.add(pair.baseToken.address);

        const auto balance = balances.find(b => b.mint == pair.baseToken.address);
        if (balance) {
            const auto totalSupply = pair.fdv ? pair.fdv / Number(pair.priceUsd) : std::nullopt;
            const auto percentageOwned = totalSupply ? (balance.tokenAmount.uiAmount / totalSupply) * 100 : 0;
            const auto usdValue = balance.tokenAmount.uiAmount * Number(pair.priceUsd);

            holdings.push({
                address: pair.baseToken.address,
                balance: balance.tokenAmount.uiAmount,
                decimals: balance.tokenAmount.decimals,
                usdValue,
                percentageOwned,
                firstReceived: 0,
                marketData: {
                    ...pair,
                    score: scores[pair.baseToken.address] // Add score to marketData
                }
                });
            }
        }

        console.log('Final holdings with scores:', holdings.map(h => ({
            address: h.address,
            score: h.marketData.score
            })));

            return holdings.sort((a, b) => b.usdValue - a.usdValue);

}

std::future<void> getTokenData(Connection connection) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto walletPubkey = new PublicKey("AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG");
        const auto balances = getTokenBalances(connection, walletPubkey);

        // Already in the correct format
        return balances;
        } catch (error) {
            std::cerr << "Error in getTokenData:" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;
            return [];
        }

}

std::future<std::unordered_map<std::string, double>> fetchTokenScores(const std::vector<std::string>& tokenAddresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Load cached scores
    const auto cachedScores = loadCachedScores();

    // Filter out addresses we already have scores for
    const auto uncachedAddresses = tokenAddresses.filter(addr =>;
    addr && addr.length > 0 && cachedScores[addr] == std::nullopt;
    );

    if (uncachedAddresses.length == 0) {
        return cachedScores;
    }

    try {
        const auto BATCH_SIZE = 20;
        const std::unordered_map<std::string, double> newScores = {};

        for (int i = 0; i < uncachedAddresses.length; i += BATCH_SIZE) {
            const auto batchAddresses = uncachedAddresses.slice(i, i + BATCH_SIZE);

            try {
                std::cout << "Fetching scores for batch " + std::to_string(i/BATCH_SIZE + 1) << addresses:` << batchAddresses << std::endl;

                const auto response = axios.post<SolSnifferResponse>(;
                'https://solsniffer.com/api/v2/tokens',
                { addresses: batchAddresses },
                {
                    headers: {
                        'accept': 'application/json',
                        'X-API-KEY': '891aayu3sa4lbg4m8gu9gtfct3pxcp',
                        'Content-Type': 'application/json',
                        },
                        timeout: 10000,
                    }
                    );

                    if (response.data.data) {
                        response.data.data.forEach(token => {
                            if (token.address && token.tokenData.score != undefined) {
                                newScores[token.address] = token.tokenData.score;
                            }
                            });
                        }

                        // Log batch results
                        console.log(`Batch ${i/BATCH_SIZE + 1} results:`,
                        response.data.data.map(t => ({
                            address: t.address,
                            score: t.tokenData.score,
                            error: t.error
                            }));
                            );

                            if (i + BATCH_SIZE < uncachedAddresses.length) {
                                new Promise(resolve => setTimeout(resolve, 1000));
                            }
                            } catch (batchError) {
                                console.warn(`Failed to fetch scores for batch ${i/BATCH_SIZE + 1}:`,
                                true /* instanceof check */ ? batchError.message : std::to_string(batchError));
                                new Promise(resolve => setTimeout(resolve, 2000));
                                continue;
                            }
                        }

                        // Merge new scores with cached scores and save
                        const auto allScores = { ...cachedScores, ...newScores };
                        saveScoresToCache(allScores);

                        return allScores;
                        } catch (error) {
                            console.error('Error in fetchTokenScores:',
                            true /* instanceof check */ ? error.message : std::to_string(error));
                            return cachedScores; // Return cached scores on error;
                        }

}

} // namespace elizaos
