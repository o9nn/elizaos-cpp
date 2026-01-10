#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/core/src/__tests__/services-by-type.test.h"

any MockWalletService1::override;

MockWalletService1::MockWalletService1(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

std::shared_ptr<Promise<std::shared_ptr<MockWalletService1>>> MockWalletService1::start(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<MockWalletService1>(runtime);
}

std::shared_ptr<Promise<void>> MockWalletService1::stop()
{
    return std::shared_ptr<Promise<void>>();
}

any MockWalletService2::override;

MockWalletService2::MockWalletService2(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

std::shared_ptr<Promise<std::shared_ptr<MockWalletService2>>> MockWalletService2::start(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<MockWalletService2>(runtime);
}

std::shared_ptr<Promise<void>> MockWalletService2::stop()
{
    return std::shared_ptr<Promise<void>>();
}

any MockPdfService::override;

MockPdfService::MockPdfService(std::shared_ptr<IAgentRuntime> runtime) : Service(runtime) {
}

std::shared_ptr<Promise<std::shared_ptr<MockPdfService>>> MockPdfService::start(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<MockPdfService>(runtime);
}

std::shared_ptr<Promise<void>> MockPdfService::stop()
{
    return std::shared_ptr<Promise<void>>();
}


void Main(void)
{
    describe(std::string("Service Type System"), [=]() mutable
    {
        shared<std::shared_ptr<AgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("username"), std::string("test")}, 
                    object::pair{std::string("clients"), array<any>()}
                }}
            });
        }
        );
        describe(std::string("Multiple services of same type"), [=]() mutable
        {
            it(std::string("should allow registering multiple services of the same type"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(true);
                auto walletServices = runtime->getServicesByType(ServiceType["WALLET"]);
                expect(walletServices)->toHaveLength(2);
                expect(const_(walletServices)[0])->toBeInstanceOf(MockWalletService1);
                expect(const_(walletServices)[1])->toBeInstanceOf(MockWalletService2);
            }
            );
            it(std::string("should return first service when using getService"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                auto firstService = runtime->getService(ServiceType["WALLET"]);
                expect(firstService)->toBeInstanceOf(MockWalletService1);
            }
            );
            it(std::string("should return empty array for non-existent service type"), [=]() mutable
            {
                auto services = runtime->getServicesByType(std::string("non-existent-type"));
                expect(services)->toHaveLength(0);
            }
            );
            it(std::string("should return null for non-existent service type with getService"), [=]() mutable
            {
                auto service = runtime->getService(std::string("non-existent-type"));
                expect(service)->toBe(nullptr);
            }
            );
        }
        );
        describe(std::string("Mixed service types"), [=]() mutable
        {
            it(std::string("should handle multiple service types correctly"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                std::async([=]() { runtime->registerService(MockPdfService); });
                auto walletServices = runtime->getServicesByType(ServiceType["WALLET"]);
                expect(walletServices)->toHaveLength(2);
                auto pdfServices = runtime->getServicesByType(ServiceType["PDF"]);
                expect(pdfServices)->toHaveLength(1);
                expect(const_(pdfServices)[0])->toBeInstanceOf(MockPdfService);
                auto videoServices = runtime->getServicesByType(ServiceType["VIDEO"]);
                expect(videoServices)->toHaveLength(0);
            }
            );
            it(std::string("should return correct services with getAllServices"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                std::async([=]() { runtime->registerService(MockPdfService); });
                auto allServices = runtime->getAllServices();
                expect(allServices->size)->toBe(2);
                expect(allServices->get(ServiceType["WALLET"]))->toHaveLength(2);
                expect(allServices->get(ServiceType["PDF"]))->toHaveLength(1);
            }
            );
        }
        );
        describe(std::string("Service type validation"), [=]() mutable
        {
            it(std::string("should handle hasService correctly with multiple services"), [=]() mutable
            {
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(false);
                std::async([=]() { runtime->registerService(MockWalletService1); });
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(true);
                std::async([=]() { runtime->registerService(MockWalletService2); });
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(true);
            }
            );
            it(std::string("should return correct service types with getRegisteredServiceTypes"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockPdfService); });
                auto serviceTypes = runtime->getRegisteredServiceTypes();
                expect(serviceTypes)->toContain(ServiceType["WALLET"]);
                expect(serviceTypes)->toContain(ServiceType["PDF"]);
                expect(serviceTypes)->toHaveLength(2);
            }
            );
        }
        );
        describe(std::string("Service lifecycle"), [=]() mutable
        {
            it(std::string("should stop all services of all types"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                std::async([=]() { runtime->registerService(MockPdfService); });
                shared stopCalls = array<string>();
                auto walletServices = runtime->getServicesByType(ServiceType["WALLET"]);
                auto pdfServices = runtime->getServicesByType(ServiceType["PDF"]);
                std::bind(&Service::stop, const_(walletServices)[0]) = [=]() mutable
                {
                    stopCalls->push(std::string("wallet1"));
                };
                std::bind(&Service::stop, const_(walletServices)[1]) = [=]() mutable
                {
                    stopCalls->push(std::string("wallet2"));
                };
                std::bind(&Service::stop, const_(pdfServices)[0]) = [=]() mutable
                {
                    stopCalls->push(std::string("pdf"));
                };
                std::async([=]() { runtime->stop(); });
                expect(stopCalls)->toContain(std::string("wallet1"));
                expect(stopCalls)->toContain(std::string("wallet2"));
                expect(stopCalls)->toContain(std::string("pdf"));
                expect(stopCalls)->toHaveLength(3);
            }
            );
        }
        );
    }
    );
}

MAIN
