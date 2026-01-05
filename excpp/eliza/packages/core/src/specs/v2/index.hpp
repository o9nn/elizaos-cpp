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
#include "types.hpp"

// Then all other includes
#include "actions.hpp"
#include "database.hpp"
#include "entities.hpp"
#include "logger.hpp"
#include "prompts.hpp"
#include "roles.hpp"
#include "runtime.hpp"
// really shouldn't be exported
//#include "search.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include "services.hpp"

} // namespace elizaos
