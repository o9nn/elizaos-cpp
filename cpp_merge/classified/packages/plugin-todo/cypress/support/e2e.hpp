#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_CYPRESS_SUPPORT_E2E_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-TODO_CYPRESS_SUPPORT_E2E_H
#include "core.h"
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
            virtual std::shared_ptr<Chainable<void>> uploadKnowledgeFile(string fileName, string content, string mimeType = undefined) = 0;
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
