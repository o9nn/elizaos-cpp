#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_STATE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_STATE_H
#include "core.h"

typedef any StateValue;

typedef array<StateValue> StateArray;

class State;
class StateObject;
class EnhancedState;

class State : public object, public std::enable_shared_from_this<State> {
public:
    using std::enable_shared_from_this<State>::shared_from_this;
    object values;

    object data;

    string text;
};

class StateObject : public object, public std::enable_shared_from_this<StateObject> {
public:
    using std::enable_shared_from_this<StateObject>::shared_from_this;
};

class EnhancedState : public object, public std::enable_shared_from_this<EnhancedState> {
public:
    using std::enable_shared_from_this<EnhancedState>::shared_from_this;
    std::shared_ptr<StateObject> values;

    std::shared_ptr<StateObject> data;

    string text;
};

#endif
