#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_AGENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_AGENT_H
#include "core.h"
#include "dotenv.h"
using dotenv = _default;
#include "@elizaos/core.h"
#include "./plugin-otc-desk.h"
#include "./plugin-groq.h"
#include "@elizaos/plugin-sql.h"
using sqlPlugin = plugin;

extern std::shared_ptr<Character> character;
extern object agent;
#endif
