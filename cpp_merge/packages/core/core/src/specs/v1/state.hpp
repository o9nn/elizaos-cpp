#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_STATE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_STATE_H
#include "core.h"
#include "../v2.h"
using StateV2 = State;
#include "./types.h"
using StateFromTypes = State;

typedef StateFromTypes State;


extern Partial<State> DEFAULT_STATE;
State fromV2State(std::shared_ptr<StateV2> stateV2);

std::shared_ptr<StateV2> toV2State(State state);

#endif
