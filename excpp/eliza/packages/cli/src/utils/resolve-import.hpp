#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 *
 * @param {Pick<ConfigLoaderSuccessResult, "absoluteBaseUrl" | "paths">} config - The configuration object containing the absolute base URL and paths mappings.
 * @returns {string|null} - The resolved absolute path if a match is found, otherwise null.
 */
std::future<void> resolveImport(const std::string& importPath, Pick<ConfigLoaderSuccessResult config, auto 'absoluteBaseUrl' | 'paths'>);

} // namespace elizaos
