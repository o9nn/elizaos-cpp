#include "commands.hpp"

namespace global {
    namespace Cypress {
    }
}
std::shared_ptr<KnowledgeTestHelper> knowledgeHelper = std::make_shared<KnowledgeTestHelper>();
std::shared_ptr<DatabaseTestHelper> dbHelper = std::make_shared<DatabaseTestHelper>();

void Main(void)
{
    Cypress->Commands->add(std:("setupTestEnvironment"), [=]() mutable
    {
        cy->log(std:("Setting up test environment"));
        cy->task(std:("setupTestEnvironment"));
    }
    );
    Cypress->Commands->add(std:("waitForBackend"), [=]() mutable
    {
        cy->request(object{
            object::pair{std:("method"), std:("GET")}, 
            object::pair{std:("url"), string_empty + (OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")))) + std:("/api/server/health")}, 
            object::pair{std:("retryOnStatusCodeFailure"), true}, 
            object::pair{std:("retryOnNetworkFailure"), true}, 
            object::pair{std:("timeout"), 30000}
        })->then([=]() mutable
        {
            cy->log(std:("✅ Backend is ready!"));
        }
        );
    }
    );
    Cypress->Commands->add(std:("waitForElizaClient"), [=]() mutable
    {
        cy->window()->should([=](auto win) mutable
        {
            expect(win["elizaClient"])->to->exist;
            expect(win["elizaClient"]["socket"])->to->exist;
        }
        );
    }
    );
    Cypress->Commands->add(std:("sendMessage"), [=](P0 contentOrMessage, auto roomId = undefined) mutable
    {
        auto BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
        object message;
        if (type_of(contentOrMessage) == std:("string")) {
            message = object{
                object::pair{std:("text"), contentOrMessage}, 
                object::pair{std:("userId"), std:("test-user-") + Date->now() + string_empty}, 
                object::pair{std:("roomId"), OR((roomId), (std:("00000000-0000-0000-0000-000000000001")))}, 
                object::pair{std:("messageId"), Date->now()->toString()}
            };
        } else {
            message = object{
                object::pair{std:("text"), contentOrMessage["text"]}, 
                object::pair{std:("userId"), OR((contentOrMessage["userId"]), (std:("test-user-") + Date->now() + string_empty))}, 
                object::pair{std:("roomId"), OR((contentOrMessage["roomId"]), (std:("00000000-0000-0000-0000-000000000001")))}, 
                object::pair{std:("messageId"), OR((contentOrMessage["messageId"]), (Date->now()->toString()))}
            };
        }
        auto headers = object{
            object::pair{std:("Content-Type"), std:("application/json")}
        };
        auto apiKey = Cypress->env(std:("ELIZA_SERVER_AUTH_TOKEN"));
        if (apiKey) {
            headers[std:("X-API-KEY")] = apiKey;
        }
        return cy->request(object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("url"), string_empty + BACKEND_URL + std:("/api/messaging/ingest-external")}, 
            object::pair{std:("body"), object{
                object::pair{std:("channel_id"), message["roomId"]}, 
                object::pair{std:("server_id"), std:("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std:("author_id"), message["userId"]}, 
                object::pair{std:("author_display_name"), message["userId"]}, 
                object::pair{std:("content"), message["text"]}, 
                object::pair{std:("source_type"), std:("test")}, 
                object::pair{std:("raw_message"), object{
                    object::pair{std:("text"), message["text"]}, 
                    object::pair{std:("messageId"), message["messageId"]}
                }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("test"), true}, 
                    object::pair{std:("originalMessageId"), message["messageId"]}
                }}
            }}, 
            object::pair{std:("headers"), std:("headers")}, 
            object::pair{std:("failOnStatusCode"), false}
        });
    }
    );
    Cypress->Commands->add(std:("elementExists"), [=](auto selector) mutable
    {
        cy->get(std:("body"))->then([=](auto $body) mutable
        {
            return cy->wrap($body["find"](selector)["length"] > 0);
        }
        );
    }
    );
    Cypress->Commands->add(std:("safeClick"), [=](auto selector) mutable
    {
        cy->get(selector)->should(std:("be.visible"))->click(object{
            object::pair{std:("force"), true}
        });
    }
    );
    Cypress->Commands->add(std:("uploadKnowledgeFile"), [=](auto fileName, auto content, auto fileType = undefined) mutable
    {
        return cy->wrap(knowledgeHelper->uploadFile(fileName, content, OR((fileType), (std:("text/plain")))));
    }
    );
    Cypress->Commands->add(std:("searchKnowledge"), [=](auto query, auto count = undefined) mutable
    {
        return knowledgeHelper->search(query, count);
    }
    );
    Cypress->Commands->add(std:("deleteKnowledgeDocument"), [=](auto documentId) mutable
    {
        return knowledgeHelper->deleteDocument(documentId);
    }
    );
    Cypress->Commands->add(std:("cleanupKnowledgeTests"), [=]() mutable
    {
        return knowledgeHelper->cleanupTestDocuments();
    }
    );
    Cypress->Commands->add(std:("waitForDocumentProcessing"), [=](auto documentId, auto timeout = undefined) mutable
    {
        return knowledgeHelper->waitForDocumentProcessing(documentId, timeout);
    }
    );
    Cypress->Commands->add(std:("toggleCapability"), [=](auto capability) mutable
    {
        auto capabilityMap = object{
            object::pair{std:("autonomy"), std:("autonomy-toggle")}, 
            object::pair{std:("camera"), std:("camera-toggle")}, 
            object::pair{std:("screen"), std:("screen-toggle")}, 
            object::pair{std:("microphone"), std:("microphone-toggle")}, 
            object::pair{std:("speaker"), std:("speakers-toggle")}, 
            object::pair{std:("shell"), std:("shell-toggle")}, 
            object::pair{std:("browser"), std:("browser-toggle")}
        };
        auto testId = const_(capabilityMap)[capability->toLowerCase()];
        if (testId) {
            cy->get(std:("[data-testid="") + testId + std:(""]"))->click();
            cy->wait(1000);
        } else {
            throw any(std::make_shared<Error>(std:("Unknown capability: ") + capability + string_empty));
        }
    }
    );
    Cypress->Commands->add(std:("getCapabilityStatus"), [=](auto capability) mutable
    {
        auto BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
        if (capability == std:("autonomy")) {
            return cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/autonomy/status"))->then([=](auto response) mutable
            {
                return response["body"]->data->enabled;
            }
            );
        } else {
            return cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/agents/default/capabilities/") + capability + string_empty)->then([=](auto response) mutable
            {
                return response["body"]->data->enabled;
            }
            );
        }
    }
    );
    Cypress->Commands->add(std:("authenticateDb"), [=](auto username = undefined, auto password = undefined) mutable
    {
        return dbHelper->authenticate(username, password);
    }
    );
    Cypress->Commands->add(std:("getDbTables"), [=]() mutable
    {
        return dbHelper->getTables();
    }
    );
    Cypress->Commands->add(std:("cleanupDbTestRecords"), [=](auto tableName, auto searchPattern = undefined) mutable
    {
        return dbHelper->cleanupTestRecords(tableName, searchPattern);
    }
    );
}

MAIN
