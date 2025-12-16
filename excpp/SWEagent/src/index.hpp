#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Main exports TypeScript implementation
 */

// Core types
* from './types';
* from './exceptions';

// Agent module - selective exports to avoid conflicts

// Environment module

// Utils module - selective exports

// Tools module - selective exports

// Run module - selective exports

// Version info

/**
 * Get agent commit hash
 */
std::string getAgentCommitHash();

/**
 * Get REX commit hash
 */
std::string getRexCommitHash();

/**
 * Get REX version
 */
std::string getRexVersion();

/**
 * Get agent version info
 */
std::string getAgentVersionInfo();

} // namespace elizaos
