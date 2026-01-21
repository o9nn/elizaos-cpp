#include "UnlinkAccounts.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void UnlinkAccounts() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState("");
        const auto { user } = usePrivy();
        const auto { unlinkFarcaster } = useUnlinkFarcaster({;
            onError: (err) => {
                std::cout << err << std::endl;
                setError(/* JSON.stringify */ std::string(err.message));
                },
                onSuccess: () => {
                    std::cout << "Unlink Farcaster success" << std::endl;
                    },
                    });
                    const auto oauth = useUnlinkOAuth({;
                        onError: (err) => {
                            std::cout << err << std::endl;
                            setError(/* JSON.stringify */ std::string(err.message));
                            },
                            });

                            return (;
                            <View>;
                        {/* Links Accounts */}
                        <Text>Unlink Accounts</Text>;

                        <View;
                        style={{
                            display: "flex",
                            flexDirection: "row",
                            margin: 10,
                            flexWrap: "wrap",
                        }}
                        >;
                        {(;
                        [;
                        "github",
                        "google",
                        "discord",
                        "apple",
                        "twitter",
                        "spotify",
                        "instagram",
                        "tiktok",
                        "linkedin",
                        "line",
                        ];
                        ).map((provider) => (;
                        <View key={provider}>;
                        <Button;
                    "title={" + "Unlink " + provider;
                    onPress={() =>;
                    oauth.unlinkOAuth({
                        provider,
                        subject: (
                        user.linked_accounts.find(;
                        "(account) => (account).type == " + provider + "_oauth";
                        );
                        ).subject,
                        });
                    }
                    disabled={
                        user.linked_accounts.find(;
                        "(account) => (account).type == " + provider + "_oauth";
                        ) == std::nullopt;
                    }
                    ></Button>;
                    </View>;
                ))}
                </View>;
                <Button;
                title="Unlink Farcaster";
                disabled={
                    user.linked_accounts.find(;
                    [&](account) { return (account).type == "farcaster"; }
                    ) == std::nullopt;
                }
                onPress={() => {
                    const auto farcasterAccount = user.linked_accounts.find(;
                    [&](account) { return (account).type == "farcaster"; }
                    ) | std::nullopt;
                    if (!farcasterAccount) return;
                    unlinkFarcaster({ fid: farcasterAccount.fid });
                }}
                />;
            {error && <Text style={{ color = "red" }}>Error = {error}</Text>}
            </View>;
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
