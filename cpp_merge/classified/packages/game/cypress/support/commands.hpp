#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_CYPRESS_SUPPORT_COMMANDS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_CYPRESS_SUPPORT_COMMANDS_H
#include "core.h"
#include "./knowledge-helpers.h"
#include "./database-helpers.h"

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
            virtual std::shared_ptr<Chainable<void>> setupTestEnvironment() = 0;
            virtual std::shared_ptr<Chainable<void>> waitForBackend(double timeout = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> clearTestData() = 0;
            virtual std::shared_ptr<Chainable<void>> seedTestData() = 0;
            virtual std::shared_ptr<Chainable<void>> bypassBoot() = 0;
            virtual std::shared_ptr<Chainable<void>> setupApiKey(P0 provider, string key) = 0;
            virtual std::shared_ptr<Chainable<boolean>> elementExists(string selector) = 0;
            virtual std::shared_ptr<Chainable<void>> safeClick(string selector) = 0;
            virtual std::shared_ptr<Chainable<any>> uploadKnowledgeFile(string fileName, string content, string fileType = undefined) = 0;
            virtual std::shared_ptr<Chainable<array<any>>> searchKnowledge(string query, double count = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> deleteKnowledgeDocument(string documentId) = 0;
            virtual std::shared_ptr<Chainable<void>> cleanupKnowledgeTests() = 0;
            virtual std::shared_ptr<Chainable<void>> waitForDocumentProcessing(string documentId, double timeout = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> toggleCapability(string capability) = 0;
            virtual std::shared_ptr<Chainable<boolean>> getCapabilityStatus(string capability) = 0;
            virtual std::shared_ptr<Chainable<std::shared_ptr<Cypress::Response<any>>>> sendMessage(object message) = 0;
            virtual std::shared_ptr<Chainable<string>> authenticateDb(string username = undefined, string password = undefined) = 0;
            virtual std::shared_ptr<Chainable<array<any>>> getDbTables() = 0;
            virtual std::shared_ptr<Chainable<void>> cleanupDbTestRecords(string tableName, string searchPattern = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> waitForBackend() = 0;
            virtual std::shared_ptr<Chainable<void>> waitForElizaClient() = 0;
            virtual std::shared_ptr<Chainable<any>> sendMessage(object message) = 0;
            virtual std::shared_ptr<Chainable<array<any>>> searchKnowledge(string query, double count = undefined) = 0;
            virtual std::shared_ptr<Chainable<void>> deleteKnowledgeDocument(string documentId) = 0;
            virtual std::shared_ptr<Chainable<void>> cleanupKnowledgeTests() = 0;
            virtual std::shared_ptr<Chainable<void>> waitForDocumentProcessing(string documentId, double timeout = undefined) = 0;
        };

    }
}
extern std::shared_ptr<KnowledgeTestHelper> knowledgeHelper;
extern std::shared_ptr<DatabaseTestHelper> dbHelper;
namespace global {
    namespace Cypress {
    }
}
#endif
