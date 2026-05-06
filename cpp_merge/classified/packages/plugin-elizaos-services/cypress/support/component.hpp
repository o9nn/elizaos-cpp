#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_CYPRESS_SUPPORT_COMPONENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_ELIZAOS_SERVICES_CYPRESS_SUPPORT_COMPONENT_H
#include "core.hpp"
#include "./commands.h"
// External dependency removed
#include "../../src/frontend/index.css.h"
// External dependency removed

namespace global {
    namespace Cypress {
        class Chainable;
    }
}

namespace global {
    namespace Cypress {
        class Chainable : public object, public std::enable_shared_from_this<Chainable> {
        public:
            using std::enable_shared_from_this<Chainable>::shared_from_this;
            virtual std::shared_ptr<Chainable<any>> mount(std::shared_ptr<React::ReactElement> component) = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
