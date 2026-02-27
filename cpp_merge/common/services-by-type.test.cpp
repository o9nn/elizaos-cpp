#include "services-by-type.test.h"

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
    describe(std:("Service Type System"), [=]() mutable
    {
        shared<std::shared_ptr<AgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = std::make_shared<AgentRuntime>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("username"), std:("test")}, 
                    object::pair{std:("clients"), array<any>()}
                }}
            });
        }
        );
        describe(std:("Multiple services of same type"), [=]() mutable
        {
            it(std:("should allow registering multiple services of the same type"), [=]() mutable
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
            it(std:("should return first service when using getService"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                auto firstService = runtime->getService(ServiceType["WALLET"]);
                expect(firstService)->toBeInstanceOf(MockWalletService1);
            }
            );
            it(std:("should return empty array for non-existent service type"), [=]() mutable
            {
                auto services = runtime->getServicesByType(std:("non-existent-type"));
                expect(services)->toHaveLength(0);
            }
            );
            it(std:("should return null for non-existent service type with getService"), [=]() mutable
            {
                auto service = runtime->getService(std:("non-existent-type"));
                expect(service)->toBe(nullptr);
            }
            );
        }
        );
        describe(std:("Mixed service types"), [=]() mutable
        {
            it(std:("should handle multiple service types correctly"), [=]() mutable
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
            it(std:("should return correct services with getAllServices"), [=]() mutable
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
        describe(std:("Service type validation"), [=]() mutable
        {
            it(std:("should handle hasService correctly with multiple services"), [=]() mutable
            {
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(false);
                std::async([=]() { runtime->registerService(MockWalletService1); });
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(true);
                std::async([=]() { runtime->registerService(MockWalletService2); });
                expect(runtime->hasService(ServiceType["WALLET"]))->toBe(true);
            }
            );
            it(std:("should return correct service types with getRegisteredServiceTypes"), [=]() mutable
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
        describe(std:("Service lifecycle"), [=]() mutable
        {
            it(std:("should stop all services of all types"), [=]() mutable
            {
                std::async([=]() { runtime->registerService(MockWalletService1); });
                std::async([=]() { runtime->registerService(MockWalletService2); });
                std::async([=]() { runtime->registerService(MockPdfService); });
                shared stopCalls = array<string>();
                auto walletServices = runtime->getServicesByType(ServiceType["WALLET"]);
                auto pdfServices = runtime->getServicesByType(ServiceType["PDF"]);
                std::bind(&Service::stop, const_(walletServices)[0]) = [=]() mutable
                {
                    stopCalls->push(std:("wallet1"));
                };
                std::bind(&Service::stop, const_(walletServices)[1]) = [=]() mutable
                {
                    stopCalls->push(std:("wallet2"));
                };
                std::bind(&Service::stop, const_(pdfServices)[0]) = [=]() mutable
                {
                    stopCalls->push(std:("pdf"));
                };
                std::async([=]() { runtime->stop(); });
                expect(stopCalls)->toContain(std:("wallet1"));
                expect(stopCalls)->toContain(std:("wallet2"));
                expect(stopCalls)->toContain(std:("pdf"));
                expect(stopCalls)->toHaveLength(3);
            }
            );
        }
        );
    }
    );
}

MAIN
