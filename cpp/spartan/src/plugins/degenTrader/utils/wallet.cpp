#include "wallet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Keypair getWalletKeypair(std::optional<IAgentRuntime> runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto privateKeyString = runtime.getSetting("SOLANA_PRIVATE_KEY");
        if (!privateKeyString) {
            throw std::runtime_error('No wallet private key configured');
        }

        try {
            const auto privateKeyBytes = decodeBase58(privateKeyString);
            return Keypair.fromSecretKey(privateKeyBytes);
            } catch (error) {
                std::cerr << "Failed to create wallet keypair:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<double> getWalletBalance(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto walletKeypair = getWalletKeypair(runtime);
        const auto connection = new Connection(runtime.getSetting("RPC_URL"));
        const auto balance = connection.getBalance(walletKeypair.publicKey);
        const auto solBalance = balance / 1e9;

        logger.log('Fetched wallet balance:', {
            address: walletKeypair.publicKey.toBase58(),
            solBalance,
            });

            return solBalance;
            } catch (error) {
                std::cerr << "Failed to get wallet balance:" << error << std::endl;
                return 0;
            }

}

std::future<Connection> getConnection(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Connection(;
    runtime.getSetting("RPC_URL") || "https://zondra-wil7oz-fast-mainnet.helius-rpc.com"
    );

}

double calculateDynamicSlippage(const std::string& amount, const std::any& quoteData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto baseSlippage = 0.45;
    const auto priceImpact = Number.parseFloat(quoteData.priceImpactPct || "0");
    const auto amountNum = Number(amount);

    auto dynamicSlippage = baseSlippage;

    if (priceImpact > 1) {
        dynamicSlippage += priceImpact * 0.5;
    }

    if (amountNum > 10000) {
        dynamicSlippage *= 1.5;
    }

    return Math.min(dynamicSlippage, 2.5);

}

std::future<> executeTrade(IAgentRuntime runtime, const std::any& params) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    success: boolean;
    signature?: string;
    error?: string;
    receivedAmount?: string;
    receivedValue?: string;

}

void extractTokenAddress(auto message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto regex = /The token (\w{44}) is not tradable/;
    const auto match = message.match(regex);
    return match ? match[1] : nullptr;

}

std::future<> executeRaydiumTrade(IAgentRuntime runtime, std::optional<std::any> params) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    success: boolean; signature?: string; error?: string
}

std::future<double> getChainWalletBalance(IAgentRuntime runtime, const std::string& _tokenAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get Solana balance
    return getWalletBalance(runtime);

}

std::future<std::string> simulateTransaction(const std::any& client, const std::any& tx) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto result = client.call({;
            account: client.account,
            to: tx.to,
            data: tx.data,
            value: tx.value,
            gas: tx.gas,
            gasPrice: tx.gasPrice,
            });
            return result;
            } catch (error) {
                return "Simulation failed: " + error.message;
            }

}

std::future<void> getWalletBalances(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto walletKeypair = getWalletKeypair(runtime);
        const auto connection = new Connection(runtime.getSetting("RPC_URL"));

        const auto solBalance = connection.getBalance(walletKeypair.publicKey);
        const auto tokenAccounts = connection.getParsedTokenAccountsByOwner(walletKeypair.publicKey, {;
            programId: new PublicKey("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"),
            });

            const auto balances = {;
                solBalance: solBalance / 1e9,
                tokens: tokenAccounts.value.map((account) => ({
                    mint: account.account.data.parsed.info.mint,
                    balance: account.account.data.parsed.info.tokenAmount.amount,
                    decimals: account.account.data.parsed.info.tokenAmount.decimals,
                    uiAmount: account.account.data.parsed.info.tokenAmount.uiAmount,
                    })),
                    };

                    //logger.log('Fetched wallet balances:', balances);
                    return balances;
                    } catch (error) {
                        std::cerr << "Failed to get wallet balances:" << error << std::endl;
                        return {
                            solBalance: 0,
                            tokens: [],
                            };
                        }

}

std::future<std::optional<TokenBalance>> getTokenBalance(IAgentRuntime runtime, const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto balances = getWalletBalances(runtime);
        const auto token = balances.tokens.find((t) => t.mint.toLowerCase() == tokenMint.toLowerCase());

        if (!token) {
            logger.warn(`No balance found for token ${tokenMint}`, {
                availableTokens: balances.tokens.map((t) => t.mint),
                });
            }

            return token;
            } catch (error) {
                std::cerr << "Failed to get token balance:" << error << std::endl;
                return nullptr;
            }

}

std::future<bool> hasTokenBalance(IAgentRuntime runtime, const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto balance = getTokenBalance(runtime, tokenMint);
    return balance != nullptr && balance.uiAmount > 0;

}

} // namespace elizaos
