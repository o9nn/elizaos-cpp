#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_CYPRESS_SUPPORT_COMMANDS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_CYPRESS_SUPPORT_COMMANDS_H
#include "core.h"

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
            virtual std::shared_ptr<Chainable<void>> waitForApp() = 0;
            virtual std::shared_ptr<Chainable<void>> login(string email, string password) = 0;
            virtual std::shared_ptr<Chainable<void>> connectWebSocket() = 0;
            virtual std::shared_ptr<Chainable<void>> cleanupTestData() = 0;
            virtual std::shared_ptr<Chainable<std::shared_ptr<JQuery<std::shared_ptr<HTMLElement>>>>> getByTestId(string testId) = 0;
            virtual std::shared_ptr<Chainable<any>> waitForApi(string alias, double timeout = undefined) = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
