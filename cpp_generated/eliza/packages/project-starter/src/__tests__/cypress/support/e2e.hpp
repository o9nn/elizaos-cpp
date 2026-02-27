#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___CYPRESS_SUPPORT_E2E_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___CYPRESS_SUPPORT_E2E_H
#include "core.h"
#include "./commands.h"
#include "@testing-library/cypress/add-commands.h"

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
            virtual std::shared_ptr<Chainable<void>> login(string username = undefined, string password = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> waitForApp() = 0;
            virtual std::shared_ptr<Chainable<void>> navigateToAgent(string agentId = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> sendChatMessage(string message) = 0;
            virtual std::shared_ptr<Chainable<void>> clearAppData() = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
