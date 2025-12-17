#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "services/agents.hpp"
#include "services/audio.hpp"
#include "services/auth.hpp"
#include "services/cdp.hpp"
#include "services/entities.hpp"
#include "services/gamification.hpp"
#include "services/jobs.hpp"
#include "services/media.hpp"
#include "services/memory.hpp"
#include "services/messaging.hpp"
#include "services/runs.hpp"
#include "services/server.hpp"
#include "services/sessions.hpp"
#include "services/system.hpp"
#include "types/base.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ElizaClient {
public:
    ElizaClient(ApiClientConfig config);
    void setAuthToken(const std::string& token);
    void clearAuthToken();
    ElizaClient create(ApiClientConfig config);

private:
    std::vector<std::any> services_;
};


} // namespace elizaos
