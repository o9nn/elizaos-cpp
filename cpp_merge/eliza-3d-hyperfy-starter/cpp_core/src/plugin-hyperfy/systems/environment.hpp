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
    std::string node;
    [&]() { return void destroy; };
};

struct EnvironmentConfig {
    std::optional<std::string> bg;
    std::optional<std::string> hdr;
    std::optional<THREE.Vector3> sunDirection;
    std::optional<double> sunIntensity;
    std::optional<std::string | number> sunColor;
    std::optional<double> fogNear;
    std::optional<double> fogFar;
    std::optional<std::string> fogColor;
    std::optional<std::string> model;
};

class AgentEnvironment : public System {
  model: std::string = null
  skys: SkyHandle[] = []
  sky: THREE.Mesh | null = null
  skyN = 0
  base!: EnvironmentConfig
  skyInfo: std::string = null
  bgUrl: std::string | null = null
  hdrUrl: std::string | null = null
  csm!: CSM

  /* constructor */ (world: std::string) {
    super(world)
  }

    // this.world.prefs.on('change', this.onPrefsChange)
    // this.world.graphics.on('resize', this.onViewportResize)


} // namespace elizaos
