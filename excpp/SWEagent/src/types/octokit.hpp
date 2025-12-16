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

declare module '@octokit/rest' {
  class Octokit {
    constructor(options?: { auth?: string });
    rest: {
      repos: {
        get(params: { owner: string; repo: string }): Promise<{ data: unknown }>;
      };
      pulls: {
        create(params: Record<string, unknown>): Promise<{ data: unknown }>;
      };
      issues: {
        get(params: { owner: string; repo: string; issue_number: number }): Promise<{ data: unknown }>;
      };
    };
  }
}

} // namespace elizaos
