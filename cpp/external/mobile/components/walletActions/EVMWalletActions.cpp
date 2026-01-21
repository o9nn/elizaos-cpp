#include "EVMWalletActions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void EVMWalletActions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [result, setResult] = useState<string | nullptr>(nullptr);
    const auto { wallets } = useEmbeddedEthereumWallet();
    const auto wallet = wallets.[0];

    const auto signMessage = async () => {;
        const auto provider = wallet.getProvider.();
        if (!provider) return;

        const auto message = "Hello World";
        const auto signature = provider.request({;
            method: "personal_sign",
            params: [message, wallet.address],
            });
            setResult(signature);
            };
            const auto signTransaction = async () => {;
                const auto provider = wallet.getProvider.();
                if (!provider) return;

                // Sign transaction (will be signed and populated)
                try {
                    const auto { signature } = provider.request({;
                        method: "eth_signTransaction",
                        params: [
                        {
                            from: wallet.address,
                            to: "0x0000000000000000000000000000000000000000",
                            value: "1",
                            },
                            ],
                            });
                            setResult(signature);
                            } catch (error) {
                                setResult(/* JSON.stringify */ std::string(error));
                            }
                            };
                            const auto signAndSendTransaction = async () => {;
                                const auto provider = wallet.getProvider.();
                                if (!provider) return;
                                const auto response = provider.request({;
                                    method: "eth_sendTransaction",
                                    params: [
                                    {
                                        from: wallet.address,
                                        to: "0x0000000000000000000000000000000000000000",
                                        value: "1",
                                        },
                                        ],
                                        });
                                        setResult(/* JSON.stringify */ std::string(response));
                                        };
                                        return (;
                                        <View>;
                                        <Text>EVM Wallet Actions</Text>;
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
