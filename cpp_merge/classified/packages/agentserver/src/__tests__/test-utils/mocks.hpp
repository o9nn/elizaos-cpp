#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC___TESTS___TEST_UTILS_MOCKS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_AGENTSERVER_SRC___TESTS___TEST_UTILS_MOCKS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "express.hpp"
#include "bun:test.h"

std::shared_ptr<IAgentRuntime> createMockAgentRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = std::nullopt);

any createMockDatabaseAdapter(any overrides = std::nullopt);

std::shared_ptr<Request> createMockRequest(Partial<std::shared_ptr<Request>> overrides = std::nullopt);

std::shared_ptr<Response> createMockResponse();

std::shared_ptr<NextFunction> createMockNext();

any createMockWebsocket();

any createMockHttpServer();

std::shared_ptr<Service> createMockService(Partial<std::shared_ptr<Service>> overrides = std::nullopt);

std::shared_ptr<Express::Multer::File> createMockUploadedFile(Partial<std::shared_ptr<Express::Multer::File>> overrides = std::nullopt);

#endif
