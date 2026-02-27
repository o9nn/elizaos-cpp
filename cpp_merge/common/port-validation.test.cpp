#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/port-validation.test.h"

void Main(void)
{
    describe(std:("validatePort"), [=]() mutable
    {
        it(std:("should return valid port number"), [=]() mutable
        {
            expect(validatePort(std:("3000")))->toBe(3000);
            expect(validatePort(std:("8080")))->toBe(8080);
            expect(validatePort(std:("65535")))->toBe(65535);
            expect(validatePort(std:("1")))->toBe(1);
        }
        );
        it(std:("should throw error for port number 0"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std:("0"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
        }
        );
        it(std:("should throw error for negative port numbers"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std:("-1"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std:("-8080"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
        }
        );
        it(std:("should throw error for port numbers above 65535"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std:("65536"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std:("70000"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std:("999999"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
        }
        );
        it(std:("should throw error for non-numeric input"), [=]() mutable
        {
            expect([=]() mutable
            {
                return validatePort(std:("abc"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect(validatePort(std:("3000abc")))->toBe(3000);
            expect([=]() mutable
            {
                return validatePort(std:("!@#$"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(string_empty);
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std:(" "));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
        }
        );
        it(std:("should handle decimal numbers correctly"), [=]() mutable
        {
            expect(validatePort(std:("3000.5")))->toBe(3000);
            expect(validatePort(std:("8080.0")))->toBe(8080);
        }
        );
        it(std:("should handle string with spaces correctly"), [=]() mutable
        {
            expect(validatePort(std:(" 3000 ")))->toBe(3000);
            expect(validatePort(std:("30 00")))->toBe(30);
        }
        );
        it(std:("should handle special number formats correctly"), [=]() mutable
        {
            expect(validatePort(std:("1e3")))->toBe(1);
            expect([=]() mutable
            {
                return validatePort(std:("0x1234"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
            expect([=]() mutable
            {
                return validatePort(std:("Infinity"));
            }
            )->toThrow(std:("Port must be a number between 1 and 65535"));
        }
        );
    }
    );
}

MAIN
