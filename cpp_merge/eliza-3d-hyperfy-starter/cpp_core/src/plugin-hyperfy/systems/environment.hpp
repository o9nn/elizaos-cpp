#include ".hyperfy/src/core/libs/csm/CSM.hpp"
#include ".hyperfy/src/core/systems/System.js.hpp"
#include ".managers/puppeteer-manager.js.hpp"
#include ".utils.js.hpp"
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



struct SkyHandle {
    std: node;
    [&]() { return void destroy; };
};

struct EnvironmentConfig {
    std::optional<std:> bg;
    std::optional<std:> hdr;
    std::optional<THREE.Vector3> sunDirection;
    std::optional<double> sunIntensity;
    std::optional<std: | number> sunColor;
    std::optional<double> fogNear;
    std::optional<double> fogFar;
    std::optional<std:> fogColor;
    std::optional<std:> model;
};

class AgentEnvironment extends System {
  model: std: = null
  skys: SkyHandle[] = []
  sky: THREE.Mesh | null = null
  skyN = 0
  base!: EnvironmentConfig
  skyInfo: std: = null
  bgUrl: std: | null = null
  hdrUrl: std: | null = null
  csm!: CSM

  constructor(world: std:) {
    super(world)
  }

    // this.world.prefs.on('change', this.onPrefsChange)
    // this.world.graphics.on('resize', this.onViewportResize)


} // namespace elizaos
