#include "wait-for-server.h"

std::function<any(double)> waitForElizaServer = [=](auto maxRetries = 36) mutable
{
    cy->log(std:("🔄 Waiting for ElizaOS AgentServer to be ready..."));
    shared waitForServer = [=](auto retries = maxRetries) mutable
    {
        if (retries <= 0) {
            cy->log(std:("❌ Server failed to respond after maximum retries"));
            throw any(std::make_shared<Error>(std:("Server failed to respond after ") + (maxRetries * 5) + std:(" seconds")));
        }
        return cy->request(object{
            object::pair{std:("method"), std:("GET")}, 
            object::pair{std:("url"), std:("http://localhost:7777/api/server/health")}, 
            object::pair{std:("failOnStatusCode"), false}, 
            object::pair{std:("timeout"), 10000}
        })->then([=](auto response) mutable
        {
            if (response["status"] == 200) {
                cy->log(std:("✅ AgentServer is ready!"));
                return response;
            } else {
                cy->log(std:("⏳ Server not ready yet (status: ") + response["status"] + std:("), retrying... (") + (maxRetries - retries + 1) + std:("/") + maxRetries + std:(")"));
                cy->wait(5000);
                return waitForServer(retries - 1);
            }
        }
        )->_catch([=]() mutable
        {
            cy->log(std:("⏳ Server connection failed, retrying... (") + (maxRetries - retries + 1) + std:("/") + maxRetries + std:(")"));
            cy->wait(5000);
            return waitForServer(retries - 1);
        }
        );
    };
    return waitForServer();
};
std::function<any(double)> waitForElizaServerOptional = [=](auto maxRetries = 36) mutable
{
    cy->log(std:("🔄 Waiting for ElizaOS AgentServer (optional)..."));
    shared waitForServer = [=](auto retries = maxRetries) mutable
    {
        if (retries <= 0) {
            cy->log(std:("⚠️ Server not available after maximum retries, continuing with frontend-only test"));
            return Promise->resolve(object{
                object::pair{std:("status"), 503}
            });
        }
        return cy->request(object{
            object::pair{std:("method"), std:("GET")}, 
            object::pair{std:("url"), std:("http://localhost:7777/api/server/health")}, 
            object::pair{std:("failOnStatusCode"), false}, 
            object::pair{std:("timeout"), 10000}
        })->then([=](auto response) mutable
        {
            if (response["status"] == 200) {
                cy->log(std:("✅ AgentServer is ready!"));
                return response;
            } else {
                cy->log(std:("⏳ Server not ready yet (status: ") + response["status"] + std:("), retrying... (") + (maxRetries - retries + 1) + std:("/") + maxRetries + std:(")"));
                cy->wait(5000);
                return waitForServer(retries - 1);
            }
        }
        )->_catch([=]() mutable
        {
            cy->log(std:("⏳ Server connection failed, retrying... (") + (maxRetries - retries + 1) + std:("/") + maxRetries + std:(")"));
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
