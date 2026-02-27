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
    string name = std:("plugin-shell-basic-e2e");

    string description = std:("Basic end-to-end tests for shell plugin functionality");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Should execute simple echo command")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing simple echo command execution..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-1"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("echo "Hello from shell plugin"")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared callbackCalled = false;
            shared callbackResponse = nullptr;
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto response) mutable
            {
                callbackCalled = true;
                callbackResponse = response;
                return array<any>();
            }
            ); });
            if (!callbackCalled) {
                throw any(std::make_shared<Error>(std:("Callback was not called")));
            }
            if (!callbackResponse["text"]["includes"](std:("Hello from shell plugin"))) {
                throw any(std::make_shared<Error>(std:("Unexpected output: ") + callbackResponse["text"] + string_empty));
            }
            console->log(std:("✓ Echo command executed successfully"));
            console->log(std:("  Output: ") + callbackResponse["text"] + string_empty);
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should list files in current directory")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing ls command..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-2"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("ls -la")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            if (OR((OR((!response), (!response["attachments"]))), (response["attachments"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std:("No attachments returned with shell output")));
            }
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (outputData["exitCode"] != 0) {
                throw any(std::make_shared<Error>(std:("ls command failed with exit code: ") + outputData["exitCode"] + string_empty));
            }
            console->log(std:("✓ ls command executed successfully"));
            console->log(std:("  Files found: ") + (outputData["stdout"]["split"](std:("\
"))["length"] - 1) + std:(" items"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle command errors gracefully")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing error handling with invalid command..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-3"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("thisisnotavalidcommand123")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            if (OR((OR((!response), (!response["attachments"]))), (response["attachments"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std:("No error information returned")));
            }
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (outputData["exitCode"] == 0) {
                throw any(std::make_shared<Error>(std:("Expected non-zero exit code for invalid command")));
            }
            console->log(std:("✓ Error handling works correctly"));
            console->log(std:("  Exit code: ") + outputData["exitCode"] + string_empty);
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should extract command from natural language")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing natural language command extraction..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-4"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Can you show me what files are in the current directory?")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, message, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            if (OR((OR((!response), (!response["attachments"]))), (response["attachments"]["length"] == 0))) {
                throw any(std::make_shared<Error>(std:("Command extraction failed")));
            }
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (AND((!outputData["command"]["includes"](std:("ls"))), (!outputData["command"]["includes"](std:("dir"))))) {
                throw any(std::make_shared<Error>(std:("Unexpected command extracted: ") + outputData["command"] + string_empty));
            }
            console->log(std:("✓ Natural language command extraction successful"));
            console->log(std:("  Extracted command: ") + outputData["command"] + string_empty);
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should clear shell history")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing shell history clearing..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std:("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std:("Shell service not available")));
            }
            std::async([=]() { shellService->executeCommand(std:("echo test1")); });
            std::async([=]() { shellService->executeCommand(std:("echo test2")); });
            auto history = shellService->getHistory();
            if (history->length < 2) {
                throw any(std::make_shared<Error>(std:("History not properly recorded")));
            }
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto message = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-msg-5"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("clear shell history")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            std::async([=]() { clearShellHistoryAction->handler(runtime, message, state, object{}, [=]() mutable
            {
                return array<any>();
            }
            ); });
            history = shellService->getHistory();
            if (history->length != 0) {
                throw any(std::make_shared<Error>(std:("History was not cleared")));
            }
            console->log(std:("✓ Shell history cleared successfully"));
        }
        }
    } };
};

#endif
