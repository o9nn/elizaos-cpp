#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PERSONALITY_SRC___TESTS___E2E_AGENT_INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_PERSONALITY_SRC___TESTS___E2E_AGENT_INTEGRATION_TEST_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;

class AgentIntegrationTestSuite;

class AgentIntegrationTestSuite : public TestSuite, public std::enable_shared_from_this<AgentIntegrationTestSuite> {
public:
    using std::enable_shared_from_this<AgentIntegrationTestSuite>::shared_from_this;
    string name = std:("agent-integration");

    string description = std:("Integration tests for self-modification plugin with packages/agent autonomy loop");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Plugin loads correctly in agent runtime")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing plugin loading in agent runtime..."));
            auto pluginNames = runtime["plugins"]["map"]([=](auto p) mutable
            {
                return p["name"];
            }
            );
            auto hasSelfModPlugin = pluginNames["includes"](std:("@elizaos/plugin-personality"));
            if (!hasSelfModPlugin) {
                throw any(std::make_shared<Error>(std:("Self-modification plugin not loaded in runtime")));
            }
            auto hasSQL = pluginNames["includes"](std:("@elizaos/plugin-sql"));
            if (!hasSQL) {
                throw any(std::make_shared<Error>(std:("SQL plugin dependency not available")));
            }
            auto hasAutonomous = pluginNames["includes"](std:("@elizaos/plugin-autonomy"));
            if (!hasAutonomous) {
                console->log(std:("Warning: Autonomous plugin not loaded, autonomy loop integration limited"));
            }
            console->log(std:("✅ Plugin loading test PASSED"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Character evolution works with agent character structure")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing character evolution with agent character..."));
            auto originalCharacter = utils::assign(object{
            }, runtime["character"]);
            auto originalBioLength = (Array->isArray(originalCharacter["bio"])) ? any(originalCharacter["bio"]["length"]) (1);
            auto originalTopicsLength = OR((originalCharacter["topics"]["length"]), (0));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto messages = array<object>{ object{
                object::pair{std:("id"), asUUID(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("You should remember that you are particularly good at helping with startup strategy")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            }, object{
                object::pair{std:("id"), asUUID(uuidv4())}, 
                object::pair{std:("entityId"), runtime["agentId"]}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Thank you for the feedback. I do enjoy analyzing business models and market opportunities.")}, 
                    object::pair{std:("source"), std:("agent")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 1000}
            } };
            for (auto& message : messages)
            {
                std::async([=]() { runtime["createMemory"](message, std:("messages")); });
            }
            auto modificationMessage = object{
                object::pair{std:("id"), asUUID(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Add "startup mentoring" to your bio and "business strategy" to your topics")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now() + 2000}
            };
            auto action = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std:("MODIFY_CHARACTER");
            }
            );
            if (!action) {
                throw any(std::make_shared<Error>(std:("MODIFY_CHARACTER action not available")));
            }
            shared modificationExecuted = false;
            auto callback = [=](auto content) mutable
            {
                if (content["actions"]["includes"](std:("MODIFY_CHARACTER"))) {
                    modificationExecuted = true;
                }
                return array<any>();
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto result = std::async([=]() { action["handler"](runtime, modificationMessage, state, object{}, callback); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std:("Character modification failed: ") + (OR((result["error"]), (result["reason"]))) + string_empty));
            }
            if (!modificationExecuted) {
                throw any(std::make_shared<Error>(std:("Modification action was not executed")));
            }
            auto updatedCharacter = runtime["character"];
            auto newBioLength = (Array->isArray(updatedCharacter["bio"])) ? any(updatedCharacter["bio"]["length"]) (1);
            auto newTopicsLength = OR((updatedCharacter["topics"]["length"]), (0));
            auto bioExpanded = newBioLength > originalBioLength;
            auto topicsExpanded = newTopicsLength > originalTopicsLength;
            if (AND((!bioExpanded), (!topicsExpanded))) {
                throw any(std::make_shared<Error>(std:("Character was not modified as expected")));
            }
            auto bioText = (Array->isArray(updatedCharacter["bio"])) ? updatedCharacter["bio"]["join"](std:(" ")) : updatedCharacter["bio"];
            auto hasStartupContent = OR((bioText["toLowerCase"]()["includes"](std:("startup"))), (bioText["toLowerCase"]()["includes"](std:("mentor"))));
            auto hasBusinessTopic = updatedCharacter["topics"]["some"]([=](auto topic) mutable
            {
                return OR((topic->toLowerCase()->includes(std:("business"))), (topic->toLowerCase()->includes(std:("strategy"))));
            }
            );
            if (AND((!hasStartupContent), (!hasBusinessTopic))) {
                throw any(std::make_shared<Error>(std:("Expected content was not added to character")));
            }
            console->log(std:("Character successfully evolved:"));
            console->log(std:("- Bio elements: ") + originalBioLength + std:(" → ") + newBioLength + string_empty);
            console->log(std:("- Topics: ") + originalTopicsLength + std:(" → ") + newTopicsLength + string_empty);
            console->log(std:("✅ Character evolution integration test PASSED"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Evolution provider integrates with agent context")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing evolution provider integration..."));
            auto roomId = uuidv4();
            auto message = object{
                object::pair{std:("id"), asUUID(uuidv4())}, 
                object::pair{std:("entityId"), uuidv4()}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Tell me about your evolution capabilities")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto state = std::async([=]() { runtime["composeState"](message, array<string>{ std:("CHARACTER_EVOLUTION") }); });
            if (!state) {
                throw any(std::make_shared<Error>(std:("State composition failed")));
            }
            auto hasEvolutionContext = state["text"]["includes"](std:("CHARACTER EVOLUTION CONTEXT"));
            if (!hasEvolutionContext) {
                throw any(std::make_shared<Error>(std:("Evolution provider did not provide context")));
            }
            if (type_of(state["values"]["hasEvolutionCapability"]) != std:("boolean")) {
                throw any(std::make_shared<Error>(std:("Evolution capability flag not set")));
            }
            auto character = runtime["character"];
            auto expectedBioCount = (Array->isArray(character["bio"])) ? any(character["bio"]["length"]) (1);
            auto expectedTopicCount = OR((character["topics"]["length"]), (0));
            if (!state["text"]["includes"](std:("Bio elements: ") + expectedBioCount + string_empty)) {
                throw any(std::make_shared<Error>(std:("Bio count in evolution context is incorrect")));
            }
            if (!state["text"]["includes"](std:("Topics: ") + expectedTopicCount + string_empty)) {
                throw any(std::make_shared<Error>(std:("Topic count in evolution context is incorrect")));
            }
            console->log(std:("✅ Evolution provider integration test PASSED"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("File manager integrates with agent character file")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing file manager integration..."));
            auto fileManager = runtime["getService"](std:("character-file-manager"));
            if (!fileManager) {
                throw any(std::make_shared<Error>(std:("Character file manager service not available")));
            }
            auto backupPath = std::async([=]() { fileManager["createBackup"](); });
            console->log(std:("Backup result:"), (backupPath) ? std:("Success") : std:("No file detected (expected in test)"));
            auto validModification = object{
                object::pair{std:("bio"), array<string>{ std:("Expert in autonomous agent development") }}, 
                object::pair{std:("topics"), array<string>{ std:("artificial intelligence"), std:("agent autonomy") }}
            };
            auto validation = fileManager["validateModification"](validModification);
            if (!validation["valid"]) {
                throw any(std::make_shared<Error>(std:("Valid modification was rejected: ") + validation["errors"]["join"](std:(", ")) + string_empty));
            }
            auto applyResult = std::async([=]() { fileManager["applyModification"](validModification); });
            if (!applyResult["success"]) {
                throw any(std::make_shared<Error>(std:("Modification application failed: ") + applyResult["error"] + string_empty));
            }
            auto character = runtime["character"];
            auto bioText = (Array->isArray(character["bio"])) ? character["bio"]["join"](std:(" ")) : character["bio"];
            auto hasAgentContent = OR((bioText["toLowerCase"]()["includes"](std:("autonomous"))), (bioText["toLowerCase"]()["includes"](std:("agent"))));
            if (!hasAgentContent) {
                throw any(std::make_shared<Error>(std:("Character modification was not applied to runtime")));
            }
            console->log(std:("✅ File manager integration test PASSED"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Autonomy loop triggers character evolution")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("Testing autonomy loop integration..."));
            auto hasAutonomous = runtime["plugins"]["some"]([=](auto p) mutable
            {
                return p["name"] == std:("@elizaos/plugin-autonomy");
            }
            );
            if (!hasAutonomous) {
                console->log(std:("Autonomous plugin not available, skipping autonomy integration test"));
                return std::shared_ptr<Promise<void>>();
            }
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto interactionPattern = array<string>{ std:("I love how you think about business strategy"), std:("Your insights on startups are really valuable"), std:("You should remember that you excel at market analysis"), std:("Maybe add "strategic thinking" to your personality traits") };
            for (auto i = 0; i < interactionPattern->get_length(); i++)
            {
                auto message = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("entityId"), userId}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), const_(interactionPattern)[i]}, 
                        object::pair{std:("source"), std:("test")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now() + i * 1000}
                };
                std::async([=]() { runtime["createMemory"](message, std:("messages")); });
                auto agentResponse = object{
                    object::pair{std:("id"), asUUID(uuidv4())}, 
                    object::pair{std:("entityId"), runtime["agentId"]}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("I appreciate the feedback about ") + (i == 0) ? any(std:("business strategy")) ((i == 1) ? any(std:("startup insights")) ((i == 2) ? std:("market analysis") : std:("strategic thinking"))) + std:(".")}, 
                        object::pair{std:("source"), std:("agent")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now() + i * 1000 + 500}
                };
                std::async([=]() { runtime["createMemory"](agentResponse, std:("messages")); });
            }
            std::async([=]() { runtime["setCache"](std:("character-evolution:last-check"), std:("0")); });
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{
                    object::pair{std:("messageCount"), interactionPattern->get_length() * 2}
                }}, 
                object::pair{std:("text"), string_empty}
            };
            auto evaluator = runtime["evaluators"]["find"]([=](auto e) mutable
            {
                return e["name"] == std:("CHARACTER_EVOLUTION");
            }
            );
            if (!evaluator) {
                throw any(std::make_shared<Error>(std:("CHARACTER_EVOLUTION evaluator not found")));
            }
            auto lastMessage = object{
                object::pair{std:("id"), asUUID(uuidv4())}, 
                object::pair{std:("entityId"), userId}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), const_(interactionPattern)[interactionPattern->get_length() - 1]}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto shouldEvaluate = std::async([=]() { evaluator["validate"](runtime, lastMessage, state); });
            if (shouldEvaluate) {
                std::async([=]() { evaluator["handler"](runtime, lastMessage, state); });
                console->log(std:("Evolution evaluator executed successfully"));
            } else {
                console->log(std:("Evolution evaluator did not trigger (may be expected based on content)"));
            }
            auto evolutionMemories = std::async([=]() { runtime["getMemories"](object{
                object::pair{std:("entityId"), runtime["agentId"]}, 
                object::pair{std:("roomId"), std:("roomId")}, 
                object::pair{std:("count"), 5}, 
                object::pair{std:("tableName"), std:("character_evolution")}
            }); });
            if (evolutionMemories["length"] > 0) {
                console->log(std:("Found ") + evolutionMemories["length"] + std:(" evolution suggestion(s)"));
                auto suggestion = const_(evolutionMemories)[0];
                console->log(std:("Evolution reasoning:"), suggestion["content"]["metadata"]["evolution"]["reasoning"]);
            }
            console->log(std:("✅ Autonomy loop integration test PASSED"));
        }
        }
    } };
};

#endif
