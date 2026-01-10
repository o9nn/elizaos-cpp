#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SHELL_SRC_TESTS_E2E_SHELL-BASIC_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SHELL_SRC_TESTS_E2E_SHELL-BASIC_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "../../service.h"
#include "../../action.h"

class ShellBasicE2ETestSuite;

class ShellBasicE2ETestSuite : public TestSuite, public std::enable_shared_from_this<ShellBasicE2ETestSuite> {
public:
    using std::enable_shared_from_this<ShellBasicE2ETestSuite>::shared_from_this;
    string name = std::string("plugin-shell-basic-e2e");

    string description = std::string("Basic end-to-end tests for shell plugin functionality");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Should execute simple echo command")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing simple echo command execution..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-1"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("echo "Hello from shell plugin"")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            shared callbackResponse = nullptr;
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto response) mutable
            {
                callbackCalled = true;
                callbackResponse = response;
                return array<any>();
            }
            ); });
            if (!callbackCalled) {
                throw any(std::make_shared<Error>(std::string("Callback was not called")));
            }
            if (!callbackResponse["text"]["includes"](std::string("Hello from shell plugin"))) {
                throw any(std::make_shared<Error>(std::string("Unexpected output: ") + callbackResponse["text"] + string_empty));
            }
            console->log(std::string("✓ Echo command executed successfully"));
            console->log(std::string("  Output: ") + callbackResponse["text"] + string_empty);
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should list files in current directory")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing ls command..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-2"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("ls -la")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            if (OR((OR((!response), (!response["attachments"]))), (response["attachments"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std::string("No attachments returned with shell output")));
            }
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (outputData["exitCode"] != 0) {
                throw any(std::make_shared<Error>(std::string("ls command failed with exit code: ") + outputData["exitCode"] + string_empty));
            }
            console->log(std::string("✓ ls command executed successfully"));
            console->log(std::string("  Files found: ") + (outputData["stdout"]["split"](std::string("\
"))["length"] - 1) + std::string(" items"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should handle command errors gracefully")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing error handling with invalid command..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-3"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("thisisnotavalidcommand123")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            if (OR((OR((!response), (!response["attachments"]))), (response["attachments"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std::string("No error information returned")));
            }
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (outputData["exitCode"] == 0) {
                throw any(std::make_shared<Error>(std::string("Expected non-zero exit code for invalid command")));
            }
            console->log(std::string("✓ Error handling works correctly"));
            console->log(std::string("  Exit code: ") + outputData["exitCode"] + string_empty);
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should extract command from natural language")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing natural language command extraction..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-4"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Can you show me what files are in the current directory?")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            if (OR((OR((!response), (!response["attachments"]))), (response["attachments"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std::string("Command extraction failed")));
            }
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (AND((!outputData["command"]["includes"](std::string("ls"))), (!outputData["command"]["includes"](std::string("dir"))))) {
                throw any(std::make_shared<Error>(std::string("Unexpected command extracted: ") + outputData["command"] + string_empty));
            }
            console->log(std::string("✓ Natural language command extraction successful"));
            console->log(std::string("  Extracted command: ") + outputData["command"] + string_empty);
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should clear shell history")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing shell history clearing..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std::string("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std::string("Shell service not available")));
            }
            std::async([=]() { shellService->executeCommand(std::string("echo test1")); });
            std::async([=]() { shellService->executeCommand(std::string("echo test2")); });
            auto history = shellService->getHistory();
            if (history->length < 2) {
                throw any(std::make_shared<Error>(std::string("History not properly recorded")));
            }
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto message = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-msg-5"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("clear shell history")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            std::async([=]() { clearShellHistoryAction->handler(runtime, message, state, object{}, [=]() mutable
            {
                return array<any>();
            }
            ); });
            history = shellService->getHistory();
            if (history->length != 0) {
                throw any(std::make_shared<Error>(std::string("History was not cleared")));
            }
            console->log(std::string("✓ Shell history cleared successfully"));
        }
        }
    } };
};

#endif
