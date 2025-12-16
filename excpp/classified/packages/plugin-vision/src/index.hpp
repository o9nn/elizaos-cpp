#include "action-audio.hpp"
#include "action.hpp"
#include "elizaos/core.hpp"
#include "provider.hpp"
#include "service.hpp"
#include "tests/e2e.hpp"
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

// Vision plugin -- gives an agent visual perception through camera integration
;
;
;
;
;
;

const visionPlugin: Plugin = {
  name: 'vision',
  description:
    'Provides visual perception through camera integration, scene analysis, and audio capture',
  services: [VisionService],
  providers: [visionProvider],
  actions: [
    // Scene analysis enabled for informational purposes
    describeSceneAction,
    // Image capture disabled by default (privacy-sensitive)
    captureImageAction,
    // Vision mode setting enabled for configuration
    setVisionModeAction,
    // Entity naming enabled for scene understanding
    nameEntityAction,
    // Person identification disabled by default (privacy-sensitive)
    identifyPersonAction,
    // Entity tracking disabled by default (privacy-sensitive)
    trackEntityAction,
    // Audio transcription actions
    transcribeAudioAction,
    toggleStreamingAudioAction,
  ],
  tests: testSuites,
  init: async (_config, _runtime) => {
    // Plugin initialization
    // The VisionService will be automatically registered and started by the runtime
    // Additional initialization logic can be added here if needed
  },
};

default visionPlugin;

// Export service for external use
{ VisionService } from './service';

} // namespace elizaos
