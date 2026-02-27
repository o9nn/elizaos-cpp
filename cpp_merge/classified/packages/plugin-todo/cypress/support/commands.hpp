#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_CYPRESS_SUPPORT_COMMANDS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_CYPRESS_SUPPORT_COMMANDS_H
#include "core.hpp"

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
            virtual std::shared_ptr<Chainable<void>> uploadFile(string selector, string fileName, string fileContent, string mimeType = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> waitForApi(string alias, double timeout = undefined) = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
