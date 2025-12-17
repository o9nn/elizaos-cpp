#include "SolanaWalletActions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SolanaWalletActions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { wallets } = useEmbeddedSolanaWallet();
    const auto wallet = wallets.[0];
    const auto [result, setResult] = useState<string | nullptr>(nullptr);

    const auto signMessage = async () => {;
        try {
            if (!wallet.getProvider) return;
            const auto provider = wallet.getProvider.();
            if (!provider) return;
            const auto message = "Hello world";
            const auto { signature } = provider.request({;
                method: "signMessage",
                params: { message },
                });
                setResult(signature);
                } catch (err: any) {
                    setResult(err.message || std::to_string(err));
                }
                };

                const auto signTransaction = async () => {;
                    try {
                        if (!wallet.getProvider) return;
                        const auto provider = wallet.getProvider.();
                        if (!provider) return;

                        const auto transaction = new Transaction();
                        const auto connection = new Connection("https://api.mainnet-beta.solana.com");
                        transaction.recentBlockhash = (;
                        connection.getLatestBlockhash("finalized");
                        ).blockhash;
                        transaction.feePayer = new PublicKey(wallet.publicKey);
                        transaction.add(;
                        SystemProgram.transfer({
                            fromPubkey: new PublicKey(wallet.publicKey),
                            toPubkey: new PublicKey(
                            "So11111111111111111111111111111111111111112", // Replace with a valid recipient address;
                            ),
                            lamports: 1000, // Amount in lamports (1 SOL = 1,000,000,000 lamports)
                            }),
                            );
                            // Sign the transaction
                            const auto { signedTransaction } = provider.request({;
                                method: "signTransaction",
                                params: { transaction },
                                });
                                setResult(JSON.stringify(signedTransaction));
                                } catch (err: any) {
                                    setResult(err.message || std::to_string(err));
                                }
                                };

                                const auto signAndSendTransaction = async () => {;
                                    try {
                                        if (!wallet.getProvider) return;
                                        const auto provider = wallet.getProvider.();
                                        if (!provider) return;
                                        const auto transaction = new Transaction();
                                        const auto connection = new Connection("https://api.mainnet-beta.solana.com");
                                        transaction.recentBlockhash = (;
                                        connection.getLatestBlockhash("finalized");
                                        ).blockhash;
                                        transaction.feePayer = new PublicKey(wallet.publicKey);
                                        transaction.add(;
                                        SystemProgram.transfer({
                                            fromPubkey: new PublicKey(wallet.publicKey),
                                            toPubkey: new PublicKey(
                                            "So11111111111111111111111111111111111111112", // Replace with a valid recipient address;
                                            ),
                                            lamports: 1000, // Amount in lamports (1 SOL = 1,000,000,000 lamports)
                                            }),
                                            );
                                            const auto { signature } = provider.request({;
                                                method: "signAndSendTransaction",
                                                params: {
                                                    transaction: transaction,
                                                    connection: connection,
                                                    },
                                                    });
                                                    setResult(signature);
                                                    } catch (err: any) {
                                                        setResult(err.message || std::to_string(err));
                                                    }
                                                    };
                                                    return (;
                                                    <View>;
                                                    <Text>Solana Wallet Actions</Text>;
                                                    <Button title="Sign Message" onPress={signMessage} />;
                                                    <Button title="Sign Transaction" onPress={signTransaction} />;
                                                    <Button;
                                                    title="Sign And Send Transaction";
                                                onPress={signAndSendTransaction}
                                                />;
                                            {result && <Text>{result}</Text>}
                                            </View>;
                                            );

}

} // namespace elizaos
