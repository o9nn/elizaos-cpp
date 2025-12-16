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

// Registry configuration constants - centralized for maintainability
const REGISTRY_ORG = 'elizaos-plugins';
const REGISTRY_REPO_NAME = 'registry';
const REGISTRY_REPO = `${REGISTRY_ORG}/${REGISTRY_REPO_NAME}`;

// Derived URLs
const REGISTRY_URL = `https://raw.githubusercontent.com/${REGISTRY_REPO}/refs/heads/main/index.json`;
const REGISTRY_GITHUB_URL = `https://github.com/${REGISTRY_REPO}`;

// Legacy exports for backward compatibility
const RAW_REGISTRY_URL = REGISTRY_URL;

} // namespace elizaos
