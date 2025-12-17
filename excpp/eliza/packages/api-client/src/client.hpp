#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "services/agents.hpp"
#include "services/audio.hpp"
#include "services/media.hpp"
#include "services/memory.hpp"
#include "services/messaging.hpp"
#include "services/server.hpp"
#include "services/system.hpp"
#include "types/base.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ElizaClient {
public:
    ElizaClient(ApiClientConfig config);
    static ElizaClient create(ApiClientConfig config);
};


} // namespace elizaos
