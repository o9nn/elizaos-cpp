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

  // getBoneTransform?: (boneName: string) => THREE.Matrix4

class AgentAvatar extends Node {
  private _src: string, null = defaults.src
  private _emote: string, null = defaults.emote
  private _onLoad: (() => void), null = defaults.onLoad

  public factory: AvatarFactory, null = null
  public hooks: any = null
  public instance: AvatarInstance, null = null
  private n = 0
  private needsRebuild: boolean = false

  constructor(data: Partial<{
    id: string
    src: string
    emote: string
    onLoad: () => void
    factory: AvatarFactory
    hooks: any
  }> = {}) {
    super(data)
    this.name = 'avatar'
  
    this.src = data.src ?? defaults.src
    this.emote = data.emote ?? defaults.emote
    this.onLoad = data.onLoad ?? defaults.onLoad
    this.factory = data.factory ?? null
    this.hooks = data.hooks ?? null
  }

      // throw new Error('[avatar] emote not a string')

  
} // namespace elizaos
