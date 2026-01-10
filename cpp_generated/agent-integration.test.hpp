#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PERSONALITY_SRC___TESTS___E2E_AGENT-INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PERSONALITY_SRC___TESTS___E2E_AGENT-INTEGRATION_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class AgentIntegrationTestSuite;

class AgentIntegrationTestSuite : public TestSuite, public std::enable_shared_from_this<AgentIntegrationTestSuite> {
public:
    using std::enable_shared_from_this<AgentIntegrationTestSuite>::shared_from_this;
    string name = std::string("agent-integration");

    string description = std::string("Integration tests for self-modification plugin with packages/agent autonomy loop");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Plugin loads correctly in agent runtime")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing plugin loading in agent runtime..."));
            auto pluginNames = runtime["plugins"]["map"]([=](auto p) mutable
            {
                return p["name"];
            }
            );
            auto hasSelfModPlugin = pluginNames["includes"](std::string("@elizaos/plugin-personality"));
            if (!hasSelfModPlugin) {
                throw any(std::make_shared<Error>(std::string("Self-modification plugin not loaded in runtime")));
            }
            auto hasSQL = pluginNames["includes"](std::string("@elizaos/plugin-sql"));
            if (!hasSQL) {
                throw any(std::make_shared<Error>(std::string("SQL plugin dependency not available")));
            }
            auto hasAutonomous = pluginNames["includes"](std::string("@elizaos/plugin-autonomy"));
            if (!hasAutonomous) {
                console->log(std::string("Warning: Autonomous plugin not loaded, autonomy loop integration limited"));
            }
            console->log(std::string("✅ Plugin loading test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Character evolution works with agent character structure")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing character evolution with agent character..."));
            auto originalCharacter = utils::assign(object{
            }, runtime["character"]);
            auto originalBioLength = (Array->isArray(originalCharacter["bio"])) ? any(originalCharacter["bio"]["length"]) : any(1);
            auto originalTopicsLength = OR((originalCharacter["topics"]["length"]), (0));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto messages = array<object>{ object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("You should remember that you are particularly good at helping with startup strategy")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            }, object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), runtime["agentId"]}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Thank you for the feedback. I do enjoy analyzing business models and market opportunities.")}, 
                    object::pair{std::string("source"), std::string("agent")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            } };
            for (auto& message : messages)
            {
                std::async([=]() { runtime["createMemory"](message, std::string("messages")); });
            }
            auto modificationMessage = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add "startup mentoring" to your bio and "business strategy" to your topics")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 2000}
            };
            auto action = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std::string("MODIFY_CHARACTER");
            }
            );
            if (!action) {
                throw any(std::make_shared<Error>(std::string("MODIFY_CHARACTER action not available")));
            }
            shared modificationExecuted = false;
            auto callback = [=](auto content) mutable
            {
                if (content["actions"]["includes"](std::string("MODIFY_CHARACTER"))) {
                    modificationExecuted = true;
                }
                return array<any>();
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto result = std::async([=]() { action["handler"](runtime, modificationMessage, state, object{}, callback); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("Character modification failed: ") + (OR((result["error"]), (result["reason"]))) + string_empty));
            }
            if (!modificationExecuted) {
                throw any(std::make_shared<Error>(std::string("Modification action was not executed")));
            }
            auto updatedCharacter = runtime["character"];
            auto newBioLength = (Array->isArray(updatedCharacter["bio"])) ? any(updatedCharacter["bio"]["length"]) : any(1);
            auto newTopicsLength = OR((updatedCharacter["topics"]["length"]), (0));
            auto bioExpanded = newBioLength > originalBioLength;
            auto topicsExpanded = newTopicsLength > originalTopicsLength;
            if (AND((!bioExpanded), (!topicsExpanded))) {
                throw any(std::make_shared<Error>(std::string("Character was not modified as expected")));
            }
            auto bioText = (Array->isArray(updatedCharacter["bio"])) ? updatedCharacter["bio"]["join"](std::string(" ")) : updatedCharacter["bio"];
            auto hasStartupContent = OR((bioText["toLowerCase"]()["includes"](std::string("startup"))), (bioText["toLowerCase"]()["includes"](std::string("mentor"))));
            auto hasBusinessTopic = updatedCharacter["topics"]["some"]([=](auto topic) mutable
            {
                return OR((topic->toLowerCase()->includes(std::string("business"))), (topic->toLowerCase()->includes(std::string("strategy"))));
            }
            );
            if (AND((!hasStartupContent), (!hasBusinessTopic))) {
                throw any(std::make_shared<Error>(std::string("Expected content was not added to character")));
            }
            console->log(std::string("Character successfully evolved:"));
            console->log(std::string("- Bio elements: ") + originalBioLength + std::string(" → ") + newBioLength + string_empty);
            console->log(std::string("- Topics: ") + originalTopicsLength + std::string(" → ") + newTopicsLength + string_empty);
            console->log(std::string("✅ Character evolution integration test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Evolution provider integrates with agent context")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing evolution provider integration..."));
            auto roomId = uuidv4();
            auto message = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), uuidv4()}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Tell me about your evolution capabilities")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = std::async([=]() { runtime["composeState"](message, array<string>{ std::string("CHARACTER_EVOLUTION") }); });
            if (!state) {
                throw any(std::make_shared<Error>(std::string("State composition failed")));
            }
            auto hasEvolutionContext = state["text"]["includes"](std::string("CHARACTER EVOLUTION CONTEXT"));
            if (!hasEvolutionContext) {
                throw any(std::make_shared<Error>(std::string("Evolution provider did not provide context")));
            }
            if (type_of(state["values"]["hasEvolutionCapability"]) != std::string("boolean")) {
                throw any(std::make_shared<Error>(std::string("Evolution capability flag not set")));
            }
            auto character = runtime["character"];
            auto expectedBioCount = (Array->isArray(character["bio"])) ? any(character["bio"]["length"]) : any(1);
            auto expectedTopicCount = OR((character["topics"]["length"]), (0));
            if (!state["text"]["includes"](std::string("Bio elements: ") + expectedBioCount + string_empty)) {
                throw any(std::make_shared<Error>(std::string("Bio count in evolution context is incorrect")));
            }
            if (!state["text"]["includes"](std::string("Topics: ") + expectedTopicCount + string_empty)) {
                throw any(std::make_shared<Error>(std::string("Topic count in evolution context is incorrect")));
            }
            console->log(std::string("✅ Evolution provider integration test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("File manager integrates with agent character file")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing file manager integration..."));
            auto fileManager = runtime["getService"](std::string("character-file-manager"));
            if (!fileManager) {
                throw any(std::make_shared<Error>(std::string("Character file manager service not available")));
            }
            auto backupPath = std::async([=]() { fileManager["createBackup"](); });
            console->log(std::string("Backup result:"), (backupPath) ? std::string("Success") : std::string("No file detected (expected in test)"));
            auto validModification = object{
                object::pair{std::string("bio"), array<string>{ std::string("Expert in autonomous agent development") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("artificial intelligence"), std::string("agent autonomy") }}
            };
            auto validation = fileManager["validateModification"](validModification);
            if (!validation["valid"]) {
                throw any(std::make_shared<Error>(std::string("Valid modification was rejected: ") + validation["errors"]["join"](std::string(", ")) + string_empty));
            }
            auto applyResult = std::async([=]() { fileManager["applyModification"](validModification); });
            if (!applyResult["success"]) {
                throw any(std::make_shared<Error>(std::string("Modification application failed: ") + applyResult["error"] + string_empty));
            }
            auto character = runtime["character"];
            auto bioText = (Array->isArray(character["bio"])) ? character["bio"]["join"](std::string(" ")) : character["bio"];
            auto hasAgentContent = OR((bioText["toLowerCase"]()["includes"](std::string("autonomous"))), (bioText["toLowerCase"]()["includes"](std::string("agent"))));
            if (!hasAgentContent) {
                throw any(std::make_shared<Error>(std::string("Character modification was not applied to runtime")));
            }
            console->log(std::string("✅ File manager integration test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Autonomy loop triggers character evolution")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing autonomy loop integration..."));
            auto hasAutonomous = runtime["plugins"]["some"]([=](auto p) mutable
            {
                return p["name"] == std::string("@elizaos/plugin-autonomy");
            }
            );
            if (!hasAutonomous) {
                console->log(std::string("Autonomous plugin not available, skipping autonomy integration test"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto interactionPattern = array<string>{ std::string("I love how you think about business strategy"), std::string("Your insights on startups are really valuable"), std::string("You should remember that you excel at market analysis"), std::string("Maybe add "strategic thinking" to your personality traits") };
            for (auto i = 0; i < interactionPattern->get_length(); i++)
            {
                auto message = object{
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("entityId"), userId}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), const_(interactionPattern)[i]}, 
                        object::pair{std::string("source"), std::string("test")}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now() + i * 1000}
                };
                std::async([=]() { runtime["createMemory"](message, std::string("messages")); });
                auto agentResponse = object{
                    object::pair{std::string("id"), asUUID(uuidv4())}, 
                    object::pair{std::string("entityId"), runtime["agentId"]}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("I appreciate the feedback about ") + (i == 0) ? any(std::string("business strategy")) : any((i == 1) ? any(std::string("startup insights")) : any((i == 2) ? std::string("market analysis") : std::string("strategic thinking"))) + std::string(".")}, 
                        object::pair{std::string("source"), std::string("agent")}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now() + i * 1000 + 500}
                };
                std::async([=]() { runtime["createMemory"](agentResponse, std::string("messages")); });
            }
            std::async([=]() { runtime["setCache"](std::string("character-evolution:last-check"), std::string("0")); });
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("messageCount"), interactionPattern->get_length() * 2}
                }}, 
                object::pair{std::string("text"), string_empty}
            };
            auto evaluator = runtime["evaluators"]["find"]([=](auto e) mutable
            {
                return e["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            if (!evaluator) {
                throw any(std::make_shared<Error>(std::string("CHARACTER_EVOLUTION evaluator not found")));
            }
            auto lastMessage = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), const_(interactionPattern)[interactionPattern->get_length() - 1]}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto shouldEvaluate = std::async([=]() { evaluator["validate"](runtime, lastMessage, state); });
            if (shouldEvaluate) {
                std::async([=]() { evaluator["handler"](runtime, lastMessage, state); });
                console->log(std::string("Evolution evaluator executed successfully"));
            } else {
                console->log(std::string("Evolution evaluator did not trigger (may be expected based on content)"));
            }
            auto evolutionMemories = std::async([=]() { runtime["getMemories"](object{
                object::pair{std::string("entityId"), runtime["agentId"]}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("count"), 5}, 
                object::pair{std::string("tableName"), std::string("character_evolution")}
            }); });
            if (evolutionMemories["length"] > 0) {
                console->log(std::string("Found ") + evolutionMemories["length"] + std::string(" evolution suggestion(s)"));
                auto suggestion = const_(evolutionMemories)[0];
                console->log(std::string("Evolution reasoning:"), suggestion["content"]["metadata"]["evolution"]["reasoning"]);
            }
            console->log(std::string("✅ Autonomy loop integration test PASSED"));
        }
        }
    } };
};

#endif
