#include "fetchTokenData.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> fetchWalletTokens() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto connection = new Connection("https://api.mainnet-beta.solana.com", "confirmed");
        const auto walletPubkey = new PublicKey(TARGET_WALLET);

        try {
            std::cout << "Fetching token accounts..." << std::endl;
            const auto tokenAccounts = connection.getParsedTokenAccountsByOwner(walletPubkey, {;
                programId: TOKEN_PROGRAM_ID,
                });

                const auto tokenAddresses = tokenAccounts.value;
                .filter(account => {
                    const auto parsedAccount = account.account;
                    const auto balance = parsedAccount.data.parsed.info.tokenAmount.uiAmount;
                    return balance > 0;
                    });
                    .map(account => {
                        const auto parsedAccount = account.account;
                        return parsedAccount.data.parsed.info.mint;
                        });

                        std::cout << "Found " + tokenAddresses.size() + " tokens with non-zero balance" << std::endl;
                        return tokenAddresses;
                        } catch (error) {
                            std::cerr << "Error fetching wallet tokens:" << error << std::endl;
                            throw;
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<DexScreenerResponse> fetchDexScreenerData(const std::vector<std::string>& tokenAddresses) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto BATCH_SIZE = 30;
    const std::vector<DexScreenerResponse> batches = [];

    for (int i = 0; i < tokenAddresses.length; i += BATCH_SIZE) {
        const auto batch = tokenAddresses.slice(i, i + BATCH_SIZE);
        const auto url = "https://api.dexscreener.com/latest/dex/tokens/" + std::to_string(batch.join(","));

        try {
            const auto response = axios.get<DexScreenerResponse>(url);
            batches.push_back(response.data);
            new Promise(resolve => setTimeout(resolve, 200));
            } catch (error) {
                std::cerr << "Error fetching batch " + std::to_string(i/BATCH_SIZE + 1) + ":" << error << std::endl;
            }
        }

        return {
            schemaVersion: batches[0].schemaVersion || "",
            pairs: batches.flatMap(batch => batch.pairs || [])
            };

}

std::future<void> getTokenBalances(Connection connection, PublicKey walletPubkey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tokenAccounts = connection.getParsedTokenAccountsByOwner(walletPubkey, {;
        programId: TOKEN_PROGRAM_ID,
        });

        return tokenAccounts.value.map(account => {;
            const auto parsedAccount = account.account;
            return {
                mint: parsedAccount.data.parsed.info.mint,
                balance: parsedAccount.data.parsed.info.tokenAmount.uiAmount,
                decimals: parsedAccount.data.parsed.info.tokenAmount.decimals
                };
                });

}

