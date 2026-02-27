#include "handle-error.test.h"

std::function<any(double)> originalmockExit = process->exit;
any mockExit = mock([=](auto code) mutable
{
    throw any(std::make_shared<Error>(std:("process.exit called with code ") + code + string_empty));
}
);

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("error"), mock()}
            }}
        });
    }
    );
    describe(std:("handleError"), [=]() mutable
    {
        it(std:("should handle Error objects with message"), [=]() mutable
        {
            shared error = std::make_shared<Error>(std:("Test error message"));
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle Error objects with stack trace"), [=]() mutable
        {
            shared error = std::make_shared<Error>(std:("Test error"));
            error->stack = std:("Error: Test error\
    at testFunction (test.js:10:5)");
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle string errors"), [=]() mutable
        {
            shared error = std:("String error message");
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle unknown error types"), [=]() mutable
        {
            shared error = object{
                object::pair{std:("custom"), std:("error object")}
            };
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle null error"), [=]() mutable
        {
            expect([=]() mutable
            {
                return handleError(nullptr);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle undefined error"), [=]() mutable
        {
            expect([=]() mutable
            {
                return handleError(undefined);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle error objects without message"), [=]() mutable
        {
            shared error = std::make_shared<Error>();
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
        it(std:("should handle circular reference errors"), [=]() mutable
        {
            shared error = object{
                object::pair{std:("prop"), std:("value")}
            };
            error["circular"] = error;
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std:("process.exit called with code 1"));
        }
        );
    }
    );
}

MAIN
