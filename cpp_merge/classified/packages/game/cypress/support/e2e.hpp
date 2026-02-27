#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_GAME_CYPRESS_SUPPORT_E2E_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_GAME_CYPRESS_SUPPORT_E2E_H
#include "core.hpp"
#include "./commands.h"
#include "./test-commands.h"

namespace global {
    namespace Cypress {
        class Chainable;
        class Assertion;
    }
}

namespace global {
    namespace Cypress {
        class Chainable : public object, public std::enable_shared_from_this<Chainable> {
        public:
            using std::enable_shared_from_this<Chainable>::shared_from_this;
            virtual std::shared_ptr<Chainable<void>> waitForAgentReady() = 0;
            virtual std::shared_ptr<Chainable<void>> sendMessageAndWait(string message) = 0;
            virtual std::shared_ptr<Chainable<void>> toggleCapability(string capability, boolean expectedState) = 0;
            virtual std::shared_ptr<Chainable<void>> verifyErrorHandling(string errorType) = 0;
            virtual std::shared_ptr<Chainable<void>> resetAppState() = 0;
            virtual std::shared_ptr<Chainable<void>> enableTestingMode() = 0;
            virtual std::shared_ptr<Chainable<void>> simulateNetworkCondition(P0 condition) = 0;
            virtual std::shared_ptr<Chainable<void>> checkA11y() = 0;
            virtual std::shared_ptr<Chainable<void>> measurePerformance(string testName) = 0;
            virtual std::shared_ptr<Chainable<void>> testResponsive() = 0;
            virtual std::shared_ptr<Chainable<void>> testKeyboardNavigation() = 0;
            virtual std::shared_ptr<Chainable<void>> verifyTextOnScreen(string expectedText) = 0;
            virtual std::shared_ptr<Chainable<void>> cleanupTestData() = 0;
            virtual std::shared_ptr<Chainable<std::shared_ptr<Element>>> tab(object options = std::nullopt) = 0;
        };

        class Assertion : public object, public std::enable_shared_from_this<Assertion> {
        public:
            using std::enable_shared_from_this<Assertion>::shared_from_this;
            virtual std::shared_ptr<Chainable<std::shared_ptr<Element>>> containOneOf(array<string> list) = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
