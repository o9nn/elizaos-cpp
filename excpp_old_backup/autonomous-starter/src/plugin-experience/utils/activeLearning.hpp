#include ".types.hpp"
#include "confidenceDecay.hpp"
#include "experienceRelationships.hpp"
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



struct Experiment {
    std::string id;
    std::string hypothesis;
    std::string action;
    std::string expectedOutcome;
    std::string domain;
    double priority;
    std::vector<std::string> relatedExperiences;
    double suggestedAt;
};

struct LearningGap {
    std::string domain;
    std::string description;
    double uncertainty;
    std::vector<Experiment> suggestedExperiments;
};

class ActiveLearningManager {
  private relationshipManager: ExperienceRelationshipManager;
  private decayManager: ConfidenceDecayManager;

  constructor() {
    this.relationshipManager = new ExperienceRelationshipManager();
    this.decayManager = new ConfidenceDecayManager();
  }

  /**
   * Identify areas where the agent needs more experience
   */

    // Group experiences by domain

    // Analyze each domain
      // Check for low confidence areas

      // Check for contradictions

      // Check for old hypotheses that haven't been validated

  /**
   * Suggest experiments to fill learning gaps
   */

    // Get the highest priority gap

    // Return the highest priority experiment from that gap

        // Same action, different outcome

      // Check if there's a validation experience

  /**
   * Generate a learning curriculum based on current knowledge
   */

    // Start with basic exploration if few experiences

      // Suggest exploratory experiments

    // Add experiments to resolve uncertainties

    // Sort by priority

} // namespace elizaos
