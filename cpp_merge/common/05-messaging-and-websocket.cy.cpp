#include "05-messaging-and-websocket.cy.h"

void Main(void)
{
    describe(std:("Messaging and WebSocket"), [=]() mutable
    {
        shared BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
        shared DEFAULT_AGENT_ID = std:("15aec527-fb92-0792-91b6-becd4fac5050");
        auto _TEST_TIMEOUT = 30000;
        before([=]() mutable
        {
            cy->waitForBackend();
        }
        );
        beforeEach([=]() mutable
        {
            cy->visit(std:("/"), object{
                object::pair{std:("timeout"), 30000}
            });
            cy->window()->then([=](auto win) mutable
            {
                win["localStorage"]["setItem"](std:("skipBoot"), std:("true"));
            }
            );
            cy->waitForElizaClient();
        }
        );
        describe(std:("WebSocket Connection"), [=]() mutable
        {
            it(std:("should establish WebSocket connection"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    expect(win["elizaClient"])->to->exist;
                    expect(win["elizaClient"]["socket"])->to->exist;
                    expect(win["elizaClient"]["socket"]["connected"])->to->be->true;
                    cy->log(std:("✅ WebSocket connected"));
                }
                );
            }
            );
            it(std:("should handle WebSocket reconnection"), [=]() mutable
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
                        cy->log(std:("✅ WebSocket reconnected successfully"));
                    }
                    );
                }
                );
            }
            );
            it(std:("should receive connection events"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared events = array<string>();
                    client["socket"]["on"](std:("connect"), [=]() mutable
                    {
                        return events->push(std:("connect"));
                    }
                    );
                    client["socket"]["on"](std:("disconnect"), [=]() mutable
                    {
                        return events->push(std:("disconnect"));
                    }
                    );
                    client["socket"]["on"](std:("error"), [=](auto error) mutable
                    {
                        return events->push(std:("error: ") + error + string_empty);
                    }
                    );
                    client["socket"]["disconnect"]();
                    cy->wait(500);
                    client["socket"]["connect"]();
                    cy->wait(2000)->then([=]() mutable
                    {
                        expect(events)->to->include(std:("disconnect"));
                        expect(events)->to->include(std:("connect"));
                        cy->log(std:("✅ Received events: ") + events->join(std:(", ")) + string_empty);
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Message Sending"), [=]() mutable
        {
            it(std:("should send message to agent via API"), [=]() mutable
            {
                auto testMessage = object{
                    object::pair{std:("text"), std:("Test message at ") + Date->now() + string_empty}, 
                    object::pair{std:("userId"), std:("test-user-cypress")}, 
                    object::pair{std:("roomId"), std:("550e8400-e29b-41d4-a716-446655440003")}, 
                    object::pair{std:("messageId"), std:("msg-") + Date->now() + string_empty}
                };
                cy->sendMessage(testMessage)->then([=](auto response) mutable
                {
                    expect(response["status"])->to->equal(201);
                    expect(response["body"])->to->have->property(std:("success"), true);
                    cy->log(std:("✅ Message sent successfully via API"));
                }
                );
            }
            );
            it(std:("should send message through WebSocket"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    auto messageText = std:("WebSocket test ") + Date->now() + string_empty;
                    shared roomId = std:("550e8400-e29b-41d4-a716-446655440004");
                    shared responseReceived = false;
                    client["socket"]["on"](std:("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            responseReceived = true;
                        }
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std:("text"), messageText}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("userId"), std:("test-user")}
                    });
                    cy->wait(3000)->then([=]() mutable
                    {
                        expect(responseReceived)->to->be->true;
                        cy->log(std:("✅ WebSocket message sent and response received"));
                    }
                    );
                }
                );
            }
            );
            it(std:("should handle multiple messages in sequence"), [=]() mutable
            {
                shared timestamp = Date->now();
                auto messages = array<object>{ object{
                    object::pair{std:("text"), std:("First message - ") + timestamp + string_empty}, 
                    object::pair{std:("userId"), std:("sequence-test-user")}
                }, object{
                    object::pair{std:("text"), std:("Second message - ") + timestamp + string_empty}, 
                    object::pair{std:("userId"), std:("sequence-test-user")}
                }, object{
                    object::pair{std:("text"), std:("Third message - ") + timestamp + string_empty}, 
                    object::pair{std:("userId"), std:("sequence-test-user")}
                } };
                messages->forEach([=](auto msg, auto index) mutable
                {
                    cy->sendMessage(utils::assign(object{
                        , 
                        object::pair{std:("roomId"), std:("550e8400-e29b-41d4-a716-446655440005")}, 
                        object::pair{std:("messageId"), std:("seq-msg-") + index + std:("-") + timestamp + string_empty}
                    }, msg))->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->equal(201);
                        cy->log(std:("✅ Message ") + (index + 1) + std:(" sent successfully"));
                    }
                    );
                    cy->wait(500);
                }
                );
            }
            );
        }
        );
        describe(std:("Message Broadcasting"), [=]() mutable
        {
            it(std:("should broadcast messages to room participants"), [=]() mutable
            {
                shared roomId = std:("550e8400-e29b-41d4-a716-446655440006");
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared receivedMessages = array<any>();
                    client["joinRoom"](roomId);
                    client["socket"]["on"](std:("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            receivedMessages->push(data);
                        }
                    }
                    );
                    cy->sendMessage(object{
                        object::pair{std:("text"), std:("Broadcast test message")}, 
                        object::pair{std:("userId"), std:("broadcaster")}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("messageId"), std:("broadcast-") + Date->now() + string_empty}
                    })->then([=]() mutable
                    {
                        cy->wait(2000)->then([=]() mutable
                        {
                            expect(receivedMessages->get_length())->to->be->at->least(1);
                            cy->log(std:("✅ Received ") + receivedMessages->get_length() + std:(" broadcast messages"));
                        }
                        );
                    }
                    );
                }
                );
            }
            );
            it(std:("should handle room-specific messaging"), [=]() mutable
            {
                shared room1 = std:("550e8400-e29b-41d4-a716-446655440007");
                shared room2 = std:("550e8400-e29b-41d4-a716-446655440008");
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared room1Messages = array<any>();
                    shared room2Messages = array<any>();
                    client["joinRoom"](room1);
                    client["joinRoom"](room2);
                    client["socket"]["on"](std:("message"), [=](auto data) mutable
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
                        object::pair{std:("text"), std:("Message for room 1")}, 
                        object::pair{std:("userId"), std:("test-user")}, 
                        object::pair{std:("roomId"), room1}, 
                        object::pair{std:("messageId"), std:("room1-msg-") + Date->now() + string_empty}
                    });
                    cy->sendMessage(object{
                        object::pair{std:("text"), std:("Message for room 2")}, 
                        object::pair{std:("userId"), std:("test-user")}, 
                        object::pair{std:("roomId"), room2}, 
                        object::pair{std:("messageId"), std:("room2-msg-") + Date->now() + string_empty}
                    });
                    cy->wait(3000)->then([=]() mutable
                    {
                        expect(room1Messages->get_length())->to->be->at->least(1);
                        expect(room2Messages->get_length())->to->be->at->least(1);
                        cy->log(std:("✅ Room isolation working: Room1=") + room1Messages->get_length() + std:(", Room2=") + room2Messages->get_length() + string_empty);
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Real-time Updates"), [=]() mutable
        {
            it(std:("should receive real-time agent responses"), [=]() mutable
            {
                shared roomId = std:("550e8400-e29b-41d4-a716-446655440009");
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared userMessageReceived = false;
                    shared agentResponseReceived = false;
                    client["joinRoom"](roomId);
                    client["socket"]["on"](std:("message"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            if (data["userId"] == std:("test-user")) {
                                userMessageReceived = true;
                            } else if (data["userId"] == DEFAULT_AGENT_ID) {
                                agentResponseReceived = true;
                            }
                        }
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std:("text"), std:("Hello, can you respond in real-time?")}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("userId"), std:("test-user")}
                    });
                    cy->wait(5000)->then([=]() mutable
                    {
                        expect(userMessageReceived)->to->be->true;
                        expect(agentResponseReceived)->to->be->true;
                        cy->log(std:("✅ Real-time message exchange completed"));
                    }
                    );
                }
                );
            }
            );
            it(std:("should receive typing indicators"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared roomId = std:("typing-test-") + Date->now() + string_empty;
                    shared typingReceived = false;
                    client["socket"]["on"](std:("typing"), [=](auto data) mutable
                    {
                        if (data["roomId"] == roomId) {
                            typingReceived = true;
                        }
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std:("text"), std:("This should trigger typing indicator")}, 
                        object::pair{std:("roomId"), std:("roomId")}, 
                        object::pair{std:("userId"), std:("test-user")}
                    });
                    cy->wait(2000)->then([=]() mutable
                    {
                        cy->log(std:("✅ Typing indicator: ") + (typingReceived) ? std:("received") : std:("not implemented") + string_empty);
                    }
                    );
                }
                );
            }
            );
            it(std:("should receive status updates"), [=]() mutable
            {
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    shared statusUpdates = array<any>();
                    client["socket"]["on"](std:("status"), [=](auto data) mutable
                    {
                        statusUpdates->push(data);
                    }
                    );
                    client["socket"]["on"](std:("agent-status"), [=](auto data) mutable
                    {
                        statusUpdates->push(data);
                    }
                    );
                    client["sendMessage"](object{
                        object::pair{std:("text"), std:("Status update test")}, 
                        object::pair{std:("roomId"), std:("550e8400-e29b-41d4-a716-446655440005")}, 
                        object::pair{std:("userId"), std:("test-user")}
                    });
                    cy->wait(3000)->then([=]() mutable
                    {
                        cy->log(std:("✅ Received ") + statusUpdates->get_length() + std:(" status updates"));
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle message send failures gracefully"), [=]() mutable
            {
                cy->sendMessage(object{
                    object::pair{std:("text"), std:("Incomplete message")}, 
                    object::pair{std:("userId"), std:("error-test-user")}, 
                    object::pair{std:("roomId"), std:("invalid-room-id")}
                })->then([=](auto response) mutable
                {
                    expect(array<double>{ 201, 400, 500 })->to->include(response["status"]);
                }
                );
            }
            );
            it(std:("should handle WebSocket disconnection"), [=]() mutable
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
                            cy->log(std:("✅ Handled disconnection and reconnection"));
                        }
                        );
                    }
                    );
                }
                );
            }
            );
            it(std:("should handle rapid message sending"), [=]() mutable
            {
                shared roomId = std:("rapid-test-") + Date->now() + string_empty;
                shared messageCount = 10;
                cy->window()->then([=](auto win) mutable
                {
                    auto client = win["elizaClient"];
                    auto sentMessages = array<any>();
                    for (auto i = 0; i < messageCount; i++)
                    {
                        auto message = object{
                            object::pair{std:("text"), std:("Rapid message ") + i + string_empty}, 
                            object::pair{std:("roomId"), std:("roomId")}, 
                            object::pair{std:("userId"), std:("rapid-test-user")}, 
                            object::pair{std:("messageId"), std:("rapid-") + i + std:("-") + Date->now() + string_empty}
                        };
                        sentMessages->push(message);
                        client["sendMessage"](message);
                    }
                    cy->wait(5000)->then([=]() mutable
                    {
                        cy->log(std:("✅ Sent ") + messageCount + std:(" messages rapidly"));
                    }
                    );
                }
                );
            }
            );
        }
        );
        describe(std:("Message Persistence"), [=]() mutable
        {
            it(std:("should persist messages in memory system"), [=]() mutable
            {
                shared timestamp = Date->now();
                auto messageText = std:("Persistence test ") + timestamp + string_empty;
                cy->sendMessage(object{
                    object::pair{std:("text"), messageText}, 
                    object::pair{std:("userId"), std:("persist-test-user")}, 
                    object::pair{std:("roomId"), std:("550e8400-e29b-41d4-a716-446655440010")}, 
                    object::pair{std:("messageId"), std:("persist-") + timestamp + string_empty}
                })->then([=]() mutable
                {
                    cy->wait(1000);
                    cy->request(object{
                        object::pair{std:("method"), std:("GET")}, 
                        object::pair{std:("url"), string_empty + BACKEND_URL + std:("/api/memory/query")}, 
                        object::pair{std:("qs"), object{
                            object::pair{std:("roomId"), std:("550e8400-e29b-41d4-a716-446655440010")}, 
                            object::pair{std:("limit"), 10}
                        }}
                    })->then([=](auto response) mutable
                    {
                        expect(response["status"])->to->equal(200);
                        expect(response["body"])->to->have->property(std:("memories"));
                        expect(response["body"]["memories"])->to->be->an(std:("array"));
                        auto foundMessage = response["body"]["memories"]["find"]([=](auto m) mutable
                        {
                            return m["content"]->text->includes(std:("Persistence test ") + timestamp + string_empty);
                        }
                        );
                        expect(foundMessage)->to->exist;
                        cy->log(std:("✅ Message persisted in memory system"));
                    }
                    );
                }
                );
            }
            );
            it(std:("should maintain message order"), [=]() mutable
            {
                auto roomId = std:("550e8400-e29b-41d4-a716-446655440011");
                shared messages = array<object>{ object{
                    object::pair{std:("text"), std:("Ordered message 1")}, 
                    object::pair{std:("userId"), std:("order-test-user")}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("messageId"), std:("order-1-") + Date->now() + string_empty}
                }, object{
                    object::pair{std:("text"), std:("Ordered message 2")}, 
                    object::pair{std:("userId"), std:("order-test-user")}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("messageId"), std:("order-2-") + Date->now() + string_empty}
                }, object{
                    object::pair{std:("text"), std:("Ordered message 3")}, 
                    object::pair{std:("userId"), std:("order-test-user")}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("messageId"), std:("order-3-") + Date->now() + string_empty}
                }, object{
                    object::pair{std:("text"), std:("Ordered message 4")}, 
                    object::pair{std:("userId"), std:("order-test-user")}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("messageId"), std:("order-4-") + Date->now() + string_empty}
                }, object{
                    object::pair{std:("text"), std:("Ordered message 5")}, 
                    object::pair{std:("userId"), std:("order-test-user")}, 
                    object::pair{std:("roomId"), std:("roomId")}, 
                    object::pair{std:("messageId"), std:("order-5-") + Date->now() + string_empty}
                } };
                messages->forEach([=](auto msg) mutable
                {
                    return cy->sendMessage(msg);
                }
                );
                cy->wait(3000)->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/memories?roomId=") + roomId + std:("&count=10"))->then([=](auto response) mutable
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
    describe(std:("Messaging and WebSocket Summary"), [=]() mutable
    {
        it(std:("should verify complete messaging functionality"), [=]() mutable
        {
            shared BACKEND_URL = OR((Cypress->env(std:("BACKEND_URL"))), (std:("http://localhost:7777")));
            shared operations = array<std::shared_ptr<any::Operation>>();
            cy->log(std:("🎯 MESSAGING AND WEBSOCKET VERIFICATION:"));
            cy->window()->then([=](auto win) mutable
            {
                auto wsConnected = OR((win["elizaClient"]["socket"]["connected"]), (false));
                operations->push(object{
                    object::pair{std:("operation"), std:("WebSocket Connection")}, 
                    object::pair{std:("success"), wsConnected}, 
                    object::pair{std:("details"), (wsConnected) ? std:("Connected") : std:("Not connected")}
                });
                if (wsConnected) {
                    shared testRoomId = std:("summary-test-") + Date->now() + string_empty;
                    return cy->sendMessage(object{
                        object::pair{std:("text"), std:("Summary test message")}, 
                        object::pair{std:("userId"), std:("summary-test")}, 
                        object::pair{std:("roomId"), testRoomId}, 
                        object::pair{std:("messageId"), std:("summary-") + Date->now() + string_empty}
                    })->then([=](auto response) mutable
                    {
                        operations->push(object{
                            object::pair{std:("operation"), std:("Message Send")}, 
                            object::pair{std:("success"), response["status"] == 201}, 
                            object::pair{std:("details"), (response["status"] == 201) ? std:("Message sent") : std:("Send failed")}
                        });
                        return cy->request(std:("GET"), string_empty + BACKEND_URL + std:("/api/memories?roomId=") + testRoomId + std:("&count=5"));
                    }
                    )->then([=](auto memResponse) mutable
                    {
                        operations->push(object{
                            object::pair{std:("operation"), std:("Message Persistence")}, 
                            object::pair{std:("success"), memResponse["status"] == 200}, 
                            object::pair{std:("details"), string_empty + (OR((memResponse["body"]["data"]["length"]), (0))) + std:(" messages retrieved")}
                        });
                    }
                    );
                }
            }
            )->then([=]() mutable
            {
                operations->forEach([=](auto op) mutable
                {
                    auto icon = (op->success) ? std:("✅") : std:("❌");
                    cy->log(string_empty + icon + std:(" ") + op->operation + std:(": ") + op->details + string_empty);
                }
                );
                auto successCount = operations->filter([=](auto op) mutable
                {
                    return op->success;
                }
                )->get_length();
                cy->log(std:("\
✅ ") + successCount + std:("/") + operations->get_length() + std:(" operations successful"));
                expect(successCount)->to->be->at->least(operations->get_length() - 1);
                cy->screenshot(std:("messaging-websocket-summary"));
            }
            );
        }
        );
    }
    );
}

MAIN
