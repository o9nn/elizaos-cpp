#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC___TESTS___TEST-UTILS_MOCKS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC___TESTS___TEST-UTILS_MOCKS_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "express.h"
#include "bun:test.h"

std::shared_ptr<IAgentRuntime> createMockAgentRuntime(Partial<std::shared_ptr<IAgentRuntime>> overrides = undefined);

any createMockDatabaseAdapter(any overrides = undefined);

std::shared_ptr<Request> createMockRequest(Partial<std::shared_ptr<Request>> overrides = undefined);

std::shared_ptr<Response> createMockResponse();

std::shared_ptr<NextFunction> createMockNext();

any createMockSocketIO();

any createMockHttpServer();

std::shared_ptr<Service> createMockService(Partial<std::shared_ptr<Service>> overrides = undefined);

std::shared_ptr<Express::Multer::File> createMockUploadedFile(Partial<std::shared_ptr<Express::Multer::File>> overrides = undefined);

#endif
