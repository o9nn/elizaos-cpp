#include "....utils/discordHelper.hpp"
#include "elizaos/core.hpp"
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



using PlatformHandlerParams = {

    await logModerationMemory(runtime, message, 'discord', `${member.displayName} was timed out.`);
    await callback({ text: `Failed to timeout ${member.displayName}`, source: 'discord' });

    await callback({ text: `Failed to restrict user`, source: 'telegram' });


} // namespace elizaos
