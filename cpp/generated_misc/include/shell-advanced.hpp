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
    string name = std::string("plugin-shell-advanced-e2e");

    string description = std::string("Advanced tests for complex multi-action shell scenarios");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Should handle piped commands correctly")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing piped command execution..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto pipeMessage = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-pipe"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("echo "line1\
line2\
line3" | grep line2")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
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
            if (OR((!outputData["stdout"]["includes"](std::string("line2"))), (outputData["stdout"]["includes"](std::string("line1"))))) {
                throw any(std::make_shared<Error>(std::string("Pipe command failed. Output: ") + outputData["stdout"] + string_empty));
            }
            console->log(std::string("✓ Piped commands work correctly"));
            console->log(std::string("  Output: ") + outputData["stdout"]["trim"]() + string_empty);
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should execute compound commands with && and ||")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing compound command execution..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto successMessage = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-and-success"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("echo "first" && echo "second"")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
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
            if (OR((!outputData["stdout"]["includes"](std::string("first"))), (!outputData["stdout"]["includes"](std::string("second"))))) {
                throw any(std::make_shared<Error>(std::string("&& operator failed on success case")));
            }
            auto failMessage = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-or-fail"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("false || echo "fallback"")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runShellCommandAction->handler(runtime, failMessage, state, object{}, [=](auto resp) mutable
            {
                response = resp;
                return array<any>();
            }
            ); });
            attachment = const_(response["attachments"])[0];
            outputData = JSON->parse(attachment["text"]);
            if (!outputData["stdout"]["includes"](std::string("fallback"))) {
                throw any(std::make_shared<Error>(std::string("|| operator failed on failure case")));
            }
            console->log(std::string("✓ Compound commands work correctly"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should handle complex multi-step workflow")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing complex multi-step workflow..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std::string("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std::string("Shell service not available")));
            }
            auto workDir = path->join(shellService->getCurrentWorkingDirectory(), std::string("workflow-") + Date->now() + string_empty);
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            {
                utils::finally __finally8323_8513([&]() mutable
                {
                    std::async([=]() { shellService->executeCommand(std::string("cd ") + path->dirname(workDir) + string_empty); });
                    std::async([=]() { shellService->executeCommand(std::string("rm -rf ") + path->basename(workDir) + string_empty); });
                });
                try
                {
                    auto createStructureMsg = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("workflow-1"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("mkdir -p ") + workDir + std::string("/{src,test,docs} && cd ") + workDir + std::string(" && echo "# My Project" > README.md")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, createStructureMsg, state, object{}, [=]() mutable
                    {
                        return array<any>();
                    }
                    ); });
                    if (!shellService->getCurrentWorkingDirectory()->includes(path->basename(workDir))) {
                        throw any(std::make_shared<Error>(std::string("Failed to change to workflow directory")));
                    }
                    auto createFilesMsg = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("workflow-2"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("echo "console.log('Hello');" > src/index.js && echo "export default {};" > src/config.js")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, createFilesMsg, state, object{}, [=]() mutable
                    {
                        return array<any>();
                    }
                    ); });
                    auto packageJsonContent = object{
                        object::pair{std::string("name"), std::string("test-project")}, 
                        object::pair{std::string("version"), std::string("1.0.0")}, 
                        object::pair{std::string("main"), std::string("src/index.js")}
                    };
                    auto createPackageMsg = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("workflow-3"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("echo '") + JSON->stringify(packageJsonContent, nullptr, 2) + std::string("' > package.json")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, createPackageMsg, state, object{}, [=]() mutable
                    {
                        return array<any>();
                    }
                    ); });
                    auto verifyMsg = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("workflow-4"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("find . -type f -name "*.js" -o -name "*.json" -o -name "*.md" | sort")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
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
                    auto expectedFiles = array<string>{ std::string("./README.md"), std::string("./package.json"), std::string("./src/config.js"), std::string("./src/index.js") };
                    auto actualFiles = outputData["stdout"]["trim"]()["split"](std::string("\
"))["sort"]();
                    for (auto& expectedFile : expectedFiles)
                    {
                        if (!actualFiles["some"]([=](auto f) mutable
                        {
                            return f["includes"](expectedFile->replace(std::string("./"), string_empty));
                        }
                        )) {
                            throw any(std::make_shared<Error>(std::string("Expected file not found: ") + expectedFile + string_empty));
                        }
                    }
                    console->log(std::string("✓ Complex workflow completed successfully"));
                    console->log(std::string("  Created ") + actualFiles["length"] + std::string(" files"));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should handle background processes and job control")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing background process handling..."));
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto bgMessage = object{
                object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-bg"))}, 
                object::pair{std::string("entityId"), runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("sleep 2 & echo "Process started in background"")}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("createdAt"), Date->now()}
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
            if (!outputData["stdout"]["includes"](std::string("Process started in background"))) {
                throw any(std::make_shared<Error>(std::string("Background process test failed")));
            }
            console->log(std::string("✓ Background process handling works"));
            console->log(std::string("  Note: Full job control requires interactive shell"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should handle wildcards and glob patterns correctly")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing wildcard and glob pattern handling..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std::string("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std::string("Shell service not available")));
            }
            auto testDir = path->join(shellService->getCurrentWorkingDirectory(), std::string("glob-test-") + Date->now() + string_empty);
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            {
                utils::finally __finally12679_12869([&]() mutable
                {
                    std::async([=]() { shellService->executeCommand(std::string("cd ") + path->dirname(testDir) + string_empty); });
                    std::async([=]() { shellService->executeCommand(std::string("rm -rf ") + path->basename(testDir) + string_empty); });
                });
                try
                {
                    std::async([=]() { shellService->executeCommand(std::string("mkdir -p ") + testDir + string_empty); });
                    std::async([=]() { shellService->executeCommand(std::string("cd ") + testDir + string_empty); });
                    std::async([=]() { shellService->executeCommand(std::string("touch file1.txt file2.txt file3.log test.json")); });
                    auto wildcardMsg = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-wildcard"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("ls *.txt")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
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
                    if (OR((OR((!outputData["stdout"]["includes"](std::string("file1.txt"))), (!outputData["stdout"]["includes"](std::string("file2.txt"))))), (outputData["stdout"]["includes"](std::string("file3.log"))))) {
                        throw any(std::make_shared<Error>(std::string("Wildcard pattern matching failed")));
                    }
                    auto findMsg = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-find"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("find . -name "*.txt"")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    };
                    std::async([=]() { runShellCommandAction->handler(runtime, findMsg, state, object{}, [=](auto resp) mutable
                    {
                        response = resp;
                        return array<any>();
                    }
                    ); });
                    auto findAttachment = const_(response["attachments"])[0];
                    auto findData = JSON->parse(findAttachment["text"]);
                    if (!findData["command"]["includes"](std::string("'*.txt'"))) {
                        throw any(std::make_shared<Error>(std::string("Find command wildcards not properly quoted")));
                    }
                    console->log(std::string("✓ Wildcard and glob patterns handled correctly"));
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Should handle script execution with natural language")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing script execution from natural language..."));
            auto shellService = runtime->getService<std::shared_ptr<ShellService>>(std::string("SHELL"));
            if (!shellService) {
                throw any(std::make_shared<Error>(std::string("Shell service not available")));
            }
            auto scriptDir = path->join(shellService->getCurrentWorkingDirectory(), std::string("script-test-") + Date->now() + string_empty);
            auto roomId = createUniqueUuid(runtime, std::string("test-room"));
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            {
                utils::finally __finally15324_15542([&]() mutable
                {
                    std::async([=]() { shellService->executeCommand(std::string("cd ") + path->dirname(scriptDir) + string_empty); });
                    std::async([=]() { shellService->executeCommand(std::string("rm -rf ") + path->basename(scriptDir) + string_empty); });
                });
                try
                {
                    std::async([=]() { shellService->executeCommand(std::string("mkdir -p ") + scriptDir + string_empty); });
                    std::async([=]() { shellService->executeCommand(std::string("cd ") + scriptDir + string_empty); });
                    auto scriptContent = std::string("#!/bin/bash\
echo "Script started"\
echo "Arguments: $@"\
echo "Script completed"");
                    std::async([=]() { shellService->executeCommand(std::string("echo '") + scriptContent + std::string("' > test.sh")); });
                    std::async([=]() { shellService->executeCommand(std::string("chmod +x test.sh")); });
                    auto nlMessage = object{
                        object::pair{std::string("id"), createUniqueUuid(runtime, std::string("test-nl-script"))}, 
                        object::pair{std::string("entityId"), runtime->agentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Can you run the test.sh script with arguments "hello world"?")}
                        }}, 
                        object::pair{std::string("agentId"), runtime->agentId}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("createdAt"), Date->now()}
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
                    if (OR((!outputData["stdout"]["includes"](std::string("Script started"))), (!outputData["stdout"]["includes"](std::string("Script completed"))))) {
                        console->log(std::string("Command extracted:"), outputData["command"]);
                        console->log(std::string("Output:"), outputData["stdout"]);
                        throw any(std::make_shared<Error>(std::string("Script execution from natural language failed")));
                    }
                    console->log(std::string("✓ Natural language script execution works"));
                    console->log(std::string("  Extracted command: ") + outputData["command"] + string_empty);
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
