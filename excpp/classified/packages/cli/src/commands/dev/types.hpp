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

import type { Subprocess } from 'bun';

/**
 * Dev command types and interfaces
 */

/**
 * Development server options
 */
struct DevOptions {
    std::optional<double> port;
    std::optional<std::string> configDir;
    std::optional<std::vector<std::string>> character;
    std::optional<bool> configure;
};


/**
 * Server process management interface
 */
struct ServerProcess {
    Subprocess | null process;
};


/**
 * File watcher configuration
 */
struct WatcherConfig {
    std::vector<std::string> ignored;
    bool ignoreInitial;
    bool persistent;
    bool followSymlinks;
    double depth;
    bool usePolling;
    double interval;
};


/**
 * Development context information
 */
struct DevContext {
    std::string directory;
    import('@/src/utils/directory-detection').DirectoryInfo directoryType;
    std::string watchDirectory;
    bool buildRequired;
};


/**
 * File change event
 */
struct FileChangeEvent {
    'add' | 'change' | 'unlink' type;
    std::string path;
    std::optional<import('node:fs').Stats> stats;
};


/**
 * Build result
 */
struct BuildResult {
    bool success;
    double duration;
    std::optional<Error> error;
};


} // namespace elizaos
