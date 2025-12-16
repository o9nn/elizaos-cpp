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
    std::any node;
    () => void destroy;
};

struct EnvironmentConfig {
    std::optional<std::string> bg;
    std::optional<std::string> hdr;
    std::optional<THREE.Vector3> sunDirection;
    std::optional<double> sunIntensity;
    std::optional<string | number> sunColor;
    std::optional<double> fogNear;
    std::optional<double> fogFar;
    std::optional<std::string> fogColor;
    std::optional<std::string> model;
};

class AgentEnvironment extends System {
  model: any = null
  skys: SkyHandle[] = []
  sky: THREE.Mesh | null = null
  skyN = 0
  base!: EnvironmentConfig
  skyInfo: any = null
  bgUrl: string | null = null
  hdrUrl: string | null = null
  csm!: CSM

  constructor(world: any) {
    super(world)
  }

    // this.world.prefs.on('change', this.onPrefsChange)
    // this.world.graphics.on('resize', this.onViewportResize)


} // namespace elizaos
