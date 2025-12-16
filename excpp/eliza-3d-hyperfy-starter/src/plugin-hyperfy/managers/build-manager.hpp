#include ".hyperfy/src/core/extras/appTools.js.hpp"
#include ".hyperfy/src/core/utils-client.js.hpp"
#include ".hyperfy/src/core/utils.js.hpp"
#include ".service.hpp"
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

;
;




;


class BuildManager {
  private runtime: IAgentRuntime;
  
  constructor(runtime: IAgentRuntime) {
    this.runtime = runtime;
  }
  
  async translate(entityId, position: [number, number, number]) {
    const service = this.getService();
    const world = service.getWorld();
    const entity = world.entities.items.get(entityId);
    if (entity) {
      const controls = world.controls;
      if (controls) {
        await controls.goto(entity.root.position.x, entity.root.position.z);
      }
      entity.root.position.fromArray(position);
      this.entityUpdate(entity);
    }
  }

  async rotate(entityId, quaternion: [number, number, number, number]) {
    const service = this.getService();
    const world = service.getWorld();
    const entity = world.entities.items.get(entityId);
    if (entity) {
      const controls = world.controls;
      if (controls) {
        await controls.goto(entity.root.position.x, entity.root.position.z);
      }
      entity.root.quaternion.fromArray(quaternion);
      this.entityUpdate(entity);
    }
  }
  
  async scale(entityId, scale: [number, number, number]) {
    const service = this.getService();
    const world = service.getWorld();
    const entity = world.entities.items.get(entityId);
    if (entity) {
      const controls = world.controls;
      if (controls) {
        await controls.goto(entity.root.position.x, entity.root.position.z);
      }
      entity.root.scale.fromArray(scale);
      this.entityUpdate(entity);
    }
  }

  async duplicate(entityId) {
    const service = this.getService();
    const world = service.getWorld();
    const entity = world.entities.items.get(entityId);
    const controls = world.controls;
    if (controls) {
      await controls.goto(entity.root.position.x, entity.root.position.z);
    }
    if (entity?.isApp) {
      let blueprintId = entity.data.blueprint
      // if unique, we also duplicate the blueprint
      if (entity.blueprint.unique) {
        const blueprint = {
          id: uuid(),
          version: 0,
          name: entity.blueprint.name,
          image: entity.blueprint.image,
          author: entity.blueprint.author,
          url: entity.blueprint.url,
          desc: entity.blueprint.desc,
          model: entity.blueprint.model,
          script: entity.blueprint.script,
          props: cloneDeep(entity.blueprint.props),
          preload: entity.blueprint.preload,
          public: entity.blueprint.public,
          locked: entity.blueprint.locked,
          frozen: entity.blueprint.frozen,
          unique: entity.blueprint.unique,
          disabled: entity.blueprint.disabled,
        }
        world.blueprints.add(blueprint, true)
        blueprintId = blueprint.id
      }
      const data = {
        id: uuid(),
        type: 'app',
        blueprint: blueprintId,
        position: entity.root.position.toArray(),
        quaternion: entity.root.quaternion.toArray(),
        scale: entity.root.scale.toArray(),
        mover: null,
        uploader: null,
        pinned: false,
        state: {},
      }
      world.entities.add(data, true)
    }
  }

  async delete(entityId) {
    const service = this.getService();
    const world = service.getWorld();
    const entity = world.entities.items.get(entityId);
    if (entity?.isApp && !entity.data.pinned) {
      const controls = world.controls;
      if (controls) {
        await controls.goto(entity.root.position.x, entity.root.position.z);
      }
      entity?.destroy(true)
      this.entityUpdate(entity);
    }
  }

