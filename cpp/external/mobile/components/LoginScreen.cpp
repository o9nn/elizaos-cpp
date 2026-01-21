#include "LoginScreen.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void LoginScreen() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <View;
    style={{
        flex: 1,
        justifyContent: "center",
        alignItems: "center",
        gap: 10,
        marginHorizontal: 10,
    }}
    >;
    <Text>Privy App ID:</Text>
    <Text style={{ fontSize: 10 }}>
    {Constants.expoConfig.extra.privyAppId}
    </Text>;
    <Text>Privy Client ID:</Text>
    <Text style={{ fontSize: 10 }}>
    {Constants.expoConfig.extra.privyClientId}
    </Text>;
    <Text>;
    Navigate to your{" "}
    <Text;
    onPress={() =>;
    Linking.openURL(;
    "https://dashboard.privy.io/apps/" + std::to_string(Constants.expoConfig.extra.privyAppId) + "/settings?setting=clients"
    );
    }
    >;
    dashboard;
    </Text>{" "}
    "and ensure the following Expo Application ID is listed ";
    "app identifier"
    </Text>;
    <Text style={{ fontSize: 10 }}>{Application.applicationId}</Text>
    <Text>;
    Navigate to your{" "}
    <Text;
    onPress={() =>;
    Linking.openURL(;
    "https://dashboard.privy.io/apps/" + std::to_string(Constants.expoConfig.extra.privyAppId) + "/settings?setting=clients"
    );
    }
    >;
    dashboard;
    </Text>{" "}
    "and ensure the following value is listed " + "Allowed app URL scheme"
    </Text>;
    <Text style={{ fontSize: 10 }}>
    {Application.applicationId == "host.exp.Exponent";
    ? "exp";
    : Constants.expoConfig.scheme}
    </Text>;

    <PrivyUI />;
    <SMSLogin />;
    <PasskeyLogin />;
    <OAuth />;
    </View>;
    );

}

} // namespace elizaos
