#include "classified/packages/game/cypress/support/wait-for-server.h"

std::function<std::any(double)> waitForElizaServer = [=](auto maxRetries = 36) mutable
{
    cy->log(std::string("🔄 Waiting for ElizaOS AgentServer to be ready..."));
    shared waitForServer = [=](auto retries = maxRetries) mutable
    {
        if (retries <= 0) {
            cy->log(std::string("❌ Server failed to respond after maximum retries"));
            throw std::any(std::make_shared<Error>(std::string("Server failed to respond after ") + (maxRetries * 5) + std::string(" seconds")));
        }
        return cy->request(object{
            object::pair{std::string("method"), std::string("GET")}, 
            object::pair{std::string("url"), std::string("http://localhost:7777/api/server/health")}, 
            object::pair{std::string("failOnStatusCode"), false}, 
            object::pair{std::string("timeout"), 10000}
        })->then([=](auto response) mutable
        {
            if (response["status"] == 200) {
                cy->log(std::string("✅ AgentServer is ready!"));
                return response;
            } else {
                cy->log(std::string("⏳ Server not ready yet (status: ") + response["status"] + std::string("), retrying... (") + (maxRetries - retries + 1) + std::string("/") + maxRetries + std::string(")"));
                cy->wait(5000);
                return waitForServer(retries - 1);
            }
        }
        )->_catch([=]() mutable
        {
            cy->log(std::string("⏳ Server connection failed, retrying... (") + (maxRetries - retries + 1) + std::string("/") + maxRetries + std::string(")"));
            cy->wait(5000);
            return waitForServer(retries - 1);
        }
        );
    };
    return waitForServer();
};
std::function<std::any(double)> waitForElizaServerOptional = [=](auto maxRetries = 36) mutable
{
    cy->log(std::string("🔄 Waiting for ElizaOS AgentServer (std::optional)..."));
    shared waitForServer = [=](auto retries = maxRetries) mutable
    {
        if (retries <= 0) {
            cy->log(std::string("⚠️ Server not available after maximum retries, continuing with frontend-only test"));
            return Promise->resolve(object{
                object::pair{std::string("status"), 503}
            });
        }
        return cy->request(object{
            object::pair{std::string("method"), std::string("GET")}, 
            object::pair{std::string("url"), std::string("http://localhost:7777/api/server/health")}, 
            object::pair{std::string("failOnStatusCode"), false}, 
            object::pair{std::string("timeout"), 10000}
        })->then([=](auto response) mutable
        {
            if (response["status"] == 200) {
                cy->log(std::string("✅ AgentServer is ready!"));
                return response;
            } else {
                cy->log(std::string("⏳ Server not ready yet (status: ") + response["status"] + std::string("), retrying... (") + (maxRetries - retries + 1) + std::string("/") + maxRetries + std::string(")"));
                cy->wait(5000);
                return waitForServer(retries - 1);
            }
        }
        )->_catch([=]() mutable
        {
            cy->log(std::string("⏳ Server connection failed, retrying... (") + (maxRetries - retries + 1) + std::string("/") + maxRetries + std::string(")"));
            cy->wait(5000);
            return waitForServer(retries - 1);
        }
        );
    };
    return waitForServer();
};

void Main(void)
{
}

MAIN
