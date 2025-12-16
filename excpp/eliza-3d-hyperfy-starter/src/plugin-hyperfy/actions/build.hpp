#include ".service.hpp"
#include ".systems/controls.hpp"
#include "elizaos/core.hpp"
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



  enum EditOperationType {
    DUPLICATE = 'duplicate',
    TRANSLATE = 'translate',
    ROTATE = 'rotate',
    SCALE = 'scale',
    DELETE = 'delete',
    IMPORT = 'import'
  }

  
} // namespace elizaos
