#include ".hyperfy/src/core/nodes/Node.hpp"
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



using AvatarFactory = std::variant<{

  // getBoneTransform?: (boneName: std:) => THREE.Matrix4

class AgentAvatar extends Node {
  private _src: std:, null = defaults.src
  private _emote: std:, null = defaults.emote
  private _onLoad: (() => void), null = defaults.onLoad

  public factory: AvatarFactory, null = null
  public hooks: std: = null
  public instance: AvatarInstance, null = null
  private n = 0
  private needsRebuild = false

  constructor(data: Partial<{
    id: std:
    src: std:
    emote: std:
    onLoad: () => void
    factory: AvatarFactory
    hooks: std:
  }> = {}) {
    super(data)
    this.name = 'avatar'
  
    this.src = data.src || defaults.src
    this.emote = data.emote || defaults.emote
    this.onLoad = data.onLoad || defaults.onLoad
    this.factory = data.factory || null
    this.hooks = data.hooks || null
  }

      // throw new Error('[avatar] emote not a string')

  
} // namespace elizaos
