#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_CYPRESS_SUPPORT_KNOWLEDGE-HELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_CYPRESS_SUPPORT_KNOWLEDGE-HELPERS_H
#include "core.h"

class KnowledgeDocument;
class KnowledgeChunk;
class KnowledgeSearchResult;
class UploadResponse;
class KnowledgeTestHelper;
namespace global {
    namespace Cypress {
        class Chainable;
    }
}

class KnowledgeDocument : public object, public std::enable_shared_from_this<KnowledgeDocument> {
public:
    using std::enable_shared_from_this<KnowledgeDocument>::shared_from_this;
    string id;

    string title;

    string description;

    string createdAt;

    Record<string, any> metadata;
};

class KnowledgeChunk : public object, public std::enable_shared_from_this<KnowledgeChunk> {
public:
    using std::enable_shared_from_this<KnowledgeChunk>::shared_from_this;
    string id;

    object content;

    array<double> embedding;
};

class KnowledgeSearchResult : public object, public std::enable_shared_from_this<KnowledgeSearchResult> {
public:
    using std::enable_shared_from_this<KnowledgeSearchResult>::shared_from_this;
    object content;

    double similarity;
};

class UploadResponse : public object, public std::enable_shared_from_this<UploadResponse> {
public:
    using std::enable_shared_from_this<UploadResponse>::shared_from_this;
    boolean success;

    object data;

    object error;
};

class KnowledgeTestHelper : public object, public std::enable_shared_from_this<KnowledgeTestHelper> {
public:
    using std::enable_shared_from_this<KnowledgeTestHelper>::shared_from_this;
    string baseUrl = std::string("http://localhost:7777/knowledge");

    string agentId;

    KnowledgeTestHelper(string agentId = std::string("00000000-0000-0000-0000-000000000001"));
    virtual std::shared_ptr<Cypress::Chainable<std::shared_ptr<UploadResponse>>> uploadFile(string fileName, string content, string fileType = std::string("text/plain"));
    virtual std::shared_ptr<Cypress::Chainable<std::shared_ptr<UploadResponse>>> uploadFromUrl(string url);
    virtual std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeDocument>>>> getDocuments();
    virtual std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeChunk>>>> getDocumentChunks(string documentId);
    virtual std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeSearchResult>>>> search(string query, double count = 10);
    virtual std::shared_ptr<Cypress::Chainable<void>> deleteDocument(string documentId);
    virtual std::shared_ptr<Cypress::Chainable<void>> waitForDocumentProcessing(string documentId, double maxWaitTime = 30000);
    virtual std::shared_ptr<Cypress::Chainable<boolean>> verifyDocumentExists(string documentId);
    virtual std::shared_ptr<Cypress::Chainable<void>> cleanupTestDocuments(array<string> testFileNames = array<string>());
    virtual std::shared_ptr<Cypress::Chainable<object>> createTestDocuments();
    virtual void validateApiResponse(any response, boolean expectedSuccessStatus = true);
    virtual std::shared_ptr<Cypress::Chainable<array<string>>> testFileTypes();
};

namespace global {
    namespace Cypress {
        class Chainable : public object, public std::enable_shared_from_this<Chainable> {
        public:
            using std::enable_shared_from_this<Chainable>::shared_from_this;
            virtual std::shared_ptr<Cypress::Chainable<std::shared_ptr<KnowledgeTestHelper>>> knowledgeHelper(string agentId = undefined) = 0;
            virtual std::shared_ptr<Cypress::Chainable<std::shared_ptr<UploadResponse>>> uploadKnowledgeFile(string fileName, string content, string fileType = undefined) = 0;
            virtual std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeSearchResult>>>> searchKnowledge(string query, double count = undefined) = 0;
            virtual std::shared_ptr<Cypress::Chainable<void>> deleteKnowledgeDocument(string documentId) = 0;
            virtual std::shared_ptr<Cypress::Chainable<void>> cleanupKnowledgeTests() = 0;
        };

    }
}
namespace global {
    namespace Cypress {
    }
}
#endif
