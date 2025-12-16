#include ".types.hpp"
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



struct DecayConfig {
    number; // Time in milliseconds for confidence to decay by half halfLife;
    number; // Minimum confidence level (never decays below this) minConfidence;
    number; // Time before decay starts (grace period) decayStartDelay;
};

class ConfidenceDecayManager {
  private config: DecayConfig;

  constructor(config: Partial<DecayConfig> = {}) {
    this.config = { ...DEFAULT_DECAY_CONFIG, ...config };
  }

  /**
   * Calculate the decayed confidence for an experience
   */

    // No decay during grace period

    // Calculate decay based on half-life

    // Apply decay but respect minimum

  /**
   * Get experiences that need reinforcement (low confidence due to decay)
   */

  /**
   * Calculate reinforcement boost when an experience is validated
   */

  /**
   * Adjust decay rate based on experience type and domain
   */

    // Facts and discoveries decay slower

    // Warnings and corrections decay slower (important to remember)

    // Domain-specific adjustments

  /**
   * Get confidence trend for an experience over time
   */


} // namespace elizaos
