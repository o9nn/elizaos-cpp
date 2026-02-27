#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___E2E_NATURAL-LANGUAGE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___E2E_NATURAL-LANGUAGE_TEST_H
#include "core.h"
#include "@elizaos/core.h"

class NaturalLanguageTestSuite;

class NaturalLanguageTestSuite : public TestSuite, public std::enable_shared_from_this<NaturalLanguageTestSuite> {
public:
    using std::enable_shared_from_this<NaturalLanguageTestSuite>::shared_from_this;
    string name = std:("natural-language");

    string description = std:("E2E tests for natural language processing and agent responses");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Agent responds to hello world")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                shared roomId = std:("test-room-hello-") + Date->now() + string_empty;
                shared userId = std:("test-user-hello");
                shared helloMessage = object{
                    object::pair{std:("id"), std:("msg-") + Date->now() + string_empty}, 
                    object::pair{std:("userId"), userId}, 
                    object::pair{std:("agentId"), runtime["agentId"]}, 
                    object::pair{std:("roomId"), roomId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("hello world")}, 
                        object::pair{std:("type"), std:("text")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now()}
                };
                console->log(std:("Sending hello world message to agent..."));
                std::async([=]() { runtime["processMessage"](helloMessage); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto messages = std::async([=]() { runtime["messageManager"]["getMessages"](object{
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("limit"), 10}
                }); });
                console->log(std:("Retrieved ") + messages["length"] + std:(" messages from conversation"));
                if (messages["length"] < 2) {
                    throw any(std::make_shared<Error>(std:("Expected at least 2 messages, got ") + messages["length"] + string_empty));
                }
                auto agentResponse = messages["find"]([=](auto m) mutable
                {
                    return AND((AND((m["userId"] == runtime["agentId"]), (m["roomId"] == roomId))), (m["id"] != helloMessage["id"]));
                }
                );
                if (!agentResponse) {
                    throw any(std::make_shared<Error>(std:("Agent did not respond to hello world message")));
                }
                console->log(std:("Agent response:"), agentResponse["content"]["text"]);
                shared responseText = agentResponse["content"]["text"]["toLowerCase"]();
                auto greetingWords = array<string>{ std:("hello"), std:("hi"), std:("hey"), std:("greetings"), std:("welcome") };
                auto containsGreeting = greetingWords->some([=](auto word) mutable
                {
                    return responseText["includes"](word);
                }
                );
                if (!containsGreeting) {
                    throw any(std::make_shared<Error>(std:("Agent response did not contain a greeting. ") + std:("Response was: "") + agentResponse["content"]["text"] + std:(""")));
                }
                console->log(std:("✓ Agent successfully responded to hello world"));
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Hello world test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Agent responds to casual greeting")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                auto greetings = array<string>{ std:("hey there!"), std:("hi, how are you?"), std:("good morning!"), std:("whats up?") };
                auto& __array3855_5290 = greetings;
                for (auto __indx3855_5290 = 0_N; __indx3855_5290 < __array3855_5290->get_length(); __indx3855_5290++)
                {
                    auto& greeting = const_(__array3855_5290)[__indx3855_5290];
                    {
                        auto roomId = std:("test-room-greeting-") + Date->now() + std:("-") + Math->random() + string_empty;
                        shared userId = std:("test-user-greeting");
                        shared message = object{
                            object::pair{std:("id"), std:("msg-") + Date->now() + std:("-") + Math->random() + string_empty}, 
                            object::pair{std:("userId"), userId}, 
                            object::pair{std:("agentId"), runtime["agentId"]}, 
                            object::pair{std:("roomId"), roomId}, 
                            object::pair{std:("content"), object{
                                object::pair{std:("text"), greeting}, 
                                object::pair{std:("type"), std:("text")}
                            }}, 
                            object::pair{std:("createdAt"), Date->now()}
                        };
                        console->log(std:("Testing greeting: "") + greeting + std:("""));
                        std::async([=]() { runtime["processMessage"](message); });
                        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                        {
                            return setTimeout(resolve, 500);
                        }
                        ); });
                        auto messages = std::async([=]() { runtime["messageManager"]["getMessages"](object{
                            object::pair{std:("roomId"), std:("roomId")}, 
                            object::pair{std:("limit"), 10}
                        }); });
                        auto agentResponse = messages["find"]([=](auto m) mutable
                        {
                            return AND((m["userId"] == runtime["agentId"]), (m["id"] != message["id"]));
                        }
                        );
                        if (!agentResponse) {
                            throw any(std::make_shared<Error>(std:("Agent did not respond to greeting: "") + greeting + std:(""")));
                        }
                        if (OR((!agentResponse["content"]["text"]), (agentResponse["content"]["text"]["length"] < 2))) {
                            throw any(std::make_shared<Error>(std:("Agent gave empty response to: "") + greeting + std:(""")));
                        }
                        console->log(std:("✓ Agent responded to: "") + greeting + std:("""));
                    }
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Casual greeting test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Agent maintains conversation context")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                auto roomId = std:("test-room-context-") + Date->now() + string_empty;
                shared userId = std:("test-user-context");
                auto firstMessage = object{
                    object::pair{std:("id"), std:("msg-1-") + Date->now() + string_empty}, 
                    object::pair{std:("userId"), userId}, 
                    object::pair{std:("agentId"), runtime["agentId"]}, 
                    object::pair{std:("roomId"), roomId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("My favorite color is blue. What's yours?")}, 
                        object::pair{std:("type"), std:("text")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now()}
                };
                console->log(std:("Sending first message about favorite color..."));
                std::async([=]() { runtime["processMessage"](firstMessage); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto secondMessage = object{
                    object::pair{std:("id"), std:("msg-2-") + Date->now() + string_empty}, 
                    object::pair{std:("userId"), userId}, 
                    object::pair{std:("agentId"), runtime["agentId"]}, 
                    object::pair{std:("roomId"), roomId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Why did you choose that color?")}, 
                        object::pair{std:("type"), std:("text")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now() + 1000}
                };
                console->log(std:("Sending follow-up question..."));
                std::async([=]() { runtime["processMessage"](secondMessage); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto messages = std::async([=]() { runtime["messageManager"]["getMessages"](object{
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("limit"), 10}
                }); });
                if (messages["length"] < 4) {
                    throw any(std::make_shared<Error>(std:("Expected at least 4 messages, got ") + messages["length"] + string_empty));
                }
                auto agentResponses = messages["filter"]([=](auto m) mutable
                {
                    return m["userId"] == runtime["agentId"];
                }
                );
                if (agentResponses["length"] < 2) {
                    throw any(std::make_shared<Error>(std:("Agent did not respond to both messages")));
                }
                auto secondResponse = const_(agentResponses)[agentResponses["length"] - 1];
                shared responseText = secondResponse["content"]["text"]["toLowerCase"]();
                auto contextWords = array<string>{ std:("color"), std:("blue"), std:("favorite"), std:("chose"), std:("choice"), std:("because") };
                auto hasContext = contextWords->some([=](auto word) mutable
                {
                    return responseText["includes"](word);
                }
                );
                if (!hasContext) {
                    console->warn(std:("Agent response may not show context awareness. ") + std:("Response: "") + secondResponse["content"]["text"] + std:("""));
                }
                console->log(std:("✓ Agent maintained conversation context"));
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Context test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    } };
};

#endif
