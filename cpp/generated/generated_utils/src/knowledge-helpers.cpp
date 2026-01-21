#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/support/knowledge-helpers.h"

KnowledgeTestHelper::KnowledgeTestHelper(string agentId) {
    this->agentId = agentId;
}

std::shared_ptr<Cypress::Chainable<std::shared_ptr<UploadResponse>>> KnowledgeTestHelper::uploadFile(string fileName, string content, string fileType)
{
    auto file = std::make_shared<File>(array<string>{ content }, fileName, object{
        object::pair{std::string("type"), fileType}
    });
    auto formData = std::make_shared<FormData>();
    formData->append(std::string("file"), file);
    formData->append(std::string("agentId"), this->agentId);
    return cy->request(object{
        object::pair{std::string("method"), std::string("POST")}, 
        object::pair{std::string("url"), string_empty + this->baseUrl + std::string("/documents")}, 
        object::pair{std::string("body"), formData}, 
        object::pair{std::string("headers"), object{
            object::pair{std::string("Accept"), std::string("application/json")}
        }}
    })->then([=](auto response) mutable
    {
        expect(response["status"])->to->eq(200);
        return as<std::shared_ptr<UploadResponse>>(response["body"]);
    }
    );
}

std::shared_ptr<Cypress::Chainable<std::shared_ptr<UploadResponse>>> KnowledgeTestHelper::uploadFromUrl(string url)
{
    return cy->request(object{
        object::pair{std::string("method"), std::string("POST")}, 
        object::pair{std::string("url"), string_empty + this->baseUrl + std::string("/import")}, 
        object::pair{std::string("body"), object{
            object::pair{std::string("url"), std::string("url")}, 
            object::pair{std::string("agentId"), this->agentId}
        }}, 
        object::pair{std::string("headers"), object{
            object::pair{std::string("Content-Type"), std::string("application/json")}
        }}
    })->then([=](auto response) mutable
    {
        expect(response["status"])->to->eq(200);
        return as<std::shared_ptr<UploadResponse>>(response["body"]);
    }
    );
}

std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeDocument>>>> KnowledgeTestHelper::getDocuments()
{
    return cy->request(object{
        object::pair{std::string("method"), std::string("GET")}, 
        object::pair{std::string("url"), string_empty + this->baseUrl + std::string("/documents?agentId=") + this->agentId + string_empty}
    })->then([=](auto response) mutable
    {
        expect(response["status"])->to->eq(200);
        expect(response["body"]["success"])->to->be->true;
        return as<array<std::shared_ptr<KnowledgeDocument>>>(response["body"]["data"]);
    }
    );
}

std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeChunk>>>> KnowledgeTestHelper::getDocumentChunks(string documentId)
{
    return cy->request(object{
        object::pair{std::string("method"), std::string("GET")}, 
        object::pair{std::string("url"), string_empty + this->baseUrl + std::string("/chunks/") + documentId + std::string("?agentId=") + this->agentId + string_empty}
    })->then([=](auto response) mutable
    {
        expect(response["status"])->to->eq(200);
        expect(response["body"]["success"])->to->be->true;
        return as<array<std::shared_ptr<KnowledgeChunk>>>(response["body"]["data"]);
    }
    );
}

std::shared_ptr<Cypress::Chainable<array<std::shared_ptr<KnowledgeSearchResult>>>> KnowledgeTestHelper::search(string query, double count)
{
    return cy->request(object{
        object::pair{std::string("method"), std::string("GET")}, 
        object::pair{std::string("url"), string_empty + this->baseUrl + std::string("/search?query=") + encodeURIComponent(query) + std::string("&agentId=") + this->agentId + std::string("&count=") + count + string_empty}
    })->then([=](auto response) mutable
    {
        expect(response["status"])->to->eq(200);
        expect(response["body"]["success"])->to->be->true;
        return as<array<std::shared_ptr<KnowledgeSearchResult>>>(response["body"]["data"]);
    }
    );
}

std::shared_ptr<Cypress::Chainable<void>> KnowledgeTestHelper::deleteDocument(string documentId)
{
    return cy->request(object{
        object::pair{std::string("method"), std::string("DELETE")}, 
        object::pair{std::string("url"), string_empty + this->baseUrl + std::string("/documents/") + documentId + std::string("?agentId=") + this->agentId + string_empty}
    })->then([=](auto response) mutable
    {
        expect(response["status"])->to->eq(204);
    }
    );
}

std::shared_ptr<Cypress::Chainable<void>> KnowledgeTestHelper::waitForDocumentProcessing(string documentId, double maxWaitTime)
{
    shared startTime = Date->now();
    shared checkProcessing = [=]() mutable
    {
        return this->getDocumentChunks(documentId)->then([=](auto chunks) mutable
        {
            if (chunks["length"] > 0) {
                cy->log(std::string("Document ") + documentId + std::string(" processed with ") + chunks["length"] + std::string(" chunks"));
                return any();
            }
            if (Date->now() - startTime > maxWaitTime) {
                throw any(std::make_shared<Error>(std::string("Document ") + documentId + std::string(" processing timeout after ") + maxWaitTime + std::string("ms")));
            }
            cy->wait(1000);
            return checkProcessing();
        }
        )->_catch([=](auto error) mutable
        {
            if (Date->now() - startTime > maxWaitTime) {
                throw any(std::make_shared<Error>(std::string("Document ") + documentId + std::string(" processing timeout: ") + error["message"] + string_empty));
            }
            cy->wait(1000);
            return checkProcessing();
        }
        );
    };
    return checkProcessing();
}

