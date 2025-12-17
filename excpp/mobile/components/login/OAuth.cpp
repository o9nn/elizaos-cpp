#include "OAuth.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void OAuth() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState("");
        const auto oauth = useLoginWithOAuth({;
            onError: (err) => {
                std::cout << err << std::endl;
                setError(/* JSON.stringify */ std::string(err.message));
                },
                });
                return (;
                <View>;
                <View;
                style={{
                    display: "flex",
                    flexDirection: "row",
                    flexWrap: "wrap",
                    justifyContent: "center",
                    alignItems: "center",
                    gap: 5,
                    margin: 10,
                }}
                >;
                {[;
                "github",
                "google",
                "discord",
                "apple",
                "twitter",
                "tiktok",
                "spotify",
                "instagram",
                "linkedin",
                "line",
                ].map((provider) => (;
                <View key={provider}>;
                <Button;
            "title={" + "Login with " + provider;
        disabled={oauth.state.status == "loading"}
        onPress={() => oauth.login({ provider })}
        ></Button>;
        </View>;
        ))}
        </View>;
        {error && <Text style={{ color = "red" }}>Error = {error}</Text>}
        </View>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
