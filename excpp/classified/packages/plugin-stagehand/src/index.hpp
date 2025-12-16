#include "plugin.js.hpp"
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

// Export service
{ StagehandService, BrowserSession } from './service.js';

// Export actions for testing
{
  browserNavigateAction,
  browserClickAction,
  browserTypeAction,
  browserSelectAction,
  browserExtractAction,
  browserScreenshotAction,
} from './plugin.js';

// Re-plugin
{ stagehandPlugin } from './plugin.js';
;
default stagehandPlugin;

} // namespace elizaos
