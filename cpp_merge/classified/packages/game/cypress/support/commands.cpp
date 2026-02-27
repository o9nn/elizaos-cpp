#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/support/commands.h"

namespace global {
    namespace Cypress {
    }
}
std::shared_ptr<KnowledgeTestHelper> knowledgeHelper = std::make_shared<KnowledgeTestHelper>();
std::shared_ptr<DatabaseTestHelper> dbHelper = std::make_shared<DatabaseTestHelper>();

void Main(void)
{
    Cypress->Commands->add(std::string("setupTestEnvironment"), [=]() mutable
    {
        cy->log(std::string("Setting up test environment"));
        cy->task(std::string("setupTestEnvironment"));
    }
    );
    Cypress->Commands->add(std::string("waitForBackend"), [=]() mutable
    {
        cy->request(object{
            object::pair{std::string("method"), std::string("GET")}, 
            object::pair{std::string("url"), string_empty + (OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")))) + std::string("/api/server/health")}, 
            object::pair{std::string("retryOnStatusCodeFailure"), true}, 
            object::pair{std::string("retryOnNetworkFailure"), true}, 
            object::pair{std::string("timeout"), 30000}
        })->then([=]() mutable
        {
            cy->log(std::string("✅ Backend is ready!"));
        }
        );
    }
    );
    Cypress->Commands->add(std::string("waitForElizaClient"), [=]() mutable
    {
        cy->window()->should([=](auto win) mutable
        {
            expect(win["elizaClient"])->to->exist;
            expect(win["elizaClient"]["socket"])->to->exist;
        }
        );
    }
    );
    Cypress->Commands->add(std::string("sendMessage"), [=](P0 contentOrMessage, auto roomId = undefined) mutable
    {
        auto BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
        object message;
        if (type_of(contentOrMessage) == std::string("string")) {
            message = object{
                object::pair{std::string("text"), contentOrMessage}, 
                object::pair{std::string("userId"), std::string("test-user-") + Date->now() + string_empty}, 
                object::pair{std::string("roomId"), OR((roomId), (std::string("00000000-0000-0000-0000-000000000001")))}, 
                object::pair{std::string("messageId"), Date->now()->toString()}
            };
        } else {
            message = object{
                object::pair{std::string("text"), contentOrMessage["text"]}, 
                object::pair{std::string("userId"), OR((contentOrMessage["userId"]), (std::string("test-user-") + Date->now() + string_empty))}, 
                object::pair{std::string("roomId"), OR((contentOrMessage["roomId"]), (std::string("00000000-0000-0000-0000-000000000001")))}, 
                object::pair{std::string("messageId"), OR((contentOrMessage["messageId"]), (Date->now()->toString()))}
            };
        }
        auto headers = object{
            object::pair{std::string("Content-Type"), std::string("application/json")}
        };
        auto apiKey = Cypress->env(std::string("ELIZA_SERVER_AUTH_TOKEN"));
        if (apiKey) {
            headers[std::string("X-API-KEY")] = apiKey;
        }
        return cy->request(object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("url"), string_empty + BACKEND_URL + std::string("/api/messaging/ingest-external")}, 
            object::pair{std::string("body"), object{
                object::pair{std::string("channel_id"), message["roomId"]}, 
                object::pair{std::string("server_id"), std::string("00000000-0000-0000-0000-000000000000")}, 
                object::pair{std::string("author_id"), message["userId"]}, 
                object::pair{std::string("author_display_name"), message["userId"]}, 
                object::pair{std::string("content"), message["text"]}, 
                object::pair{std::string("source_type"), std::string("test")}, 
                object::pair{std::string("raw_message"), object{
                    object::pair{std::string("text"), message["text"]}, 
                    object::pair{std::string("messageId"), message["messageId"]}
                }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("test"), true}, 
                    object::pair{std::string("originalMessageId"), message["messageId"]}
                }}
            }}, 
            object::pair{std::string("headers"), std::string("headers")}, 
            object::pair{std::string("failOnStatusCode"), false}
        });
    }
    );
    Cypress->Commands->add(std::string("elementExists"), [=](auto selector) mutable
    {
        cy->get(std::string("body"))->then([=](auto $body) mutable
        {
            return cy->wrap($body["find"](selector)["length"] > 0);
        }
        );
    }
    );
    Cypress->Commands->add(std::string("safeClick"), [=](auto selector) mutable
    {
        cy->get(selector)->should(std::string("be.visible"))->click(object{
            object::pair{std::string("force"), true}
        });
    }
    );
    Cypress->Commands->add(std::string("uploadKnowledgeFile"), [=](auto fileName, auto content, auto fileType = undefined) mutable
    {
        return cy->wrap(knowledgeHelper->uploadFile(fileName, content, OR((fileType), (std::string("text/plain")))));
    }
    );
    Cypress->Commands->add(std::string("searchKnowledge"), [=](auto query, auto count = undefined) mutable
    {
        return knowledgeHelper->search(query, count);
    }
    );
    Cypress->Commands->add(std::string("deleteKnowledgeDocument"), [=](auto documentId) mutable
    {
        return knowledgeHelper->deleteDocument(documentId);
    }
    );
    Cypress->Commands->add(std::string("cleanupKnowledgeTests"), [=]() mutable
    {
        return knowledgeHelper->cleanupTestDocuments();
    }
    );
    Cypress->Commands->add(std::string("waitForDocumentProcessing"), [=](auto documentId, auto timeout = undefined) mutable
    {
        return knowledgeHelper->waitForDocumentProcessing(documentId, timeout);
    }
    );
    Cypress->Commands->add(std::string("toggleCapability"), [=](auto capability) mutable
    {
        auto capabilityMap = object{
            object::pair{std::string("autonomy"), std::string("autonomy-toggle")}, 
            object::pair{std::string("camera"), std::string("camera-toggle")}, 
            object::pair{std::string("screen"), std::string("screen-toggle")}, 
            object::pair{std::string("microphone"), std::string("microphone-toggle")}, 
            object::pair{std::string("speaker"), std::string("speakers-toggle")}, 
            object::pair{std::string("shell"), std::string("shell-toggle")}, 
            object::pair{std::string("browser"), std::string("browser-toggle")}
        };
        auto testId = const_(capabilityMap)[capability->toLowerCase()];
        if (testId) {
            cy->get(std::string("[data-testid="") + testId + std::string(""]"))->click();
            cy->wait(1000);
        } else {
            throw any(std::make_shared<Error>(std::string("Unknown capability: ") + capability + string_empty));
        }
    }
    );
    Cypress->Commands->add(std::string("getCapabilityStatus"), [=](auto capability) mutable
    {
        auto BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
        if (capability == std::string("autonomy")) {
            return cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/autonomy/status"))->then([=](auto response) mutable
            {
                return response["body"]->data->enabled;
            }
            );
        } else {
            return cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/agents/default/capabilities/") + capability + string_empty)->then([=](auto response) mutable
            {
                return response["body"]->data->enabled;
            }
            );
        }
    }
    );
    Cypress->Commands->add(std::string("authenticateDb"), [=](auto username = undefined, auto password = undefined) mutable
    {
        return dbHelper->authenticate(username, password);
    }
    );
    Cypress->Commands->add(std::string("getDbTables"), [=]() mutable
    {
        return dbHelper->getTables();
    }
    );
    Cypress->Commands->add(std::string("cleanupDbTestRecords"), [=](auto tableName, auto searchPattern = undefined) mutable
    {
        return dbHelper->cleanupTestRecords(tableName, searchPattern);
    }
    );
}

MAIN
