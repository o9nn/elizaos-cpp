#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_DEV_UTILS_FILE-WATCHER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_DEV_UTILS_FILE-WATCHER_H
#include "core.h"
#include "chokidar.h"
using chokidar = _default;
#include "node:fs.h"
#include "node:path.h"
using path = _default;
#include "../types.h"

extern std::shared_ptr<WatcherConfig> DEFAULT_WATCHER_CONFIG;
array<string> findTsFiles(string dir, string watchDir);

std::shared_ptr<Promise<void>> watchDirectory(string dir, std::function<void()> onChange, Partial<std::shared_ptr<WatcherConfig>> config = object{});

std::function<void()> createDebouncedHandler(std::function<void()> handler, double delay = 300);

#endif
