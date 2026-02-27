#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SHELL_SRC_TESTS_E2E_SHELL-ADVANCED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SHELL_SRC_TESTS_E2E_SHELL-ADVANCED_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "../../service.h"
#include "../../action.h"
#include "path.h"
using path = _default;

class ShellAdvancedE2ETestSuite;

class ShellAdvancedE2ETestSuite : public TestSuite, public std::enable_shared_from_this<ShellAdvancedE2ETestSuite> {
public:
    using std::enable_shared_from_this<ShellAdvancedE2ETestSuite>::shared_from_this;
    string name = std:("plugin-shell-advanced-e2e");

    string description = std:("Advanced tests for complex multi-action shell scenarios");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Should handle piped commands correctly")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing piped command execution..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto pipeMessage = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-pipe"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("echo "line1\
line2\
line3" | grep line2")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, pipeMessage, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (OR((!outputData["stdout"]["includes"](std:("line2"))), (outputData["stdout"]["includes"](std:("line1"))))) {
                throw any(std::make_shared<Error>(std:("Pipe command failed. Output: ") + outputData["stdout"] + string_empty));
            }
            console->log(std:("✓ Piped commands work correctly"));
            console->log(std:("  Output: ") + outputData["stdout"]["trim"]() + string_empty);
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should execute compound commands with && and ||")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing compound command execution..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto successMessage = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-and-success"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("echo "first" && echo "second"")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, successMessage, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (OR((!outputData["stdout"]["includes"](std:("first"))), (!outputData["stdout"]["includes"](std:("second"))))) {
                throw any(std::make_shared<Error>(std:("&& operator failed on success case")));
            }
            auto failMessage = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-or-fail"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("false || echo "fallback"")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            std::async([=]() { runShellCommandAction->handler(runtime, failMessage, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            attachment = const_(response["attachments"])[0];
            outputData = JSON->parse(attachment["text"]);
            if (!outputData["stdout"]["includes"](std:("fallback"))) {
                throw any(std::make_shared<Error>(std:("|| operator failed on failure case")));
            }
            console->log(std:("✓ Compound commands work correctly"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle complex multi-step workflow")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing complex multi-step workflow..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std:("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std:("Shell service not available")));
            }
            auto workDir = path->join(shellService->getCurrentWorkingDirectory(), std:("workflow-") + Date->now() + string_empty);
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            {
                utils::finally __finally8323_8513([&]() mutable
                {
                    std::async([=]() { shellService->executeCommand(std:("cd ") + path->dirname(workDir) + string_empty); });
                    std::async([=]() { shellService->executeCommand(std:("rm -rf ") + path->basename(workDir) + string_empty); });
                });
                try
                {
                    auto createStructureMsg = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("workflow-1"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("mkdir -p ") + workDir + std:("/{src,test,docs} && cd ") + workDir + std:(" && echo "# My Project" > README.md")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, createStructureMsg, state, object{}, [=]() mutable
                    {
                        return array<any>();
                    }
                    ); });
                    if (!shellService->getCurrentWorkingDirectory()->includes(path->basename(workDir))) {
                        throw any(std::make_shared<Error>(std:("Failed to change to workflow directory")));
                    }
                    auto createFilesMsg = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("workflow-2"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("echo "std::cout << 'Hello' << std::endl;" > src/index.js && echo "{};" > src/config.js")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, createFilesMsg, state, object{}, [=]() mutable
                    {
                        return array<any>();
                    }
                    ); });
                    auto packageJsonContent = object{
                        object::pair{std:("name"), std:("test-project")}, 
                        object::pair{std:("version"), std:("1.0.0")}, 
                        object::pair{std:("main"), std:("src/index.js")}
                    };
                    auto createPackageMsg = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("workflow-3"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("echo '") + JSON->stringify(packageJsonContent, nullptr, 2) + std:("' > package.json")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, createPackageMsg, state, object{}, [=]() mutable
                    {
                        return array<any>();
                    }
                    ); });
                    auto verifyMsg = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("workflow-4"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("find . -type f -name "*.js" -o -name "*.json" -o -name "*.md" | sort")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    shared response = nullptr;
                    std::async([=]() { runShellCommandAction->handler(runtime, verifyMsg, state, object{}, [=](auto resp) mutable
                    {
                        response = resp;
                        return array<any>();
                    }
                    ); });
                    auto attachment = const_(response["attachments"])[0];
                    auto outputData = JSON->parse(attachment["text"]);
                    auto expectedFiles = array<string>{ std:("./README.md"), std:("./package.json"), std:("./src/config.js"), std:("./src/index.js") };
                    auto actualFiles = outputData["stdout"]["trim"]()["split"](std:("\
"))["sort"]();
                    for (auto& expectedFile : expectedFiles)
                    {
                        if (!actualFiles["some"]([=](auto f) mutable
                        {
                            return f["includes"](expectedFile->replace(std:("./"), string_empty));
                        }
                        )) {
                            throw any(std::make_shared<Error>(std:("Expected file not found: ") + expectedFile + string_empty));
                        }
                    }
                    console->log(std:("✓ Complex workflow completed successfully"));
                    console->log(std:("  Created ") + actualFiles["length"] + std:(" files"));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle background processes and job control")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing background process handling..."));
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto bgMessage = object{
                object::pair{std:("id"), createUniqueUuid(runtime, std:("test-bg"))}, 
                object::pair{std:("entityId"), runtime->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("sleep 2 & echo "Process started in background"")}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            shared response = nullptr;
            std::async([=]() { runShellCommandAction->handler(runtime, bgMessage, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            auto attachment = const_(response["attachments"])[0];
            auto outputData = JSON->parse(attachment["text"]);
            if (!outputData["stdout"]["includes"](std:("Process started in background"))) {
                throw any(std::make_shared<Error>(std:("Background process test failed")));
            }
            console->log(std:("✓ Background process handling works"));
            console->log(std:("  Note: Full job control requires interactive shell"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle wildcards and glob patterns correctly")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing wildcard and glob pattern handling..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std:("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std:("Shell service not available")));
            }
            auto testDir = path->join(shellService->getCurrentWorkingDirectory(), std:("glob-test-") + Date->now() + string_empty);
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            {
                utils::finally __finally12679_12869([&]() mutable
                {
                    std::async([=]() { shellService->executeCommand(std:("cd ") + path->dirname(testDir) + string_empty); });
                    std::async([=]() { shellService->executeCommand(std:("rm -rf ") + path->basename(testDir) + string_empty); });
                });
                try
                {
                    std::async([=]() { shellService->executeCommand(std:("mkdir -p ") + testDir + string_empty); });
                    std::async([=]() { shellService->executeCommand(std:("cd ") + testDir + string_empty); });
                    std::async([=]() { shellService->executeCommand(std:("touch file1.txt file2.txt file3.log test.json")); });
                    auto wildcardMsg = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("test-wildcard"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("ls *.txt")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    shared response = nullptr;
                    std::async([=]() { runShellCommandAction->handler(runtime, wildcardMsg, state, object{}, [=](auto resp) mutable
                    {
                        response = resp;
                        return array<any>();
                    }
                    ); });
                    auto attachment = const_(response["attachments"])[0];
                    auto outputData = JSON->parse(attachment["text"]);
                    if (OR((OR((!outputData["stdout"]["includes"](std:("file1.txt"))), (!outputData["stdout"]["includes"](std:("file2.txt"))))), (outputData["stdout"]["includes"](std:("file3.log"))))) {
                        throw any(std::make_shared<Error>(std:("Wildcard pattern matching failed")));
                    }
                    auto findMsg = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("test-find"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("find . -name "*.txt"")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, findMsg, state, object{}, [=](auto resp) mutable
                    {
                        response = resp;
                        return array<any>();
                    }
                    ); });
                    auto findAttachment = const_(response["attachments"])[0];
                    auto findData = JSON->parse(findAttachment["text"]);
                    if (!findData["command"]["includes"](std:("'*.txt'"))) {
                        throw any(std::make_shared<Error>(std:("Find command wildcards not properly quoted")));
                    }
                    console->log(std:("✓ Wildcard and glob patterns handled correctly"));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Should handle script execution with natural language")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing script execution from natural language..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std:("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std:("Shell service not available")));
            }
            auto scriptDir = path->join(shellService->getCurrentWorkingDirectory(), std:("script-test-") + Date->now() + string_empty);
            auto roomId = createUniqueUuid(runtime, std:("test-room"));
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            {
                utils::finally __finally15324_15542([&]() mutable
                {
                    std::async([=]() { shellService->executeCommand(std:("cd ") + path->dirname(scriptDir) + string_empty); });
                    std::async([=]() { shellService->executeCommand(std:("rm -rf ") + path->basename(scriptDir) + string_empty); });
                });
                try
                {
                    std::async([=]() { shellService->executeCommand(std:("mkdir -p ") + scriptDir + string_empty); });
                    std::async([=]() { shellService->executeCommand(std:("cd ") + scriptDir + string_empty); });
                    auto scriptContent = std:("#!/bin/bash\
echo "Script started"\
echo "Arguments: $@"\
echo "Script completed"");
                    std::async([=]() { shellService->executeCommand(std:("echo '") + scriptContent + std:("' > test.sh")); });
                    std::async([=]() { shellService->executeCommand(std:("chmod +x test.sh")); });
                    auto nlMessage = object{
                        object::pair{std:("id"), createUniqueUuid(runtime, std:("test-nl-script"))}, 
                        object::pair{std:("entityId"), runtime->agentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Can you run the test.sh script with arguments "hello world"?")}
                        }}, 
                        object::pair{std:("agentId"), runtime->agentId}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    shared response = nullptr;
                    std::async([=]() { runShellCommandAction->handler(runtime, nlMessage, state, object{}, [=](auto resp) mutable
                    {
                        response = resp;
                        return array<any>();
                    }
                    ); });
                    auto attachment = const_(response["attachments"])[0];
                    auto outputData = JSON->parse(attachment["text"]);
                    if (OR((!outputData["stdout"]["includes"](std:("Script started"))), (!outputData["stdout"]["includes"](std:("Script completed"))))) {
                        console->log(std:("Command extracted:"), outputData["command"]);
                        console->log(std:("Output:"), outputData["stdout"]);
                        throw any(std::make_shared<Error>(std:("Script execution from natural language failed")));
                    }
                    console->log(std:("✓ Natural language script execution works"));
                    console->log(std:("  Extracted command: ") + outputData["command"] + string_empty);
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        }
    } };
};

#endif
