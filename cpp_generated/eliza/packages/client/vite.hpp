#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_VITE_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_VITE_CONFIG_H
#include "core.h"
#include "@vitejs/plugin-react-swc.h"
using react = _default;
#include "node:path.h"
using path = _default;
#include "node:fs.h"
using fs = _default;
#include "vite.h"
#include "vite-plugin-compression.h"
using viteCompression = _default;
#include "@tailwindcss/vite.h"
using tailwindcss = _default;
#include "vite-plugin-node-polyfills.h"

class CustomUserConfig;

class CustomUserConfig : public UserConfig, public std::enable_shared_from_this<CustomUserConfig> {
public:
    using std::enable_shared_from_this<CustomUserConfig>::shared_from_this;
};

extern std::function<string()> getVersionAndWriteInfo;
extern std::function<any()> versionPlugin;
#endif
