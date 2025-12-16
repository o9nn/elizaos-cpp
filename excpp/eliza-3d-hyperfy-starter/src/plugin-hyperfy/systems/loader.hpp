#include ".hyperfy/src/core/extras/createEmoteFactory.js.hpp"
#include ".hyperfy/src/core/extras/createNode.js.hpp"
#include ".hyperfy/src/core/extras/glbToNodes.js.hpp"
#include ".hyperfy/src/core/libs/gltfloader/GLTFLoader.js.hpp"
#include ".hyperfy/src/core/systems/System.js.hpp"
#include ".managers/puppeteer-manager.js.hpp"
#include ".utils.js.hpp"
#include "avatar.js.hpp"
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



// 
// --- Mock Browser Environment for Loaders ---
// These might need adjustment based on GLTFLoader/VRMLoaderPlugin requirements
  // Mock URL if not globally available or needs specific behavior
  // globalThis.URL = URL; // Usually available in modern Node

  // Mock self if needed by any dependency
  // globalThis.self = globalThis;

  // Mock window minimally

  // Mock document minimally for GLTFLoader
        // Basic mock for image elements if texture loading is attempted (though we aim to bypass it)
      // Default mock for other elements like canvas
      // Basic canvas mock if needed
    // Add more document mocks if loader errors indicate they are needed

  // Polyfill fetch if using older Node version without native fetch
  // globalThis.fetch = fetch;
// --- End Mocks ---

class AgentLoader extends System {
  promises: Map<any, any>;
  results: Map<any, any>;
  gltfLoader: GLTFLoader;
  dummyScene: any;
  constructor(world) {
    super(world);
    this.promises = new Map();
    this.results = new Map();
    this.gltfLoader = new GLTFLoader();

    // --- Dummy Scene for Hooks ---
    // Create one dummy object to act as the scene target for all avatar loads
    this.dummyScene = new THREE.Object3D();
    this.dummyScene.name = "AgentLoaderDummyScene";
    // -----------------------------

    // --- Attempt to register VRM plugin ---
    // try {
    //     this.gltfLoader.register(parser => new VRMLoaderPlugin(parser, {
    //         autoUpdateHumanBones: false
    //     }));
    //     console.log("[AgentLoader] VRMLoaderPlugin registered.");
    // } catch (vrmError) {
    //     console.error("[AgentLoader] Warning: Failed to register VRMLoaderPlugin. VRM-specific features might be unavailable.", vrmError);
    // }
    // ---------------------------------------
  }

  // --- Dummy Preload Methods ---
    // No-op for agent
    // No-op for agent
    // ClientNetwork calls this after snapshot, so it must exist.
  // ---------------------------

  // --- Basic Cache Handling ---
  // ... (has, get methods remain the same) ...
  // ---------------------------

        // TEMP WORKAROUND: Only load scripts that do not create video, UI, or image elements.
        // TODO: Replace this with a proper script validation system.


} // namespace elizaos
