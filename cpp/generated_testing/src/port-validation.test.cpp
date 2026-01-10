#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/port-validation.test.h"

void Main(void)
{
    describe(std::string("validatePort"), [=]() mutable
    {
        it(std::string("should return valid port number as number"), [=]() mutable
        {
            expect(validatePort(std::string("3000")))->toBe(3000);
            expect(validatePort(std::string("8080")))->toBe(8080);
            expect(validatePort(std::string("65535")))->toBe(65535);
            expect(validatePort(std::string("1")))->toBe(1);
        }
        );
        it(std::string("should throw error for port number 0"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std::string("0"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
        }
        );
        it(std::string("should throw error for negative port numbers"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std::string("-1"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std::string("-8080"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
        }
        );
        it(std::string("should throw error for port numbers above 65535"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std::string("65536"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std::string("70000"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std::string("999999"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
        }
        );
        it(std::string("should throw error for non-numeric input"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std::string("abc"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect(validatePort(std::string("3000abc")))->toBe(3000);
            expect([=]() mutable
            {
                return validatePort(std::string("!@#$"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(string_empty);
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std::string(" "));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
        }
        );
        it(std::string("should handle decimal numbers correctly"), [=]() mutable
        {
            expect(validatePort(std::string("3000.5")))->toBe(3000);
            expect(validatePort(std::string("8080.0")))->toBe(8080);
        }
        );
        it(std::string("should handle string with spaces correctly"), [=]() mutable
        {
            expect(validatePort(std::string(" 3000 ")))->toBe(3000);
            expect(validatePort(std::string("30 00")))->toBe(30);
        }
        );
        it(std::string("should handle special number formats correctly"), [=]() mutable
        {
            expect(validatePort(std::string("1e3")))->toBe(1);
            expect([=]() mutable
            {
                return validatePort(std::string("0x1234"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std::string("Infinity"));
            }
            )->toThrow(std::string("Port must be a number between 1 and 65535"));
        }
        );
    }
    );
}

MAIN
