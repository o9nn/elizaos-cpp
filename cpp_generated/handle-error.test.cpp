#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/handle-error.test.h"

std::function<any(double)> originalmockExit = process->exit;
any mockExit = mock([=](auto code) mutable
{
    throw any(std::make_shared<Error>(std::string("process.exit called with code ") + code + string_empty));
}
);

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("error"), mock()}
            }}
        });
    }
    );
    describe(std::string("handleError"), [=]() mutable
    {
        it(std::string("should handle Error objects with message"), [=]() mutable
        {
            shared error = std::make_shared<Error>(std::string("Test error message"));
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle Error objects with stack trace"), [=]() mutable
        {
            shared error = std::make_shared<Error>(std::string("Test error"));
            error->stack = std::string("Error: Test error\
    at testFunction (test.js:10:5)");
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle string errors"), [=]() mutable
        {
            shared error = std::string("String error message");
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle unknown error types"), [=]() mutable
        {
            shared error = object{
                object::pair{std::string("custom"), std::string("error object")}
            };
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle null error"), [=]() mutable
        {
            expect([=]() mutable
            {
                return handleError(nullptr);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle undefined error"), [=]() mutable
        {
            expect([=]() mutable
            {
                return handleError(undefined);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle error objects without message"), [=]() mutable
        {
            shared error = std::make_shared<Error>();
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
        it(std::string("should handle circular reference errors"), [=]() mutable
        {
            shared error = object{
                object::pair{std::string("prop"), std::string("value")}
            };
            error["circular"] = error;
            expect([=]() mutable
            {
                return handleError(error);
            }
            )->toThrow(std::string("process.exit called with code 1"));
        }
        );
    }
    );
}

MAIN
