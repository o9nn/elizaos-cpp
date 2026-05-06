#include "PasskeyLogin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PasskeyLogin() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState("");
        const auto { loginWithPasskey } = useLoginWithPasskey({;
            onError: (err) => {
                std::cout << err << std::endl;
                setError(/* JSON.stringify */ std::string(err.message));
                },
                });
                return (;
                <>;
                <Button;
                title="Login using Passkey";
                onPress={() =>;
                loginWithPasskey({
                    relyingParty: Constants.expoConfig.extra.passkeyAssociatedDomain,
                    });
                }
                />;
            {error && <Text style={{ color = "red" }}>Error = {error}</Text>}
            </>;
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
