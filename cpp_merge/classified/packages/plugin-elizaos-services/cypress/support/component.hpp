#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_CYPRESS_SUPPORT_COMPONENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_CYPRESS_SUPPORT_COMPONENT_H
#include "core.h"
#include "./commands.h"
#include "@testing-library/cypress/add-commands.h"
#include "../../src/frontend/index.css.h"
#include "@cypress/react.h"

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
