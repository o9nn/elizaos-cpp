#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/instrumentation.test.h"

void Main(void)
{
    describe(std::string("InstrumentationService"), [=]() mutable
    {
        it(std::string("initializes and can flush and stop"), [=]() mutable
        {
            auto svc = std::make_shared<InstrumentationService>(object{
                object::pair{std::string("enabled"), true}, 
                object::pair{std::string("serviceName"), std::string("test")}
            });
            expect(svc->isEnabled())->toBe(true);
            std::async([=]() { svc->flush(); });
            std::async([=]() { svc->stop(); });
            expect(svc->isEnabled())->toBe(false);
        }
        );
        it(std::string("disabled service reports disabled"), [=]() mutable
        {
            auto svc = std::make_shared<InstrumentationService>(object{
                object::pair{std::string("enabled"), false}, 
                object::pair{std::string("serviceName"), std::string("x")}
            });
            expect(svc->isEnabled())->toBe(false);
        }
        );
    }
    );
}

MAIN
