#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_DEV_UTILS_FILE_WATCHER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_DEV_UTILS_FILE_WATCHER_H
#include "core.hpp"
#include "chokidar.hpp"
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
#include "../types.h"

extern std::shared_ptr<WatcherConfig> DEFAULT_WATCHER_CONFIG;
array<string> findTsFiles(string dir, string watchDir);

std::shared_ptr<Promise<void>> watchDirectory(string dir, std::function<void()> onChange, Partial<std::shared_ptr<WatcherConfig>> config = object{});

std::function<void()> createDebouncedHandler(std::function<void()> handler, double delay = 300);

#endif
