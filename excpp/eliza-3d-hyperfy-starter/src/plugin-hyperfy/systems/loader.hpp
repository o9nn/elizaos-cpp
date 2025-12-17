#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".hyperfy/src/core/extras/createEmoteFactory.js.hpp"
#include ".hyperfy/src/core/extras/createNode.js.hpp"
#include ".hyperfy/src/core/extras/glbToNodes.js.hpp"
#include ".hyperfy/src/core/libs/gltfloader/GLTFLoader.js.hpp"
#include ".hyperfy/src/core/systems/System.js.hpp"
#include ".managers/puppeteer-manager.js.hpp"
#include ".utils.js.hpp"
#include "avatar.js.hpp"

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

class AgentLoader {
public:
    AgentLoader(auto world);
    void preload(auto type, auto url);
    void execPreload();
    void has(auto type, auto url);
    void get(auto type, auto url);
    void load(auto type, auto url);
    void parseGLB(const std::string& type, const std::string& key, const std::string& url);

private:
    std::unordered_map<std::any, std::any> promises_;
    std::unordered_map<std::any, std::any> results_;
    GLTFLoader gltfLoader_;
    std::any dummyScene_;
};


} // namespace elizaos
