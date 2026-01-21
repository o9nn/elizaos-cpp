#include ".service.hpp"
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



enum SnapshotType {
  LOOK_AROUND     = 'LOOK_AROUND',
  LOOK_DIRECTION  = 'LOOK_DIRECTION',
  LOOK_AT_ENTITY  = 'LOOK_AT_ENTITY'
}

/* -------------------------------------------------------------------------- */
/* HYPERFY_SCENE_PERCEPTION action                                            */
/* -------------------------------------------------------------------------- */

      await callback({ text: 'Unable to observe environment. Hyperfy world not available.' });

    /* Decide snapshot strategy */
      await callback({ thought: 'Cannot decide how to look.', metadata: { error: 'selector_failure' } });

    /* Capture snapshot */
      await callback({ thought: 'Snapshot failed.', metadata: { error: 'snapshot_failure', snapshotType } });

    /* IMAGE_DESCRIPTION – detailed scene analysis */
      await callback({ thought: 'Cannot understand the scene.', metadata: { error: 'vision_failure' } });

    //  Add dynamic header for scene perception

    /* generate final XML response */
      await callback({ thought: 'No response generated.', metadata: { error: 'text_large_failure' } });

      await callback({ thought: 'Malformed XML.', metadata: { error: 'xml_parse_failure', xmlRaw } });

    // General observation
  
    // Spatial direction
  
    // Exploration or scouting
  
    // Object-centric look
  
    // Directional command
  
    // Subtle curiosity
  
    // Tactical evaluation
  
    // Emotional tone: concern
  
    // Humor or roleplay
  
    // Checking behind an object

} // namespace elizaos
