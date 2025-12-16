#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "util.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Use the legacy getter for initialization

struct InitSdkOptions {
    std::optional<bool> loadToken;
    std::optional<std::any> env;
};

    // Set the cluster from env or use default

    // Get connection based on env if provided

    // Create a new instance each time since we're passing potentially different config
      // urlConfigs: {
      //   BASE_HOST: '<API_HOST>', // api url configs, currently api doesn't support devnet
      // },


} // namespace elizaos
