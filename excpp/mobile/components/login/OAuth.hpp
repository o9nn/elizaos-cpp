#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;

default ,
  });
  return (
    <View>
      <View
        style={{
          display: "flex",
          flexDirection: "row",
          flexWrap: "wrap",
          justifyContent: "center",
          alignItems: "center",
          gap: 5,
          margin: 10,
        }}
      >
        {[
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
        ].map((provider) => (
          <View key={provider}>
            <Button
              title={`Login with ${provider}`}
              disabled={oauth.state.status === "loading"}
              onPress={() => oauth.login({ provider } as LoginWithOAuthInput)}
            ></Button>
          </View>
        ))}
      </View>
      {error && <Text style={{ color: "red" }}>Error: {error}</Text>}
    </View>
  );
}

} // namespace elizaos
