#include "PrivyUI.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PrivyUI() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [error, setError] = useState("");

        const auto { login } = useLogin();
        return (;
        <View>;
        <Button;
        title="Login with Privy UIs";
        onPress={() => {
            login({ loginMethods: ["email"] })
            .then((session) => {
                std::cout << "User logged in" << session.user << std::endl;
                });
                .catch((err) => {
                    setError(/* JSON.stringify */ std::string(err.error));
                    });
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
