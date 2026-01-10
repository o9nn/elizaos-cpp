#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PERSONALITY_SRC___TESTS___E2E_SELF-MODIFICATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PERSONALITY_SRC___TESTS___E2E_SELF-MODIFICATION_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class SelfModificationTestSuite;

class SelfModificationTestSuite : public TestSuite, public std::enable_shared_from_this<SelfModificationTestSuite> {
public:
    using std::enable_shared_from_this<SelfModificationTestSuite>::shared_from_this;
    string name = std::string("self-modification-e2e");

    string description = std::string("End-to-end tests for agent self-modification and character evolution");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Plugin initialization and service availability")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing plugin initialization..."));
            auto fileManager = runtime["getService"](std::string("character-file-manager"));
            if (!fileManager) {
                throw any(std::make_shared<Error>(std::string("CharacterFileManager service not available")));
            }
            auto modifyAction = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std::string("MODIFY_CHARACTER");
            }
            );
            if (!modifyAction) {
                throw any(std::make_shared<Error>(std::string("MODIFY_CHARACTER action not registered")));
            }
            auto evolutionEvaluator = runtime["evaluators"]["find"]([=](auto e) mutable
            {
                return e["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            if (!evolutionEvaluator) {
                throw any(std::make_shared<Error>(std::string("CHARACTER_EVOLUTION evaluator not registered")));
            }
            auto evolutionProvider = runtime["providers"]["find"]([=](auto p) mutable
            {
                return p["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            if (!evolutionProvider) {
                throw any(std::make_shared<Error>(std::string("CHARACTER_EVOLUTION provider not registered")));
            }
            console->log(std::string("✅ Plugin initialization test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Character evolution evaluator triggers correctly")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing character evolution evaluator..."));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto messages = array<object>{ object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("You should be more encouraging when helping people learn")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            }, object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), runtime["agentId"]}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("I understand. I want to be more supportive.")}, 
                    object::pair{std::string("source"), std::string("agent")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            }, object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Yes, maybe add more positive reinforcement to your responses")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 2000}
            } };
            for (auto& message : messages)
            {
                std::async([=]() { runtime["createMemory"](message, std::string("messages")); });
            }
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("messageCount"), messages->get_length()}
                }}, 
                object::pair{std::string("text"), string_empty}
            };
            auto evaluator = runtime["evaluators"]["find"]([=](auto e) mutable
            {
                return e["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            auto shouldRun = std::async([=]() { evaluator["validate"](runtime, const_(messages)[2], state); });
            if (!shouldRun) {
                throw any(std::make_shared<Error>(std::string("Evolution evaluator should have triggered on encouraging feedback")));
            }
            console->log(std::string("✅ Character evolution evaluator test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("MODIFY_CHARACTER action handles user requests")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing MODIFY_CHARACTER action with user request..."));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto message = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add machine learning to your list of topics you know about")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto action = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std::string("MODIFY_CHARACTER");
            }
            );
            auto isValid = std::async([=]() { action["validate"](runtime, message, state); });
            if (!isValid) {
                throw any(std::make_shared<Error>(std::string("MODIFY_CHARACTER action should validate for topic addition request")));
            }
            shared responseReceived = false;
            auto callback = [=](auto content) mutable
            {
                if (content["actions"]["includes"](std::string("MODIFY_CHARACTER"))) {
                    responseReceived = true;
                    console->log(std::string("Action response:"), content["text"]);
                }
                return array<any>();
            };
            auto result = std::async([=]() { action["handler"](runtime, message, state, object{}, callback); });
            if (!responseReceived) {
                throw any(std::make_shared<Error>(std::string("MODIFY_CHARACTER action did not execute properly")));
            }
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("MODIFY_CHARACTER action failed: ") + (OR((result["error"]), (result["reason"]))) + string_empty));
            }
            auto topics = OR((runtime["character"]["topics"]), (array<any>()));
            if (!topics["includes"](std::string("machine learning"))) {
                throw any(std::make_shared<Error>(std::string("Character topics were not updated with machine learning")));
            }
            console->log(std::string("✅ MODIFY_CHARACTER action test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("CHARACTER_EVOLUTION provider supplies context")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing CHARACTER_EVOLUTION provider..."));
            auto roomId = asUUID(uuidv4());
            auto message = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), asUUID(uuidv4())}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Tell me about your evolution capabilities")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto provider = runtime["providers"]["find"]([=](auto p) mutable
            {
                return p["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            auto result = std::async([=]() { provider["get"](runtime, message, state); });
            if (!result) {
                throw any(std::make_shared<Error>(std::string("CHARACTER_EVOLUTION provider returned no result")));
            }
            if (OR((!result["text"]), (!result["text"]["includes"](std::string("CHARACTER EVOLUTION CONTEXT"))))) {
                throw any(std::make_shared<Error>(std::string("Provider did not return expected evolution context")));
            }
            if (!result["values"]["hasEvolutionCapability"]) {
                throw any(std::make_shared<Error>(std::string("Provider should indicate evolution capability is available")));
            }
            console->log(std::string("✅ CHARACTER_EVOLUTION provider test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Character file manager validates modifications safely")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing character file manager validation..."));
            auto fileManager = runtime["getService"](std::string("character-file-manager"));
            auto validModification = object{
                object::pair{std::string("bio"), array<string>{ std::string("Interested in helping people learn new technologies") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("education"), std::string("learning") }}
            };
            auto validResult = fileManager["validateModification"](validModification);
            if (!validResult["valid"]) {
                throw any(std::make_shared<Error>(std::string("Valid modification was rejected: ") + validResult["errors"]["join"](std::string(", ")) + string_empty));
            }
            auto invalidModification = object{
                object::pair{std::string("bio"), array<string>{ std::string("<script>alert("xss")</script>") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("javascript:void(0)") }}
            };
            auto invalidResult = fileManager["validateModification"](invalidModification);
            if (invalidResult["valid"]) {
                throw any(std::make_shared<Error>(std::string("Invalid modification with XSS was accepted")));
            }
            auto excessiveModification = object{
                object::pair{std::string("bio"), ((array(25)))->fill(std::string("Too many bio elements"))}
            };
            auto excessiveResult = fileManager["validateModification"](excessiveModification);
            if (excessiveResult["valid"]) {
                throw any(std::make_shared<Error>(std::string("Excessive modification was accepted")));
            }
            console->log(std::string("✅ Character file manager validation test PASSED"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("End-to-end character evolution workflow")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("Testing complete character evolution workflow..."));
            auto roomId = asUUID(uuidv4());
            auto userId = asUUID(uuidv4());
            auto baselineTopics = array<any>{ (OR((runtime["character"]["topics"]), (array<any>()))) };
            auto baselineBio = (Array->isArray(runtime["character"]["bio"])) ? array<any>{ runtime["character"]["bio"] } : array<any>{ runtime["character"]["bio"] };
            auto feedbackMessage = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("You should remember that you are particularly good at explaining complex topics in simple terms")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            std::async([=]() { runtime["createMemory"](feedbackMessage, std::string("messages")); });
            auto agentResponse = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), runtime["agentId"]}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Thank you for the feedback. I will remember to focus on clear, simple explanations.")}, 
                    object::pair{std::string("source"), std::string("agent")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 1000}
            };
            std::async([=]() { runtime["createMemory"](agentResponse, std::string("messages")); });
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("messageCount"), 5}
                }}, 
                object::pair{std::string("text"), string_empty}
            };
            auto evaluator = runtime["evaluators"]["find"]([=](auto e) mutable
            {
                return e["name"] == std::string("CHARACTER_EVOLUTION");
            }
            );
            std::async([=]() { runtime["setCache"](std::string("character-evolution:last-check"), std::string("0")); });
            auto shouldEvaluate = std::async([=]() { evaluator["validate"](runtime, feedbackMessage, state); });
            if (shouldEvaluate) {
                std::async([=]() { evaluator["handler"](runtime, feedbackMessage, state); });
            }
            auto evolutionSuggestions = std::async([=]() { runtime["getMemories"](object{
                object::pair{std::string("entityId"), runtime["agentId"]}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("count"), 5}, 
                object::pair{std::string("tableName"), std::string("character_evolution")}
            }); });
            auto modificationMessage = object{
                object::pair{std::string("id"), asUUID(uuidv4())}, 
                object::pair{std::string("entityId"), userId}, 
                object::pair{std::string("roomId"), std::string("roomId")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Add "clear explanations" to your bio and "educational communication" to your topics")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now() + 2000}
            };
            auto action = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std::string("MODIFY_CHARACTER");
            }
            );
            shared modificationApplied = false;
            auto callback = [=](auto content) mutable
            {
                if (content["actions"]["includes"](std::string("MODIFY_CHARACTER"))) {
                    modificationApplied = true;
                }
                return array<any>();
            };
            auto result = std::async([=]() { action["handler"](runtime, modificationMessage, state, object{}, callback); });
            if (!result["success"]) {
                throw any(std::make_shared<Error>(std::string("Character modification failed: ") + (OR((result["error"]), (result["reason"]))) + string_empty));
            }
            auto newTopics = OR((runtime["character"]["topics"]), (array<any>()));
            auto newBio = (Array->isArray(runtime["character"]["bio"])) ? any(runtime["character"]["bio"]) : any(array<any>{ runtime["character"]["bio"] });
            auto hasNewTopic = newTopics["some"]([=](auto topic) mutable
            {
                return OR((topic->includes(std::string("educational"))), (topic->includes(std::string("communication"))));
            }
            );
            auto hasNewBio = newBio["some"]([=](auto bioItem) mutable
            {
                return OR((bioItem->includes(std::string("clear"))), (bioItem->includes(std::string("explanation"))));
            }
            );
            if (AND((!hasNewTopic), (!hasNewBio))) {
                throw any(std::make_shared<Error>(std::string("Character was not modified as expected")));
            }
            if (!modificationApplied) {
                throw any(std::make_shared<Error>(std::string("Modification was not properly executed")));
            }
            console->log(std::string("Character successfully evolved:"));
            console->log(std::string("- Baseline topics:"), baselineTopics->get_length());
            console->log(std::string("- New topics:"), newTopics["length"]);
            console->log(std::string("- Baseline bio elements:"), baselineBio->get_length());
            console->log(std::string("- New bio elements:"), newBio["length"]);
            console->log(std::string("✅ End-to-end character evolution workflow test PASSED"));
        }
        }
    } };
};

#endif
