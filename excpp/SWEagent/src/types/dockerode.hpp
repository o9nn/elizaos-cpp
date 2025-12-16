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

declare module 'dockerode' {
  default class Dockerode {
    constructor(options?: Record<string, unknown>);
    createContainer(options: Record<string, unknown>): Promise<unknown>;
    getContainer(id: string): unknown;
    listContainers(options?: Record<string, unknown>): Promise<unknown[]>;
    listImages(options?: Record<string, unknown>): Promise<unknown[]>;
    pull(image: string, options?: Record<string, unknown>): Promise<unknown>;
    modem: unknown;
  }
}

} // namespace elizaos
