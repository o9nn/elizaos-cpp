#include "instrumentation.test.h"

void Main(void)
{
    describe(std:("InstrumentationService"), [=]() mutable
    {
        it(std:("initializes and can flush and stop"), [=]() mutable
        {
            auto svc = std::make_shared<InstrumentationService>(object{
                object::pair{std:("enabled"), true}, 
                object::pair{std:("serviceName"), std:("test")}
            });
            expect(svc->isEnabled())->toBe(true);
            std::async([=]() { svc->flush(); });
            std::async([=]() { svc->stop(); });
            expect(svc->isEnabled())->toBe(false);
        }
        );
        it(std:("disabled service reports disabled"), [=]() mutable
        {
            auto svc = std::make_shared<InstrumentationService>(object{
                object::pair{std:("enabled"), false}, 
                object::pair{std:("serviceName"), std:("x")}
            });
            expect(svc->isEnabled())->toBe(false);
        }
        );
    }
    );
}

MAIN
