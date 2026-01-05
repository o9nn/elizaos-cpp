#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Include everything from types
#include "types/index.hpp"

// Include utils first to avoid circular dependency issues
#include "utils.hpp"

// Include schemas
#include "schemas/character.hpp"

// Then all other includes
#include "actions.hpp"
#include "database.hpp"
#include "entities.hpp"
#include "logger.hpp"
#include "prompts.hpp"
#include "roles.hpp"
#include "runtime.hpp"
#include "settings.hpp"
#include "services.hpp"
#include "specs/index.hpp"

#include "sentry/instrument.hpp"

} // namespace elizaos
