#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___E2E_NATURAL-LANGUAGE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___E2E_NATURAL-LANGUAGE_TEST_H
#include "core.h"
#include "@elizaos/core.h"

class NaturalLanguageTestSuite;

class NaturalLanguageTestSuite : public TestSuite, public std::enable_shared_from_this<NaturalLanguageTestSuite> {
public:
    using std::enable_shared_from_this<NaturalLanguageTestSuite>::shared_from_this;
    string name = std::string("natural-language");

    string description = std::string("E2E tests for natural language processing and agent responses");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Agent responds to hello world")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                shared roomId = std::string("test-room-hello-") + Date->now() + string_empty;
                shared userId = std::string("test-user-hello");
                shared helloMessage = object{
                    object::pair{std::string("id"), std::string("msg-") + Date->now() + string_empty}, 
                    object::pair{std::string("userId"), userId}, 
                    object::pair{std::string("agentId"), runtime["agentId"]}, 
                    object::pair{std::string("roomId"), roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("hello world")}, 
                        object::pair{std::string("type"), std::string("text")}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now()}
                };
                console->log(std::string("Sending hello world message to agent..."));
                std::async([=]() { runtime["processMessage"](helloMessage); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto messages = std::async([=]() { runtime["messageManager"]["getMessages"](object{
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("limit"), 10}
                }); });
                console->log(std::string("Retrieved ") + messages["length"] + std::string(" messages from conversation"));
                if (messages["length"] < 2) {
                    throw any(std::make_shared<Error>(std::string("Expected at least 2 messages, got ") + messages["length"] + string_empty));
                }
                auto agentResponse = messages["find"]([=](auto m) mutable
                {
                    return AND((AND((m["userId"] == runtime["agentId"]), (m["roomId"] == roomId))), (m["id"] != helloMessage["id"]));
                }
                );
                if (!agentResponse) {
                    throw any(std::make_shared<Error>(std::string("Agent did not respond to hello world message")));
                }
                console->log(std::string("Agent response:"), agentResponse["content"]["text"]);
                shared responseText = agentResponse["content"]["text"]["toLowerCase"]();
                auto greetingWords = array<string>{ std::string("hello"), std::string("hi"), std::string("hey"), std::string("greetings"), std::string("welcome") };
                auto containsGreeting = greetingWords->some([=](auto word) mutable
                {
                    return responseText["includes"](word);
                }
                );
                if (!containsGreeting) {
                    throw any(std::make_shared<Error>(std::string("Agent response did not contain a greeting. ") + std::string("Response was: "") + agentResponse["content"]["text"] + std::string(""")));
                }
                console->log(std::string("✓ Agent successfully responded to hello world"));
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Hello world test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Agent responds to casual greeting")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                auto greetings = array<string>{ std::string("hey there!"), std::string("hi, how are you?"), std::string("good morning!"), std::string("whats up?") };
                auto& __array3855_5290 = greetings;
                for (auto __indx3855_5290 = 0_N; __indx3855_5290 < __array3855_5290->get_length(); __indx3855_5290++)
                {
                    auto& greeting = const_(__array3855_5290)[__indx3855_5290];
                    {
                        auto roomId = std::string("test-room-greeting-") + Date->now() + std::string("-") + Math->random() + string_empty;
                        shared userId = std::string("test-user-greeting");
                        shared message = object{
                            object::pair{std::string("id"), std::string("msg-") + Date->now() + std::string("-") + Math->random() + string_empty}, 
                            object::pair{std::string("userId"), userId}, 
                            object::pair{std::string("agentId"), runtime["agentId"]}, 
                            object::pair{std::string("roomId"), roomId}, 
                            object::pair{std::string("content"), object{
                                object::pair{std::string("text"), greeting}, 
                                object::pair{std::string("type"), std::string("text")}
                            }}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        };
                        console->log(std::string("Testing greeting: "") + greeting + std::string("""));
                        std::async([=]() { runtime["processMessage"](message); });
                        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                        {
                            return setTimeout(resolve, 500);
                        }
                        ); });
                        auto messages = std::async([=]() { runtime["messageManager"]["getMessages"](object{
                            object::pair{std::string("roomId"), std::string("roomId")}, 
                            object::pair{std::string("limit"), 10}
                        }); });
                        auto agentResponse = messages["find"]([=](auto m) mutable
                        {
                            return AND((m["userId"] == runtime["agentId"]), (m["id"] != message["id"]));
                        }
                        );
                        if (!agentResponse) {
                            throw any(std::make_shared<Error>(std::string("Agent did not respond to greeting: "") + greeting + std::string(""")));
                        }
                        if (OR((!agentResponse["content"]["text"]), (agentResponse["content"]["text"]["length"] < 2))) {
                            throw any(std::make_shared<Error>(std::string("Agent gave empty response to: "") + greeting + std::string(""")));
                        }
                        console->log(std::string("✓ Agent responded to: "") + greeting + std::string("""));
                    }
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Casual greeting test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Agent maintains conversation context")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                auto roomId = std::string("test-room-context-") + Date->now() + string_empty;
                shared userId = std::string("test-user-context");
                auto firstMessage = object{
                    object::pair{std::string("id"), std::string("msg-1-") + Date->now() + string_empty}, 
                    object::pair{std::string("userId"), userId}, 
                    object::pair{std::string("agentId"), runtime["agentId"]}, 
                    object::pair{std::string("roomId"), roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("My favorite color is blue. What's yours?")}, 
                        object::pair{std::string("type"), std::string("text")}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now()}
                };
                console->log(std::string("Sending first message about favorite color..."));
                std::async([=]() { runtime["processMessage"](firstMessage); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto secondMessage = object{
                    object::pair{std::string("id"), std::string("msg-2-") + Date->now() + string_empty}, 
                    object::pair{std::string("userId"), userId}, 
                    object::pair{std::string("agentId"), runtime["agentId"]}, 
                    object::pair{std::string("roomId"), roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Why did you choose that color?")}, 
                        object::pair{std::string("type"), std::string("text")}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now() + 1000}
                };
                console->log(std::string("Sending follow-up question..."));
                std::async([=]() { runtime["processMessage"](secondMessage); });
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, 1000);
                }
                ); });
                auto messages = std::async([=]() { runtime["messageManager"]["getMessages"](object{
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("limit"), 10}
                }); });
                if (messages["length"] < 4) {
                    throw any(std::make_shared<Error>(std::string("Expected at least 4 messages, got ") + messages["length"] + string_empty));
                }
                auto agentResponses = messages["filter"]([=](auto m) mutable
                {
                    return m["userId"] == runtime["agentId"];
                }
                );
                if (agentResponses["length"] < 2) {
                    throw any(std::make_shared<Error>(std::string("Agent did not respond to both messages")));
                }
                auto secondResponse = const_(agentResponses)[agentResponses["length"] - 1];
                shared responseText = secondResponse["content"]["text"]["toLowerCase"]();
                auto contextWords = array<string>{ std::string("color"), std::string("blue"), std::string("favorite"), std::string("chose"), std::string("choice"), std::string("because") };
                auto hasContext = contextWords->some([=](auto word) mutable
                {
                    return responseText["includes"](word);
                }
                );
                if (!hasContext) {
                    console->warn(std::string("Agent response may not show context awareness. ") + std::string("Response: "") + secondResponse["content"]["text"] + std::string("""));
                }
                console->log(std::string("✓ Agent maintained conversation context"));
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Context test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    } };
};

#endif
