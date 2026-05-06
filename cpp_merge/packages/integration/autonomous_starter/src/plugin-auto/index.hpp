#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_AUTO_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_AUTO_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./events.h"
#include "./service.h"
// Using alias removed (invalid transpilation)
#include "./types.h"
#include "./messageFeed.h"
#include "./reflect.h"

namespace global {
    namespace NodeJS {
        class ProcessEnv;
    }
}

namespace global {
    namespace NodeJS {
        class ProcessEnv : public object, public std::enable_shared_from_this<ProcessEnv> {
        public:
            using std::enable_shared_from_this<ProcessEnv>::shared_from_this;
            string AUTONOMOUS_LOOP_INTERVAL;
        };

    }
}
extern std::shared_ptr<Plugin> autoPlugin;
namespace global {
    namespace NodeJS {
    }
}
#endif
