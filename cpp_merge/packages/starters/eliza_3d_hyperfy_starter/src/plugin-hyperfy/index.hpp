#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_INDEX_H
#include "core.hpp"
#include "ses.hpp"
// External dependency removed
// External dependency removed
#include "./service.h"
#include "zod.hpp"
#include "./actions/goto.h"
#include "./actions/use.h"
#include "./actions/unuse.h"
#include "./actions/stop.h"
#include "./actions/walk_randomly.h"
#include "./actions/ambient.h"
#include "./actions/perception.h"
#include "./actions/build.h"
#include "./actions/reply.h"
#include "./actions/ignore.h"
#include "./providers/world.h"
#include "./providers/emote.h"
#include "./providers/actions.h"
#include "./providers/character.h"
#include "./events.h"

extern string HYPERFY_WS_URL;
extern any hyperfyPluginConfigSchema;
extern std::shared_ptr<Plugin> hyperfyPlugin;
#endif
