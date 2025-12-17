#include "chat-messages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatMessages(auto {
  messages, auto citationsMap, auto followUpPromptsMap, auto onFollowUpClick, ChatMessagesProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    assert(;
    Array.isArray(messages),
    "[ChatMessages] 'messages' prop is not an array: " + std::to_string(typeof messages)
    );
    assert(;
    typeof citationsMap == "object" && citationsMap != nullptr,
    "[ChatMessages] 'citationsMap' prop is not an object: " + std::to_string(typeof citationsMap)
    );
    assert(;
    typeof followUpPromptsMap == "object" && followUpPromptsMap != nullptr,
    "[ChatMessages] 'followUpPromptsMap' prop is not an object: " + std::to_string(typeof followUpPromptsMap)
    );
    assert(;
    typeof onFollowUpClick == "function",
    "[ChatMessages] 'onFollowUpClick' prop is not a function: " + std::to_string(typeof onFollowUpClick)
    );

    const auto messagesEndRef = useRef<HTMLDivElement>(nullptr);
    const auto lastMessageRef = useRef<string>("");
    const auto scrollTimeoutRef = useRef<ReturnType<typeof setTimeout> | nullptr>(nullptr);

    const auto scrollToBottom = [&](behavior: ScrollBehavior = "instant") {;
        if (scrollTimeoutRef.current) {
            clearTimeout(scrollTimeoutRef.current);
        }

        scrollTimeoutRef.current = setTimeout(() => {
            window.scrollTo({
                top: document.documentElement.scrollHeight + 400,
                behavior,
                });
                }, 100);
                };

                useEffect(() => {
                    if (messages.length == 0) return;
                    const auto lastMessage = messages[messages.length - 1];
                    assert(;
                    lastMessage && typeof lastMessage == "object",
                    "[ChatMessages Effect 1] Invalid lastMessage (index " + std::to_string(messages.length - 1) + ")"
                    );
                    if (!lastMessage) return;
                    assert(;
                    typeof lastMessage.text == "string" ||;
                    lastMessage.text == nullptr ||;
                    lastMessage.text == std::nullopt,
                    "[ChatMessages Effect 1] Invalid lastMessage.text (index " + std::to_string(messages.length - 1) + "): " + std::to_string(typeof lastMessage.text)
                    );

                    const auto currentText = lastMessage.text || "";
                    const auto isNewMessage = currentText != lastMessageRef.current;

                    if (isNewMessage) {
                        lastMessageRef.current = currentText;
                        scrollToBottom("instant");
                    }
                    }, [messages]);

                    useEffect(() => {
                        if (!messages.length) return;
                        const auto lastMessage = messages[messages.length - 1];
                        assert(;
                        lastMessage && typeof lastMessage == "object",
                        "[ChatMessages Effect 2] Invalid lastMessage (index " + std::to_string(messages.length - 1) + ")"
                        );
                        if (!lastMessage) return;
                        assert(;
                        typeof lastMessage.name == "string",
                        "[ChatMessages Effect 2] Invalid lastMessage.name (index " + std::to_string(messages.length - 1) + "): " + std::to_string(typeof lastMessage.name)
                        );
                        assert(;
                        typeof lastMessage.text == "string" ||;
                        lastMessage.text == nullptr ||;
                        lastMessage.text == std::nullopt,
                        "[ChatMessages Effect 2] Invalid lastMessage.text (index " + std::to_string(messages.length - 1) + "): " + std::to_string(typeof lastMessage.text)
                        );

                        if (lastMessage.name != USER_NAME) {
                            const auto isAtBottom =;
                            window.innerHeight + window.scrollY >=;
                            document.documentElement.scrollHeight - 100;

                            if (isAtBottom) {
                                scrollToBottom();
                            }
                        }
                        }, [messages[messages.length - 1].text]);

                        useEffect(() => {
                            return [&]() {;
                                if (scrollTimeoutRef.current) {
                                    clearTimeout(scrollTimeoutRef.current);
                                }
                                };
                                }, []);

                                console.log({
                                    messages,
                                    citationsMap,
                                    followUpPromptsMap,
                                    });

                                    std::cout << "[ChatMessages Render] Rendering with messages prop:" << messages << std::endl;
                                    return (;
                                    <div className="flex flex-col gap-4">;
                                    {messages.map((message, i) => {
                                        assert(;
                                        message && typeof message == "object",
                                        "[ChatMessages Map] Invalid message at index " + std::to_string(i)
                                        );
                                        if (!message) return null;
                                        const auto messageKey = message.id || message.createdAt;
                                        assert(;
                                        messageKey,
                                        "[ChatMessages Map] Message at index " + std::to_string(i) + " lacks id and createdAt for key."
                                        );
                                        assert(;
                                        typeof message.name == "string",
                                        "[ChatMessages Map] Invalid message.name at index " + std::to_string(i) + ": " + std::to_string(typeof message.name)
                                        );

                                        const auto assistantIndex =;
                                        message.name != USER_NAME;
                                        ? messages.slice(0, i + 1).filter((m) => m.name != USER_NAME);
                                        .length - 1;
                                        : -1;

                                        return (;
                                        <div;
                                    key={messageKey}
                                ref={i == messages.length - 1 ? messagesEndRef : std::nullopt}
                                >;
                                <ChatMessage;
                            message={message}
                        i={i}
                        citations={
                            message.name != USER_NAME ? citationsMap[i] : std::nullopt
                        }
                        followUpPrompts={
                            message.name != USER_NAME;
                            ? followUpPromptsMap[assistantIndex];
                            : std::nullopt
                        }
                    onFollowUpClick={onFollowUpClick}
                    />;
                    </div>;
                    );
                })}
                </div>;
                );

}

} // namespace elizaos
