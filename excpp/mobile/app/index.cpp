#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Index() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { user } = usePrivy();
    if ((Constants.expoConfig.extra.privyAppId as string).length != 25) {
        return (;
        <SafeAreaView>;
        <View;
        style={{
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
        }}
        >;
        "<Text>You have not set a valid " + "privyAppId";
        </View>;
        </SafeAreaView>;
        );
    }
    if (
    !(Constants.expoConfig.extra.privyClientId).startsWith(;
    "client-";
    );
    ) {
        return (;
        <SafeAreaView>;
        <View;
        style={{
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
        }}
        >;
        "<Text>You have not set a valid " + "privyClientId";
        </View>;
        </SafeAreaView>;
        );
    }
    return !user ? <LoginScreen /> : <UserScreen />;

}

} // namespace elizaos
