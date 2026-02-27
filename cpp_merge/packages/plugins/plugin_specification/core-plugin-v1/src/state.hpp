#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_STATE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V1_SRC_STATE_H
#include "core.hpp"
#include "./types.h"
using StateFromTypes = State;
// External dependency removed
using StateV2 = State;

typedef StateFromTypes State;


extern Partial<State> DEFAULT_STATE;
State fromV2State(std::shared_ptr<StateV2> stateV2);

std::shared_ptr<StateV2> toV2State(State state);

#endif
