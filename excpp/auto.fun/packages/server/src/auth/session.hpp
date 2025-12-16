#include ".redis.hpp"
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


const SESSION_TTL = 1 * 24 * 60 * 60;

struct SessionData {
    std::string publicKey;
    std::optional<std::vector<std::string>> privileges;
    double createdAt;
};


std::future<std::string> createSession(SessionData data);`, JSON.stringify(data), SESSION_TTL);
   return sid;
}

std::future<SessionData | null> getSession(const std::string& sid);`);
   return raw ? (JSON.parse(raw) as SessionData) : null;
}

async `);
}

} // namespace elizaos
