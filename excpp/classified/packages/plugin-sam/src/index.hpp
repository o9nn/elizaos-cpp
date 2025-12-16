#include "actions/sayAloud.hpp"
#include "services/SamTTSService.hpp"
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

import type { Plugin } from '@elizaos/core';
;
;

/**
 * Plugin SAM - Retro TTS using SAM Speech Synthesizer
 *
 * Provides classic 1980s text-to-speech capabilities using the SAM synthesizer.
 * Integrates with the hardware bridge to send audio directly to user speakers.
 */
const samPlugin: Plugin = {
  name: '@elizaos/plugin-sam',
  description: 'Retro text-to-speech using SAM Speech Synthesizer with hardware bridge integration',
  actions: [sayAloudAction],
  services: [SamTTSService],
};

default samPlugin;
* from './actions/sayAloud';
* from './services/SamTTSService';

} // namespace elizaos
