#pragma once

#include <string>
#include "dreamgen_provider.hpp"
#include "koboldcpp_provider.hpp"

namespace elizaos {

/**
 * DgenKoboldPlugin - ElizaOS plugin providing DreamGen and KoboldCpp model providers.
 *
 * This plugin registers two model providers with the ElizaOS runtime:
 *
 * 1. DreamGen (cloud): High-quality creative text generation via DreamGen's lucid-v1 models.
 *    Supports character-based role-play with the extended 'text' role, narrator mode,
 *    and the DRY sampler for reduced repetition.
 *
 * 2. KoboldCpp (local): Fast, private inference using locally-running GGUF models.
 *    Supports both the OpenAI-compatible API and KoboldCpp's native API with
 *    advanced sampling parameters.
 *
 * Composition: DreamGen ⊕ KoboldCpp (additive - runtime selection)
 *
 * Usage in character.json:
 *   { "modelProvider": "dreamgen" }   -- for cloud creative writing
 *   { "modelProvider": "koboldcpp" }  -- for local private inference
 */
class DgenKoboldPlugin {
public:
    static constexpr const char* PLUGIN_NAME = "dgen_kobold_provider";
    static constexpr const char* PLUGIN_DESCRIPTION =
        "Provides DreamGen cloud and KoboldCpp local LLM inference for ElizaOS agents";

    /**
     * Get the plugin name.
     */
    static std::string getName() { return PLUGIN_NAME; }

    /**
     * Get the plugin description.
     */
    static std::string getDescription() { return PLUGIN_DESCRIPTION; }
};

} // namespace elizaos
