#include "knowledge-helpers.h"

KnowledgeTestHelper::KnowledgeTestHelper(string agentId) {
    this->agentId = agentId;
}

std::shared_ptr<Cypress::Chainable<std::shared_ptr<UploadResponse>>> KnowledgeTestHelper::uploadFile(string fileName, string content, string fileType)
{
    auto file = std::make_shared<File>(array<string>{ content }, fileName, object{
        object::pair{std:("type"), fileType}
    });
    auto formData = std::make_shared<FormData>();
    formData->append(std:("file"), file);
    formData->append(std:("agentId"), this->agentId);
    return cy->request(object{
        object::pair{std:("method"), std:("POST")}, 
        object::pair{std:("url"), string_empty + this->baseUrl + std:("/documents")}, 
        object::pair{std:("body"), formData}, 
        object::pair{std:("headers"), object{
            object::pair{std:("Accept"), std:("application/json")}
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
        object::pair{std:("method"), std:("POST")}, 
        object::pair{std:("url"), string_empty + this->baseUrl + std:("/import")}, 
        object::pair{std:("body"), object{
            object::pair{std:("url"), std:("url")}, 
            object::pair{std:("agentId"), this->agentId}
        }}, 
        object::pair{std:("headers"), object{
            object::pair{std:("Content-Type"), std:("application/json")}
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
        object::pair{std:("method"), std:("GET")}, 
        object::pair{std:("url"), string_empty + this->baseUrl + std:("/documents?agentId=") + this->agentId + string_empty}
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
        object::pair{std:("method"), std:("GET")}, 
        object::pair{std:("url"), string_empty + this->baseUrl + std:("/chunks/") + documentId + std:("?agentId=") + this->agentId + string_empty}
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
        object::pair{std:("method"), std:("GET")}, 
        object::pair{std:("url"), string_empty + this->baseUrl + std:("/search?query=") + encodeURIComponent(query) + std:("&agentId=") + this->agentId + std:("&count=") + count + string_empty}
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
        object::pair{std:("method"), std:("DELETE")}, 
        object::pair{std:("url"), string_empty + this->baseUrl + std:("/documents/") + documentId + std:("?agentId=") + this->agentId + string_empty}
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
                cy->log(std:("Document ") + documentId + std:(" processed with ") + chunks["length"] + std:(" chunks"));
                return any();
            }
            if (Date->now() - startTime > maxWaitTime) {
                throw any(std::make_shared<Error>(std:("Document ") + documentId + std:(" processing timeout after ") + maxWaitTime + std:("ms")));
            }
            cy->wait(1000);
            return checkProcessing();
        }
        )->_catch([=](auto error) mutable
        {
            if (Date->now() - startTime > maxWaitTime) {
                throw any(std::make_shared<Error>(std:("Document ") + documentId + std:(" processing timeout: ") + error["message"] + string_empty));
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
                )), (doc["title"]->includes(std:("test-"))))), (doc["title"]->includes(std:("cypress-"))));
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
                        cy->log(std:("Failed to delete document ") + doc["id"] + std:(", continuing..."));
                        resolve();
                    }
                    );
                }
                );
            }
            );
            return cy->wrap(Promise->all(deletePromises))->then([=]() mutable
            {
                cy->log(std:("Cleaned up ") + testDocs["length"] + std:(" test documents"));
            }
            );
        }
        )->then([=]() mutable
        {
        }
        , [=]() mutable
        {
            cy->log(std:("Failed to retrieve documents for cleanup, continuing..."));
        }
        );
    }
    );
}

std::shared_ptr<Cypress::Chainable<object>> KnowledgeTestHelper::createTestDocuments()
{
    auto testTextContent = std:("This is a comprehensive test document for knowledge base testing. It contains information about artificial intelligence, machine learning, and natural language processing.");
    shared testUrl = std:("https://raw.githubusercontent.com/ai16z/eliza/main/README.md");
    return this->uploadFile(std:("cypress-test-document.txt"), testTextContent)->then([=](auto textResponse) mutable
    {
        return this->uploadFromUrl(testUrl)->then([=](auto urlResponse) mutable
        {
            return object{
                object::pair{std:("textDocId"), textResponse["data"]["id"]}, 
                object::pair{std:("urlDocId"), urlResponse["data"]["id"]}
            };
        }
        );
    }
    );
}

void KnowledgeTestHelper::validateApiResponse(any response, boolean expectedSuccessStatus)
{
    expect(response)->to->have->property(std:("success"), expectedSuccessStatus);
    if (expectedSuccessStatus) {
        expect(response)->to->have->property(std:("data"));
    } else {
        expect(response)->to->have->property(std:("error"));
        expect(response["error"])->to->have->property(std:("message"));
    }
}

std::shared_ptr<Cypress::Chainable<array<string>>> KnowledgeTestHelper::testFileTypes()
{
    auto testFiles = array<object>{ object{
        object::pair{std:("name"), std:("test.txt")}, 
        object::pair{std:("content"), std:("Plain text content")}, 
        object::pair{std:("type"), std:("text/plain")}
    }, object{
        object::pair{std:("name"), std:("test.md")}, 
        object::pair{std:("content"), std:("# Markdown Content\
\
This is a test markdown file.")}, 
        object::pair{std:("type"), std:("text/markdown")}
    }, object{
        object::pair{std:("name"), std:("test.json")}, 
        object::pair{std:("content"), std:("{"key": "value", "test": true}")}, 
        object::pair{std:("type"), std:("application/json")}
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
    Cypress->Commands->add(std:("knowledgeHelper"), [=](auto agentId = undefined) mutable
    {
        return cy->wrap(std::make_shared<KnowledgeTestHelper>(agentId));
    }
    );
    Cypress->Commands->add(std:("uploadKnowledgeFile"), [=](auto fileName, auto content, auto fileType = undefined) mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->uploadFile(fileName, content, fileType);
    }
    );
    Cypress->Commands->add(std:("searchKnowledge"), [=](auto query, auto count = undefined) mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->search(query, count);
    }
    );
    Cypress->Commands->add(std:("deleteKnowledgeDocument"), [=](auto documentId) mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->deleteDocument(documentId);
    }
    );
    Cypress->Commands->add(std:("cleanupKnowledgeTests"), [=]() mutable
    {
        auto helper = std::make_shared<KnowledgeTestHelper>();
        return helper->cleanupTestDocuments(array<string>{ std:("test-"), std:("cypress-") });
    }
    );
}

MAIN
