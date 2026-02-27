#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_VITE_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_VITE_CONFIG_H
#include "core.hpp"
// External dependency removed
// Using alias removed (invalid transpilation)
#include "node:path.h"
// Using alias removed (invalid transpilation)
#include "node:fs.h"
// Using alias removed (invalid transpilation)
// Self-include removed: vite.hpp
#include "vite-plugin-compression.h"
// Using alias removed (invalid transpilation)
// External dependency removed
// Using alias removed (invalid transpilation)
#include "vite-plugin-node-polyfills.h"

class CustomUserConfig;

class CustomUserConfig : public UserConfig, public std::enable_shared_from_this<CustomUserConfig> {
public:
    using std::enable_shared_from_this<CustomUserConfig>::shared_from_this;
};

extern std::function<string()> getVersionAndWriteInfo;
extern std::function<any()> versionPlugin;
#endif
