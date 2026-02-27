#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_CYPRESS_SUPPORT_E2E_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_CYPRESS_SUPPORT_E2E_H
#include "core.hpp"
#include "./commands.h"

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
            virtual std::shared_ptr<Chainable<void>> visitKnowledgePanel() = 0;
            virtual std::shared_ptr<Chainable<void>> uploadKnowledgeFile(string fileName, string content, string mimeType = std::nullopt) = 0;
            virtual std::shared_ptr<Chainable<void>> searchKnowledge(string query) = 0;
            virtual std::shared_ptr<Chainable<void>> deleteDocument(string title) = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
