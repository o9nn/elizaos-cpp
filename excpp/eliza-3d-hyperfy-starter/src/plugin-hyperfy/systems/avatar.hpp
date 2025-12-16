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





const defaults = {
  src: null as string | null,
  emote: null as string | null,
  onLoad: null as (() => void) | null,
}

using AvatarFactory = std::variant<{
  create: (
    matrixWorld: THREE.Matrix4,
    hooks: any,
    avatarNode: AgentAvatar
  ) => AvatarInstance
  applyStats?: (stats: any) => void
}

type AvatarInstance = {
  move: (matrixWorld: THREE.Matrix4) => void
  destroy: () => void
  setEmote: (emote: string, null) => void
  height?: number
  headToHeight?: number
  // getBoneTransform?: (boneName: string) => THREE.Matrix4
}

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

  async mount() {
    this.needsRebuild = false
    if (this._src) {
      const n = ++this.n
      let avatar = this.ctx.world.loader.get('avatar', this._src)
      if (!avatar) avatar = await this.ctx.world.loader.load('avatar', this._src)
      if (this.n !== n) return
      this.factory = avatar?.factory ?? null
      this.hooks = avatar?.hooks ?? null
    }
    if (this.factory) {
      this.instance = this.factory.create(this.matrixWorld, this.hooks, this)
      this.instance.setEmote(this._emote)
      this.ctx.world?.setHot(this.instance, true)
      this._onLoad?.()
    }
  }

  commit(didMove: boolean) {
    if (this.needsRebuild) {
      this.unmount()
      this.mount()
    }
    if (didMove) {
      this.instance?.move(this.matrixWorld)
    }
  }

  unmount() {
    this.n++
    if (this.instance) {
      this.ctx.world?.setHot(this.instance, false)
      this.instance.destroy()
      this.instance = null
    }
  }

  applyStats(stats: any) {
    this.factory?.applyStats?.(stats)
  }

  get src(): string, null {
    return this._src
  }

  set src(value: string, null) {
    if (value !== null && !isString(value)) {
      throw new Error('[avatar] src not a string')
    }
    if (this._src === value) return
    this._src = value
    this.needsRebuild = true
    this.setDirty()
  }

  get emote(): string, null {
    return this._emote
  }

  set emote(value: string, null) {
    if (value !== null && !isString(value)) {
      // throw new Error('[avatar] emote not a string')
      return>;
    }
    if (this._emote === value) return
    this._emote = value
    this.instance?.setEmote(value)
  }

  get onLoad(): (() => void) | null {
    return this._onLoad
  }

  set onLoad(value: (() => void) | null) {
    this._onLoad = value
  }

  getHeight(): number | null {
    return this.instance?.height ?? null
  }

  getHeadToHeight(): number | null {
    return this.instance?.headToHeight ?? null
  }

  getBoneTransform(_boneName: string): THREE.Matrix4 {
    const matrix = new THREE.Matrix4()
    if (this.parent && this.parent.position) {
      matrix.makeTranslation(
        this.parent.position.x,
        this.parent.position.y,
        this.parent.position.z
      )
    }
    return matrix
  }

  setEmote(url: string | null) {
    this.emote = url
  }

  get height(): number | null {
    return this.getHeight()
  }

  copy(source: AgentAvatar, recursive: boolean): this {
    super.copy(source, recursive)
    this._src = source._src
    this._emote = source._emote
    this._onLoad = source._onLoad
    this.factory = source.factory
    this.hooks = source.hooks
    return this
  }

  getProxy(): any {
    if (!this.proxy) {
      const self = this
      const baseProxy = super.getProxy()
  
      this.proxy = {
        ...baseProxy,
  
        get src() {
          return self.src
        },
        set src(value: string | null) {
          self.src = value
        },
  
        get emote() {
          return self.emote
        },
        set emote(value: string | null) {
          self.emote = value
        },
  
        get onLoad() {
          return self.onLoad
        },
        set onLoad(value: (() => void) | null) {
          self.onLoad = value
        },
  
        getHeight() {
          return self.getHeight()
        },
  
        getHeadToHeight() {
          return self.getHeadToHeight()
        },
  
        getBoneTransform(boneName: string) {
          return self.getBoneTransform(boneName)
        },
  
        setEmote(url: string | null) {
          return self.setEmote(url)
        },
  
        get height() {
          return self.height
        },
      }
    }
  
    return this.proxy
  }
  
}
} // namespace elizaos