std::shared_ptr<Cypress::Chainable<boolean>> KnowledgeTestHelper::verifyDocumentExists(string documentId)
{
    return this->getDocumentChunks(documentId)->then([=](auto chunks) mutable
    {
        return chunks["length"] > 0;
    }
    )->_catch([=]() mutable
    {
        return false;
    }
    );
}

std::shared_ptr<Cypress::Chainable<void>> KnowledgeTestHelper::cleanupTestDocuments(array<string> testFileNames)
{
    return cy->wrap(nullptr)->then([=]() mutable
    {
        return this->getDocuments()->then([=](auto documents) mutable
        {
            shared testDocs = documents["filter"]([=](auto doc) mutable
            {
                return OR((OR((testFileNames->some([=](auto name) mutable
                {
                    return doc["title"]->includes(name);
                }
                )), (doc["title"]->includes(std::string("test-"))))), (doc["title"]->includes(std::string("cypress-"))));
            }
            );
            auto deletePromises = testDocs["map"]([=](auto doc) mutable
            {
                return std::make_shared<Cypress->Promise>([=](auto resolve) mutable
                {
                    this->deleteDocument(doc["id"])->then([=]() mutable
                    {
                        return resolve();
                    }
                    )->_catch([=]() mutable
                    {
                        cy->log(std::string("Failed to delete document ") + doc["id"] + std::string(", continuing..."));
                        resolve();
                    }
                    );
                }
                );
            }
            );
            return cy->wrap(Promise->all(deletePromises))->then([=]() mutable
            {
                cy->log(std::string("Cleaned up ") + testDocs["length"] + std::string(" test documents"));
            }
            );
        }
        )->then([=]() mutable
        {
        }
        , [=]() mutable
        {
            cy->log(std::string("Failed to retrieve documents for cleanup, continuing..."));
        }
        );
    }
    );
}

std::shared_ptr<Cypress::Chainable<object>> KnowledgeTestHelper::createTestDocuments()
{
    auto testTextContent = std::string("This is a comprehensive test document for knowledge base testing. It contains information about artificial intelligence, machine learning, and natural language processing.");
    shared testUrl = std::string("https://raw.githubusercontent.com/ai16z/eliza/main/README.md");
    return this->uploadFile(std::string("cypress-test-document.txt"), testTextContent)->then([=](auto textResponse) mutable
    {
        return this->uploadFromUrl(testUrl)->then([=](auto urlResponse) mutable
        {
            return object{
                object::pair{std::string("textDocId"), textResponse["data"]["id"]}, 
                object::pair{std::string("urlDocId"), urlResponse["data"]["id"]}
            };
        }
        );
    }
    );
}

void KnowledgeTestHelper::validateApiResponse(any response, boolean expectedSuccessStatus)
{
    expect(response)->to->have->property(std::string("success"), expectedSuccessStatus);
    if (expectedSuccessStatus) {
        expect(response)->to->have->property(std::string("data"));
    } else {
        expect(response)->to->have->property(std::string("error"));
        expect(response["error"])->to->have->property(std::string("message"));
    }
}

std::shared_ptr<Cypress::Chainable<array<string>>> KnowledgeTestHelper::testFileTypes()
{
    auto testFiles = array<object>{ object{
        object::pair{std::string("name"), std::string("test.txt")}, 
        object::pair{std::string("content"), std::string("Plain text content")}, 
        object::pair{std::string("type"), std::string("text/plain")}
    }, object{
        object::pair{std::string("name"), std::string("test.md")}, 
        object::pair{std::string("content"), std::string("# Markdown Content\
\
This is a test markdown file.")}, 
        object::pair{std::string("type"), std::string("text/markdown")}
    }, object{
        object::pair{std::string("name"), std::string("test.json")}, 
        object::pair{std::string("content"), std::string("{"key": "value", "test": true}")}, 
        object::pair{std::string("type"), std::string("application/json")}
    } };
    auto uploadPromises = testFiles->map([=](auto file) mutable
    {
        return this->uploadFile(file["name"], file["content"], file["type"])->then([=](auto response) mutable
        {
            return response["data"]->id;
        }
        );
    }
    );
    return cy->wrap(Promise->all(uploadPromises));
}

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std::string("knowledgeHelper"), [=](auto agentId = undefined) mutable
    {
        return cy->wrap(std::make_shared<KnowledgeTestHelper>(agentId));
    }
    );
    Cypress->Commands->add(std::string("uploadKnowledgeFile"), [=](auto fileName, auto content, auto fileType = undefined) mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->uploadFile(fileName, content, fileType);
    }
    );
    Cypress->Commands->add(std::string("searchKnowledge"), [=](auto query, auto count = undefined) mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->search(query, count);
    }
    );
    Cypress->Commands->add(std::string("deleteKnowledgeDocument"), [=](auto documentId) mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->deleteDocument(documentId);
    }
    );
    Cypress->Commands->add(std::string("cleanupKnowledgeTests"), [=]() mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->cleanupTestDocuments(array<string>{ std::string("test-"), std::string("cypress-") });
    }
    );
}

MAIN
