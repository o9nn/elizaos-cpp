#include "LinkAccounts.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void LinkAccounts() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState("");
        const auto [email, setEmail] = useState("");
        const auto [phone, setPhone] = useState("");
        const auto [emailCode, setEmailCode] = useState("");
        const auto [smsCode, setSmsCode] = useState("");
        const auto { linkWithFarcaster } = useLinkWithFarcaster({;
            onSuccess: () => {
                std::cout << "Link Farcaster success" << std::endl;
                },
                onError: (err) => {
                    std::cout << err << std::endl;
                    setError(JSON.stringify(err.message));
                    },
                    });
                    const auto { sendCode: sendCodeEmail, linkWithCode: linkWithCodeEmail } =;
                    useLinkEmail({
                        onError: (err) => {
                            std::cout << err << std::endl;
                            setError(JSON.stringify(err.message));
                            },
                            onLinkSuccess: () => {
                                std::cout << "Link Email success" << std::endl;
                                },
                                onSendCodeSuccess: () => {
                                    std::cout << "Link Email success" << std::endl;
                                    },
                                    });
                                    const auto { sendCode: sendCodeSMS, linkWithCode: linkWithCodeSMS } = useLinkSMS({;
                                        onError: (err) => {
                                            std::cout << err << std::endl;
                                            setError(JSON.stringify(err.message));
                                            },
                                            onLinkSuccess: () => {
                                                std::cout << "Link SMS success" << std::endl;
                                                },
                                                onSendCodeSuccess: () => {
                                                    std::cout << "Link SMS success" << std::endl;
                                                    },
                                                    });

                                                    const auto { linkWithPasskey } = useLinkWithPasskey({;
                                                        onError: (err) => {
                                                            std::cout << err << std::endl;
                                                            setError(JSON.stringify(err.message));
                                                            },
                                                            });
                                                            const auto oauth = useLinkWithOAuth({;
                                                                onError: (err) => {
                                                                    std::cout << err << std::endl;
                                                                    setError(JSON.stringify(err.message));
                                                                    },
                                                                    });
                                                                    return (;
                                                                    <View>;
                                                                {/* Links Accounts */}
                                                                <Text>Link Accounts</Text>;

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
                                                            "Link " + std::to_string(provider);
                                                        disabled={oauth.state.status == "loading"}
                                                    onPress={() => oauth.link({ provider })}
                                                    ></Button>;
                                                    </View>;
                                                ))}
                                                </View>;
                                                <Button;
                                                title="Link Passkey";
                                                onPress={() =>;
                                                linkWithPasskey({
                                                    relyingParty: Constants.expoConfig.extra.passkeyAssociatedDomain,
                                                    });
                                                }
                                                />;
                                                <View;
                                                style={{
                                                    display: "flex",
                                                    flexDirection: "column",
                                                    gap: 10,
                                                    borderBlockColor: "black",
                                                    borderWidth: 1,
                                                    padding: 10,
                                                }}
                                                >;
                                                <Text>Link Email</Text>;
                                                <View style={{ display: "flex", flexDirection: "row", gap: 10 }}>
                                            {" "}
                                            <TextInput;
                                        value={email}
                                    onChangeText={setEmail}
                                    placeholder="Email";
                                    style={{
                                        borderWidth: 1,
                                        borderColor: "black",
                                        padding: 10,
                                        margin: 10,
                                        color: "black",
                                    }}
                                    />;
                                    <Button title="Get Code" onPress={() => sendCodeEmail({ email })} />;
                                    </View>;
                                    <View style={{ display: "flex", flexDirection: "row", gap: 10 }}>
                                    <TextInput;
                                value={emailCode}
                            onChangeText={setEmailCode}
                            placeholder="Code";
                            inputMode="numeric";
                            style={{
                                borderWidth: 1,
                                borderColor: "black",
                                padding: 10,
                                margin: 10,
                                color: "black",
                            }}
                            />;
                            <Button;
                            title="Link email";
                        onPress={() => linkWithCodeEmail({ code: emailCode, email })}
                        />;
                        </View>;
                        </View>;
                        <View;
                        style={{
                            display: "flex",
                            flexDirection: "column",
                            gap: 10,
                            borderBlockColor: "black",
                            borderWidth: 1,
                            padding: 10,
                        }}
                        >;
                        <Text>Link SMS</Text>;
                        <View style={{ display: "flex", flexDirection: "row", gap: 10 }}>
                        <TextInput;
                    value={phone}
                onChangeText={setPhone}
                placeholder="Phone";
                style={{
                    borderWidth: 1,
                    borderColor: "black",
                    padding: 10,
                    margin: 10,
                    color: "black",
                }}
                />;
                <Button title="Link SMS" onPress={() => sendCodeSMS({ phone })} />;
                </View>;
                <View style={{ display: "flex", flexDirection: "row", gap: 10 }}>
                <TextInput;
            value={smsCode}
        onChangeText={setSmsCode}
        placeholder="Code";
        inputMode="numeric";
        style={{
            borderWidth: 1,
            borderColor: "black",
            padding: 10,
            margin: 10,
            color: "black",
        }}
        />;
        <Button;
        title="Link Code";
        onPress={() => linkWithCodeSMS({ code: smsCode, phone })}
        />;
        </View>{" "}
        </View>;
        <Button title="Link Farcaster" onPress={() => linkWithFarcaster({})} />;
        {error && <Text style={{ color = "red" }}>Error = {error}</Text>}
        </View>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
