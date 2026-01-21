#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC___TESTS___SERVICES-BY-TYPE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC___TESTS___SERVICES-BY-TYPE_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../runtime.h"
#include "../types.h"
#include "../types/service.h"
#include "../types/runtime.h"
#include "uuid.h"
using uuidv4 = v4;

class MockWalletService1;
class MockWalletService2;
class MockPdfService;

class MockWalletService1 : public Service, public std::enable_shared_from_this<MockWalletService1> {
public:
    using std::enable_shared_from_this<MockWalletService1>::shared_from_this;
    static any override;

    string serviceType = ServiceType["WALLET"];

    string capabilityDescription = std::string("Mock wallet service 1");

    MockWalletService1(std::shared_ptr<IAgentRuntime> runtime);
    static std::shared_ptr<Promise<std::shared_ptr<MockWalletService1>>> start(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<void>> stop();
};

class MockWalletService2 : public Service, public std::enable_shared_from_this<MockWalletService2> {
public:
    using std::enable_shared_from_this<MockWalletService2>::shared_from_this;
    static any override;

    string serviceType = ServiceType["WALLET"];

    string capabilityDescription = std::string("Mock wallet service 2");

    MockWalletService2(std::shared_ptr<IAgentRuntime> runtime);
    static std::shared_ptr<Promise<std::shared_ptr<MockWalletService2>>> start(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<void>> stop();
};

class MockPdfService : public Service, public std::enable_shared_from_this<MockPdfService> {
public:
    using std::enable_shared_from_this<MockPdfService>::shared_from_this;
    static any override;

    string serviceType = ServiceType["PDF"];

    string capabilityDescription = std::string("Mock PDF service");

    MockPdfService(std::shared_ptr<IAgentRuntime> runtime);
    static std::shared_ptr<Promise<std::shared_ptr<MockPdfService>>> start(std::shared_ptr<IAgentRuntime> runtime);
    virtual std::shared_ptr<Promise<void>> stop();
};

#endif
