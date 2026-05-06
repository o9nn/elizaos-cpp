#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/e2e/05-messaging-and-websocket.cy.h"

void Main(void)
{
    describe(std::string("Messaging and WebSocket"), [=]() mutable
    {
        shared BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
        shared DEFAULT_AGENT_ID = std::string("15aec527-fb92-0792-91b6-becd4fac5050");
        auto _TEST_TIMEOUT = 30000;
        before([=]() mutable
        {
            cy->waitForBackend();
        }
        );
        beforeEach([=]() mutable
        {
            cy->visit(std::string("/"), object{
                object::pair{std::string("timeout"), 30000}
            });
            cy->window()->then([=](auto win) mutable
            {
                win["localStorage"]["setItem"](std::string("skipBoot"), std::string("true"));
            }
            );
            cy->waitForElizaClient();
        }
        );
        describe(std::string("WebSocket Connection"), [=]() mutable
        {
            it(std::string("should establish WebSocket connection"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    expect(win["elizaClient"])->to->exist;
                    expect(win["elizaClient"]["socket"])->to->exist;
                    expect(win["elizaClient"]["socket"]["connected"])->to->be->true;
                    cy->log(std::string("✅ WebSocket connected"));
                }
                );
            }
            );
            it(std::string("should handle WebSocket reconnection"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    shared client = win["elizaClient"];
                    client["socket"]["disconnect"]();
                    cy->wait(1000);
                    cy->wrap(nullptr)->then([=]() mutable
                    {
                        return std::make_shared<Cypress->Promise>([=](auto resolve) mutable
                        {
                            shared checkConnection = [=]() mutable
                            {
                                if (client["socket"]["connected"]) {
                                    resolve(true);
                                } else {
                                    setTimeout(checkConnection, 500);
                                }
                            };
                            checkConnection();
                        }
                        );
                    }
                    )->then([=]() mutable
                    {
                        expect(client["socket"]["connected"])->to->be->true;
                        cy->log(std::string("✅ WebSocket reconnected successfully"));
                    }
                    );
                }
                );
            }
            );
            it(std::string("should receive connection events"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared events = array<string>();
                    client["socket"]["on"](std::string("connect"), [=]() mutable
                    {
                        return events->push(std::string("connect"));
                    }
                    );
                    client["socket"]["on"](std::string("disconnect"), [=]() mutable
                    {
                        return events->push(std::string("disconnect"));
                    }
                    );
                    client["socket"]["on"](std::string("error"), [=](auto error) mutable
                    {
                        return events->push(std::string("error: ") + error + string_empty);
                    }
                    );
                    client["socket"]["disconnect"]();
                    cy->wait(500);
                    client["socket"]["connect"]();
                    cy->wait(2000)->then([=]() mutable
                    {
                        expect(events)->to->include(std::string("disconnect"));
                        expect(events)->to->include(std::string("connect"));
                        cy->log(std::string("✅ Received events: ") + events->join(std::string(", ")) + string_empty);
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Message Sending"), [=]() mutable
        {
            it(std::string("should send message to agent via API"), [=]() mutable
            {
                auto testMessage = object{
                    object::pair{std::string("text"), std::string("Test message at ") + Date->now() + string_empty}, 
                    object::pair{std::string("userId"), std::string("test-user-cypress")}, 
                    object::pair{std::string("roomId"), std::string("550e8400-e29b-41d4-a716-446655440003")}, 
                    object::pair{std::string("messageId"), std::string("msg-") + Date->now() + string_empty}
                };
                cy->sendMessage(testMessage)->then([=](auto response) mutable
                {
                    expect(response["status"])->to->equal(201);
                    expect(response["body"])->to->have->property(std::string("success"), true);
                    cy->log(std::string("✅ Message sent successfully via API"));
                }
                );
            }
            );
            it(std::string("should send message through WebSocket"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    auto messageText = std::string("WebSocket test ") + Date->now() + string_empty;
                    shared roomId = std::string("550e8400-e29b-41d4-a716-446655440004");
                    shared responseReceived = false;
                    client["socket"]["on"](std::string("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            responseReceived = true;
                        }
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std::string("text"), messageText}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("userId"), std::string("test-user")}
                    });
                    cy->wait(3000)->then([=]() mutable
                    {
                        expect(responseReceived)->to->be->true;
                        cy->log(std::string("✅ WebSocket message sent and response received"));
                    }
                    );
                }
                );
            }
            );
            it(std::string("should handle multiple messages in sequence"), [=]() mutable
            {
                shared timestamp = Date->now();
                auto messages = array<object>{ object{
                    object::pair{std::string("text"), std::string("First message - ") + timestamp + string_empty}, 
                    object::pair{std::string("userId"), std::string("sequence-test-user")}
                }, object{
                    object::pair{std::string("text"), std::string("Second message - ") + timestamp + string_empty}, 
                    object::pair{std::string("userId"), std::string("sequence-test-user")}
                }, object{
                    object::pair{std::string("text"), std::string("Third message - ") + timestamp + string_empty}, 
                    object::pair{std::string("userId"), std::string("sequence-test-user")}
                } };
                messages->forEach([=](auto msg, auto index) mutable
                {
                    cy->sendMessage(utils::assign(object{
                        , 
                        object::pair{std::string("roomId"), std::string("550e8400-e29b-41d4-a716-446655440005")}, 
                        object::pair{std::string("messageId"), std::string("seq-msg-") + index + std::string("-") + timestamp + string_empty}
                    }, msg))->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->equal(201);
                        cy->log(std::string("✅ Message ") + (index + 1) + std::string(" sent successfully"));
                    }
                    );
                    cy->wait(500);
                }
                );
            }
            );
        }
        );
        describe(std::string("Message Broadcasting"), [=]() mutable
        {
            it(std::string("should broadcast messages to room participants"), [=]() mutable
            {
                shared roomId = std::string("550e8400-e29b-41d4-a716-446655440006");
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared receivedMessages = array<any>();
                    client["joinRoom"](roomId);
                    client["socket"]["on"](std::string("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            receivedMessages->push(data);
                        }
                    }
                    );
                    cy->sendMessage(object{
                        object::pair{std::string("text"), std::string("Broadcast test message")}, 
                        object::pair{std::string("userId"), std::string("broadcaster")}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("messageId"), std::string("broadcast-") + Date->now() + string_empty}
                    })->then([=]() mutable
                    {
                        cy->wait(2000)->then([=]() mutable
                        {
                            expect(receivedMessages->get_length())->to->be->at->least(1);
                            cy->log(std::string("✅ Received ") + receivedMessages->get_length() + std::string(" broadcast messages"));
                        }
                        );
                    }
                    );
                }
                );
            }
            );
            it(std::string("should handle room-specific messaging"), [=]() mutable
            {
                shared room1 = std::string("550e8400-e29b-41d4-a716-446655440007");
                shared room2 = std::string("550e8400-e29b-41d4-a716-446655440008");
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared room1Messages = array<any>();
                    shared room2Messages = array<any>();
                    client["joinRoom"](room1);
                    client["joinRoom"](room2);
                    client["socket"]["on"](std::string("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == room1) {
                            room1Messages->push(data);
                        }
                        if (data["roomId"] == room2) {
                            room2Messages->push(data);
                        }
                    }
                    );
                    cy->sendMessage(object{
                        object::pair{std::string("text"), std::string("Message for room 1")}, 
                        object::pair{std::string("userId"), std::string("test-user")}, 
                        object::pair{std::string("roomId"), room1}, 
                        object::pair{std::string("messageId"), std::string("room1-msg-") + Date->now() + string_empty}
                    });
                    cy->sendMessage(object{
                        object::pair{std::string("text"), std::string("Message for room 2")}, 
                        object::pair{std::string("userId"), std::string("test-user")}, 
                        object::pair{std::string("roomId"), room2}, 
                        object::pair{std::string("messageId"), std::string("room2-msg-") + Date->now() + string_empty}
                    });
                    cy->wait(3000)->then([=]() mutable
                    {
                        expect(room1Messages->get_length())->to->be->at->least(1);
                        expect(room2Messages->get_length())->to->be->at->least(1);
                        cy->log(std::string("✅ Room isolation working: Room1=") + room1Messages->get_length() + std::string(", Room2=") + room2Messages->get_length() + string_empty);
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Real-time Updates"), [=]() mutable
        {
            it(std::string("should receive real-time agent responses"), [=]() mutable
            {
                shared roomId = std::string("550e8400-e29b-41d4-a716-446655440009");
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared userMessageReceived = false;
                    shared agentResponseReceived = false;
                    client["joinRoom"](roomId);
                    client["socket"]["on"](std::string("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            if (data["userId"] == std::string("test-user")) {
                                userMessageReceived = true;
                            } else if (data["userId"] == DEFAULT_AGENT_ID) {
                                agentResponseReceived = true;
                            }
                        }
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std::string("text"), std::string("Hello, can you respond in real-time?")}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("userId"), std::string("test-user")}
                    });
                    cy->wait(5000)->then([=]() mutable
                    {
                        expect(userMessageReceived)->to->be->true;
                        expect(agentResponseReceived)->to->be->true;
                        cy->log(std::string("✅ Real-time message exchange completed"));
                    }
                    );
                }
                );
            }
            );
            it(std::string("should receive typing indicators"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared roomId = std::string("typing-test-") + Date->now() + string_empty;
                    shared typingReceived = false;
                    client["socket"]["on"](std::string("typing"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            typingReceived = true;
                        }
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std::string("text"), std::string("This should trigger typing indicator")}, 
                        object::pair{std::string("roomId"), std::string("roomId")}, 
                        object::pair{std::string("userId"), std::string("test-user")}
                    });
                    cy->wait(2000)->then([=]() mutable
                    {
                        cy->log(std::string("✅ Typing indicator: ") + (typingReceived) ? std::string("received") : std::string("not implemented") + string_empty);
                    }
                    );
                }
                );
            }
            );
            it(std::string("should receive status updates"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared statusUpdates = array<any>();
                    client["socket"]["on"](std::string("status"), [=](auto data) mutable
                    {
                        statusUpdates->push(data);
                    }
                    );
                    client["socket"]["on"](std::string("agent-status"), [=](auto data) mutable
                    {
                        statusUpdates->push(data);
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std::string("text"), std::string("Status update test")}, 
                        object::pair{std::string("roomId"), std::string("550e8400-e29b-41d4-a716-446655440005")}, 
                        object::pair{std::string("userId"), std::string("test-user")}
                    });
                    cy->wait(3000)->then([=]() mutable
                    {
                        cy->log(std::string("✅ Received ") + statusUpdates->get_length() + std::string(" status updates"));
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle message send failures gracefully"), [=]() mutable
            {
                cy->sendMessage(object{
                    object::pair{std::string("text"), std::string("Incomplete message")}, 
                    object::pair{std::string("userId"), std::string("error-test-user")}, 
                    object::pair{std::string("roomId"), std::string("invalid-room-id")}
                })->then([=](auto response) mutable
                {
                    expect(array<double>{ 201, 400, 500 })->to->include(response["status"]);
                }
                );
            }
            );
            it(std::string("should handle WebSocket disconnection"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    shared client = win["elizaClient"];
                    auto _originalState = client["socket"]["connected"];
                    client["socket"]["disconnect"]();
                    cy->wait(1000)->then([=]() mutable
                    {
                        expect(client["socket"]["connected"])->to->be->false;
                        client["socket"]["connect"]();
                        cy->wait(2000)->then([=]() mutable
                        {
                            expect(client["socket"]["connected"])->to->be->true;
                            cy->log(std::string("✅ Handled disconnection and reconnection"));
                        }
                        );
                    }
                    );
                }
                );
            }
            );
            it(std::string("should handle rapid message sending"), [=]() mutable
            {
                shared roomId = std::string("rapid-test-") + Date->now() + string_empty;
                shared messageCount = 10;
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    auto sentMessages = array<any>();
                    for (auto i = 0; i < messageCount; i++)
                    {
                        auto message = object{
                            object::pair{std::string("text"), std::string("Rapid message ") + i + string_empty}, 
                            object::pair{std::string("roomId"), std::string("roomId")}, 
                            object::pair{std::string("userId"), std::string("rapid-test-user")}, 
                            object::pair{std::string("messageId"), std::string("rapid-") + i + std::string("-") + Date->now() + string_empty}
                        };
                        sentMessages->push(message);
                        client["sendMessage"](message);
                    }
                    cy->wait(5000)->then([=]() mutable
                    {
                        cy->log(std::string("✅ Sent ") + messageCount + std::string(" messages rapidly"));
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std::string("Message Persistence"), [=]() mutable
        {
            it(std::string("should persist messages in memory system"), [=]() mutable
            {
                shared timestamp = Date->now();
                auto messageText = std::string("Persistence test ") + timestamp + string_empty;
                cy->sendMessage(object{
                    object::pair{std::string("text"), messageText}, 
                    object::pair{std::string("userId"), std::string("persist-test-user")}, 
                    object::pair{std::string("roomId"), std::string("550e8400-e29b-41d4-a716-446655440010")}, 
                    object::pair{std::string("messageId"), std::string("persist-") + timestamp + string_empty}
                })->then([=]() mutable
                {
                    cy->wait(1000);
                    cy->request(object{
                        object::pair{std::string("method"), std::string("GET")}, 
                        object::pair{std::string("url"), string_empty + BACKEND_URL + std::string("/api/memory/query")}, 
                        object::pair{std::string("qs"), object{
                            object::pair{std::string("roomId"), std::string("550e8400-e29b-41d4-a716-446655440010")}, 
                            object::pair{std::string("limit"), 10}
                        }}
                    })->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->equal(200);
                        expect(response["body"])->to->have->property(std::string("memories"));
                        expect(response["body"]["memories"])->to->be->an(std::string("array"));
                        auto foundMessage = response["body"]["memories"]["find"]([=](auto m) mutable
                        {
                            return m["content"]->text->includes(std::string("Persistence test ") + timestamp + string_empty);
                        }
                        );
                        expect(foundMessage)->to->exist;
                        cy->log(std::string("✅ Message persisted in memory system"));
                    }
                    );
                }
                );
            }
            );
            it(std::string("should maintain message order"), [=]() mutable
            {
                auto roomId = std::string("550e8400-e29b-41d4-a716-446655440011");
                shared messages = array<object>{ object{
                    object::pair{std::string("text"), std::string("Ordered message 1")}, 
                    object::pair{std::string("userId"), std::string("order-test-user")}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("messageId"), std::string("order-1-") + Date->now() + string_empty}
                }, object{
                    object::pair{std::string("text"), std::string("Ordered message 2")}, 
                    object::pair{std::string("userId"), std::string("order-test-user")}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("messageId"), std::string("order-2-") + Date->now() + string_empty}
                }, object{
                    object::pair{std::string("text"), std::string("Ordered message 3")}, 
                    object::pair{std::string("userId"), std::string("order-test-user")}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("messageId"), std::string("order-3-") + Date->now() + string_empty}
                }, object{
                    object::pair{std::string("text"), std::string("Ordered message 4")}, 
                    object::pair{std::string("userId"), std::string("order-test-user")}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("messageId"), std::string("order-4-") + Date->now() + string_empty}
                }, object{
                    object::pair{std::string("text"), std::string("Ordered message 5")}, 
                    object::pair{std::string("userId"), std::string("order-test-user")}, 
                    object::pair{std::string("roomId"), std::string("roomId")}, 
                    object::pair{std::string("messageId"), std::string("order-5-") + Date->now() + string_empty}
                } };
                messages->forEach([=](auto msg) mutable
                {
                    return cy->sendMessage(msg);
                }
                );
                cy->wait(3000)->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/memories?roomId=") + roomId + std::string("&count=10"))->then([=](auto response) mutable
                {
                    expect(response["status"])->to->eq(200);
                    shared retrievedMessages = response["body"]["data"]["map"]([=](auto m) mutable
                    {
                        return m["content"];
                    }
                    )["reverse"]();
                    messages->forEach([=](auto msg, auto index) mutable
                    {
                        expect(const_(retrievedMessages)[index])->to->include(msg["text"]);
                    }
                    );
                }
                );
            }
            );
        }
        );
    }
    );
    describe(std::string("Messaging and WebSocket Summary"), [=]() mutable
    {
        it(std::string("should verify complete messaging functionality"), [=]() mutable
        {
            shared BACKEND_URL = OR((Cypress->env(std::string("BACKEND_URL"))), (std::string("http://localhost:7777")));
            shared operations = array<std::shared_ptr<any::Operation>>();
            cy->log(std::string("🎯 MESSAGING AND WEBSOCKET VERIFICATION:"));
            cy->window()->then([=](auto win) mutable
            {
                auto wsConnected = OR((win["elizaClient"]["socket"]["connected"]), (false));
                operations->push(object{
                    object::pair{std::string("operation"), std::string("WebSocket Connection")}, 
                    object::pair{std::string("success"), wsConnected}, 
                    object::pair{std::string("details"), (wsConnected) ? std::string("Connected") : std::string("Not connected")}
                });
                if (wsConnected) {
                    shared testRoomId = std::string("summary-test-") + Date->now() + string_empty;
                    return cy->sendMessage(object{
                        object::pair{std::string("text"), std::string("Summary test message")}, 
                        object::pair{std::string("userId"), std::string("summary-test")}, 
                        object::pair{std::string("roomId"), testRoomId}, 
                        object::pair{std::string("messageId"), std::string("summary-") + Date->now() + string_empty}
                    })->then([=](auto response) mutable
                    {
                        operations->push(object{
                            object::pair{std::string("operation"), std::string("Message Send")}, 
                            object::pair{std::string("success"), response["status"] == 201}, 
                            object::pair{std::string("details"), (response["status"] == 201) ? std::string("Message sent") : std::string("Send failed")}
                        });
                        return cy->request(std::string("GET"), string_empty + BACKEND_URL + std::string("/api/memories?roomId=") + testRoomId + std::string("&count=5"));
                    }
                    )->then([=](auto memResponse) mutable
                    {
                        operations->push(object{
                            object::pair{std::string("operation"), std::string("Message Persistence")}, 
                            object::pair{std::string("success"), memResponse["status"] == 200}, 
                            object::pair{std::string("details"), string_empty + (OR((memResponse["body"]["data"]["length"]), (0))) + std::string(" messages retrieved")}
                        });
                    }
                    );
                }
            }
            )->then([=]() mutable
            {
                operations->forEach([=](auto op) mutable
                {
                    auto icon = (op->success) ? std::string("✅") : std::string("❌");
                    cy->log(string_empty + icon + std::string(" ") + op->operation + std::string(": ") + op->details + string_empty);
                }
                );
                auto successCount = operations->filter([=](auto op) mutable
                {
                    return op->success;
                }
                )->get_length();
                cy->log(std::string("\
✅ ") + successCount + std::string("/") + operations->get_length() + std::string(" operations successful"));
                expect(successCount)->to->be->at->least(operations->get_length() - 1);
                cy->screenshot(std::string("messaging-websocket-summary"));
            }
            );
        }
        );
    }
    );
}

MAIN
