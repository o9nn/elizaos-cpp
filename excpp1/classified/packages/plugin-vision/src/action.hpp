#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "service.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Vision actions for scene analysis and image capture

    // Check if vision capabilities are enabled in runtime settings

      // Build comprehensive scene description

      // Overall scene

      // People in scene

      // Objects in scene

      // Environment details from enhanced scene

  // Note: This action is disabled by default - privacy-sensitive, can capture images
    // Check if camera or screen capture capabilities are enabled in runtime settings

      // Create image attachment

      //   runtime,
      //   message,
      //   thought,
      //   text,
      //   ['CAPTURE_IMAGE'],
      //   [imageAttachment]
      // );

    // Multi-action: Describe scene then capture image

    // Check if any vision capabilities are enabled in runtime settings

      // Extract the desired mode from the message

// Enhanced actions for entity tracking and face recognition

    // Check if vision capabilities are enabled in runtime settings

      // Extract name from message

      // Get entity tracker

      // Update entities

      // For now, assign to the most prominent person (largest bounding box)

      // Assign the name

        // Success response

  // Note: This action is disabled by default - privacy-sensitive, can identify and recognize people

    // Check if vision capabilities are enabled in runtime settings

      // Get entity tracker

      // Update entities

      // Build response about visible people

          // Add more context if available

      // Check for recently departed people

  // Note: This action is disabled by default - privacy-sensitive, can track and monitor people

    // Check if vision capabilities are enabled in runtime settings

      // Get entity tracker

      // Update entities


} // namespace elizaos