std::future<std::optional<TokenAnalysis>> fetchTokenAnalysis(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto url = "https://api.dexscreener.com/latest/dex/pairs/solana/" + address;
        const auto response = axios.get(url);
        const auto pair = response.data.pair;

        if (!pair) return null;

        return {
            priceChange24h: pair.priceChange.h24 || 0,
            volumeChange24h: pair.volumeChange.h24 || 0,
            volumeAvg24h: pair.volume.h24 || 0,
            txCount24h: pair.txns.h24.total || 0,
            timeSeries: pair.priceUsd.map((price: number, index: number) => ({
                timestamp: Date.now() - (index * 3600000), // Approximate hourly data
                price,
                volume: pair.volume.h24 / 24 // Approximate hourly volume
                }));
                };
                } catch (error) {
                    std::cerr << "Error fetching analysis for " + address + ":" << error << std::endl;
                    return nullptr;
                }

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto connection = new Connection("https://api.mainnet-beta.solana.com", "confirmed");
        const auto walletPubkey = new PublicKey(TARGET_WALLET);

        // Get token balances
        std::cout << "Fetching token balances..." << std::endl;
        const auto balances = getTokenBalances(connection, walletPubkey);

        // Get market data
        std::cout << "Fetching market data..." << std::endl;
        const auto tokenAddresses = balances.map(b => b.mint);
        const auto marketData = fetchDexScreenerData(tokenAddresses);

        // Combine balance and market data
        const std::vector<TokenHolding> holdings = [];

        marketData.pairs.forEach(pair => {
            const auto balance = balances.find(b => b.mint == pair.baseToken.address);
            if (balance) {
                const auto totalSupply = pair.fdv ? pair.fdv / Number(pair.priceUsd) : std::nullopt;
                const auto percentageOwned = totalSupply ? (balance.balance / totalSupply) * 100 : 0;
                const auto usdValue = balance.balance * Number(pair.priceUsd);

                holdings.push_back({
                    address: pair.baseToken.address,
                    balance: balance.balance,
                    decimals: balance.decimals,
                    usdValue,
                    percentageOwned,
                    marketData: pair
                    });
                }
                });

                // Sort by USD value
                const auto sortedHoldings = holdings.sort((a, b) => b.usdValue - a.usdValue);

                std::cout << "\n== Significant Holdings Analysis (≥10% of supply) ==\n" << std::endl;

                for (const auto& holding : sortedHoldings.filter(h => h.percentageOwned >= 10)
                    const auto pair = holding.marketData;
                    const auto analysis = fetchTokenAnalysis(pair.pairAddress);

                    std::cout << "Token: " + pair.baseToken.name + " (" + pair.baseToken.symbol + ")" << std::endl;
                    std::cout << "Address: " + pair.baseToken.address << std::endl;
                    std::cout << "Balance: " + std::to_string(holding.balance.toLocaleString()) + " tokens" << std::endl;
                    std::cout << "USD Value: $" + std::to_string(holding.usdValue.toLocalestd::to_string(std::nullopt, {maximumFractionDigits: 2})) << std::endl;
                    std::cout << "Percentage Owned: " + std::to_string(holding.percentageOwned.toFixed(2)) + "%" << std::endl;

                    std::cout << "\nPrice Metrics:" << std::endl;
                    std::cout << "Current Price: $" + pair.priceUsd << std::endl;
                    if (analysis) {
                        std::cout << "24h Change: " + std::to_string(analysis.priceChange24h.toFixed(2)) + "%" << std::endl;
                        std::cout << "24h Volume: $" + std::to_string(analysis.volumeAvg24h.toLocaleString()) << std::endl;
                        std::cout << "24h Transactions: " + std::to_string(analysis.txCount24h.toLocaleString()) << std::endl;

                        // Volume analysis
                        const auto volumeToMcap = analysis.volumeAvg24h / (pair.marketCap || 1);
                        std::cout << "Volume/MCap Ratio: " + std::to_string((volumeToMcap * 100).toFixed(2)) + "%" << std::endl;
                    }

                    std::cout << "\nLiquidity Metrics:" << std::endl;
                    std::cout << "Market Cap: $" + std::to_string(pair.marketCap.toLocaleString() || "N/A") << std::endl;
                    std::cout << "FDV: $" + std::to_string(pair.fdv.toLocaleString() || "N/A") << std::endl;
                    std::cout << "Liquidity: $" + std::to_string(pair.liquidity.usd.toLocaleString() || "N/A") << std::endl;

                    // Calculate liquidity ratio
                    if (pair.marketCap && pair.liquidity.usd) {
                        const auto liquidityRatio = (pair.liquidity.usd / pair.marketCap) * 100;
                        std::cout << "Liquidity/MCap Ratio: " + std::to_string(liquidityRatio.toFixed(2)) + "%" << std::endl;
                    }

                    if (analysis.timeSeries && analysis.timeSeries.length > 0) {
                        std::cout << "\nRecent Price Action:" << std::endl;
                        const auto last24h = analysis.timeSeries.slice(0, 24).reverse();
                        const auto timeframes = ["1h", "4h", "12h", "24h"];

                        timeframes.forEach(tf => {
                            const auto hours = parseInt(tf);
                            const auto startPrice = last24h[0].price || 0;
                            const auto endPrice = last24h[hours - 1].price || 0;
                            const auto change = ((endPrice - startPrice) / startPrice) * 100;
                            std::cout << tf + " Change: " + std::to_string(change.toFixed(2)) + "%" << std::endl;
                            });
                        }

                        if (pair.info.socials) {
                            std::cout << "\nSocial Links:" << std::endl;
                            const auto socialMap = new Map<string, string>();

                            pair.info.socials.forEach(social => {
                                const auto platform = social.platform.toLowerCase();
                                if (platform == 'twitter' || platform == 'telegram' || platform == 'discord') {
                                    socialMap.set(platform, social.handle);
                                }
                                });

                                if (socialMap.has('twitter')) console.log(`Twitter: https://twitter.com/${socialMap.get('twitter')}`);
                                if (socialMap.has('telegram')) console.log(`Telegram: https://t.me/${socialMap.get('telegram')}`);
                                if (socialMap.has('discord')) console.log(`Discord: ${socialMap.get('discord')}`);
                            }

                            if (pair.info.websites && pair.info.websites.length > 0) {
                                std::cout << "Website: " + std::to_string(pair.info.websites[0].url) << std::endl;
                            }

                            // Add warning indicators
                            std::cout << "\nRisk Indicators:" << std::endl;
                            if (holding.percentageOwned > 25) {
                                std::cout << "⚠️ High ownership concentration (>25%)" << std::endl;
                            }
                            if (pair.liquidity.usd && pair.liquidity.usd < 50000) {
                                std::cout << "⚠️ Low liquidity (<$50k)" << std::endl;
                            }
                            if (analysis.volumeAvg24h && analysis.volumeAvg24h < 10000) {
                                std::cout << "⚠️ Low trading volume (<$10k/24h)" << std::endl;
                            }

                            std::cout << "\n-------------------\n" << std::endl;

                            // Add a small delay between API calls
                            new Promise(resolve => setTimeout(resolve, 200));
                        }

                        // Portfolio summary with more metrics
                        const auto totalValue = sortedHoldings.reduce((sum, h) => sum + h.usdValue, 0);
                        const auto significantHoldings = sortedHoldings.filter(h => h.percentageOwned >= 10);

                        std::cout << "\n== Portfolio Summary ==" << std::endl;
                        std::cout << "Total Holdings: " + sortedHoldings.size() + " tokens" << std::endl;
                        std::cout << "Total Portfolio Value: $" + std::to_string(totalValue.toLocaleString()) << std::endl;
                        std::cout << "Significant Positions (≥10%): " + significantHoldings.size() << std::endl;
                        std::cout << "Average Position Size: $" + std::to_string((totalValue / sortedHoldings.size()).toLocaleString()) << std::endl;

                        // Calculate concentration metrics
                        const auto topHoldingsValue = significantHoldings.reduce((sum, h) => sum + h.usdValue, 0);
                        std::cout << "Value in Significant Positions: $" + std::to_string(topHoldingsValue.toLocaleString()) + " (" + std::to_string(((topHoldingsValue/totalValue)*100).toFixed(2)) + "% of portfolio)" << std::endl;

                        } catch (error) {
                            std::cerr << "Script failed:" << error << std::endl;
                        }

}

} // namespace elizaos
