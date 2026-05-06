#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/SamTTSService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * SAY_ALOUD Action
 *
 * Makes the agent speak text aloud using the SAM speech synthesizer.
 * The audio is sent through the hardware bridge to the user's speakers.
 */

    // Trigger words and phrases for speech synthesis

    // Check if message contains speech-related triggers

    // Also check if the user is asking agent to vocalize something

    // Get SAM TTS service

    // Extract text to speak from the message

    // Extract voice options from the message

    // Synthesize and play the speech

/**
 * Extract text to speak from user message
 */
std::string extractTextToSpeak(const std::string& messageText);

/**
 * Extract voice configuration options from user message
 */
SamTTSOptions extractVoiceOptions(const std::string& messageText);

} // namespace elizaos