  async importEntity(
    url: string,
    position?: any,
    quaternion?: any
  ) {
    const service = this.getService();
    const world = service.getWorld();
    const resolvedUrlurl = await resolveUrl(url, world);
    let file
    
    const resp = await fetch(resolvedUrlurl)
    const blob = await resp.blob()
    file = new File([blob], url.split('/').pop(), { type: resp.headers.get('content-type') })
    if (!file) return
    
    await new Promise(resolve => setTimeout(resolve, 100))
    
    const maxSize = world.network.maxUploadSize * 1024 * 1024

    if (file.size > maxSize) {
      console.error(`File too large. Maximum size is ${maxSize / (1024 * 1024)}MB`)
      return
    }
    const validVec3 = (v: any): v is [number, number, number] =>
      Array.isArray(v) && v.length === 3 && v.every(n => typeof n === 'number');

    const validQuat = (q: any): q is [number, number, number, number] =>
      Array.isArray(q) && q.length === 4 && q.every(n => typeof n === 'number');

    position = validVec3(position) ? position : [0, 0, 0];
    quaternion = validQuat(quaternion) ? quaternion : [0, 0, 0, 1];
    
    const controls = world.controls;
    if (controls) {
      await controls.goto(position[0], position[2]);
    }
    
    const transform = {
      position,
      quaternion,
    };
    const ext = file.name.split('.').pop().toLowerCase()
    if (ext === 'hyp') {
      this.addApp(file, transform)
    }
    if (ext === 'glb' || ext === 'vrm') {
      this.addModel(file, transform)
    }
  }

  async addApp(file, transform) {
    const service = this.getService();
    const world = service.getWorld();
    const info = await importApp(file)
    
    const blueprint = {
      id: uuid(),
      version: 0,
      name: info.blueprint.name,
      image: info.blueprint.image,
      author: info.blueprint.author,
      url: info.blueprint.url,
      desc: info.blueprint.desc,
      model: info.blueprint.model,
      script: info.blueprint.script,
      props: info.blueprint.props,
      preload: info.blueprint.preload,
      public: info.blueprint.public,
      locked: info.blueprint.locked,
      frozen: info.blueprint.frozen,
      unique: info.blueprint.unique,
      disabled: info.blueprint.disabled,
    }
    world.blueprints.add(blueprint, true)
    const data = {
      id: uuid(),
      type: 'app',
      blueprint: blueprint.id,
      position: transform.position,
      quaternion: transform.quaternion,
      scale: [1, 1, 1],
      mover: null,
      uploader: world.network.id,
      pinned: false,
      state: {},
    }
    const app = world.entities.add(data, true)
    const promises = info.assets.map(asset => {
      return world.network.upload(asset.file)
    })
    try {
      await Promise.all(promises)
      app.onUploaded()
    } catch (err) {
      console.error('failed to upload .hyp assets')
      console.error(err)
      app.destroy()
    }
  }

  async addModel(file, transform) {
    const service = this.getService();
    const world = service.getWorld();
    const hash = await hashFile(file)
    const ext = file.name.split('.').pop().toLowerCase()
    const filename = `${hash}.${ext}`
    
    const baseUrl = world.assetsUrl.replace(/\/$/, "");
    const url = `${baseUrl}/${filename}`;
    const uploadPromise = world.network.upload(file);
    const timeoutPromise = new Promise((_resolve, reject) =>
      setTimeout(() => reject(new Error("Upload timed out")), 30000)
    );

    await Promise.race([uploadPromise, timeoutPromise]);
    
    const blueprint = {
      id: uuid(),
      version: 0,
      name: file.name.split('.')[0],
      image: null,
      author: null,
      url: null,
      desc: null,
      model: url,
      script: null,
      props: {},
      preload: false,
      public: false,
      locked: false,
      unique: false,
      disabled: false,
    }

    world.blueprints.add(blueprint, true)
    const data = {
      id: uuid(),
      type: 'app',
      blueprint: blueprint.id,
      position: transform.position,
      quaternion: transform.quaternion,
      scale: [1, 1, 1],
      mover: null,
      uploader: world.network.id,
      pinned: false,
      state: {},
    }
    const app = world.entities.add(data, true)
    app.onUploaded()
  }
  
  entityUpdate(entity) {
    const service = this.getService();
    const world = service.getWorld();
    world.network.send('entityModified', {
        id: entity.data.id,
        position: entity.root.position.toArray(),
        quaternion: entity.root.quaternion.toArray(),
        scale: entity.root.scale.toArray(),
    })
  }

  private getService() {
    return this.runtime.getService<HyperfyService>(HyperfyService.serviceType);
  }
}

} // namespace elizaos
