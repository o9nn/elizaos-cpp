#include "elizaos/core.hpp"
#include "types.hpp"
#include "utils/confidenceDecay.hpp"
#include "utils/experienceAnalyzer.hpp"
#include "utils/experienceRelationships.hpp"
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



class ExperienceService extends Service {
  static override serviceType: ServiceTypeName =
    ExperienceServiceType.EXPERIENCE;
  override capabilityDescription =
    "Manages agent experiences, learning from successes and failures to improve future decisions";

  private experiences: Map<UUID, Experience> = new Map();
  private experiencesByDomain: Map<string, Set<UUID>> = new Map();
  private experiencesByType: Map<ExperienceType, Set<UUID>> = new Map();
  private maxExperiences = 10000; // Configurable limit - reverted to default
  private decayManager: ConfidenceDecayManager;
  private relationshipManager: ExperienceRelationshipManager;

  constructor(runtime: IAgentRuntime) {
    super(runtime);
    this.decayManager = new ConfidenceDecayManager();
    this.relationshipManager = new ExperienceRelationshipManager();
    this.loadExperiences();
  }

    // loadExperiences is called by constructor

      // TODO: Load from knowledge service if available

      // Generate embedding for the experience
        // Leave embedding undefined on error

      // Store the experience

      // Update indexes

      // Check for contradictions and add relationships

      // Prune if necessary

      // Emit event

    // Start with all experiences

    // Apply filters

    // Sort by relevance (considering decayed confidence)

    // Apply limit

    // Include related experiences if requested

    // Update access counts

      // Generate embedding for the query text

      // Calculate similarities

      // Sort by similarity and return top results

      // Update access counts

    // Analyze patterns

    // Calculate reliability based on consistency and confidence

    // Find alternatives

    // Generate recommendations

          // Skip short words

    // Return words that appear in at least 30% of texts

        // Extract alternative from learning

    // Add specific recommendations based on patterns

    // Add domain-specific recommendations

    // Sort experiences by importance (ascending) and access count (ascending)
      // First sort by importance
      // Then by access count
      // Finally by age (older first)

    // Remove the least important experiences

      // Remove from domain index

      // Remove from type index

    // TODO: Save experiences to database

} // namespace elizaos
