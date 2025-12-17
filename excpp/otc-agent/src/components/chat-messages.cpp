#include "chat-messages.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatMessages(auto {
  messages, auto citationsMap, auto followUpPromptsMap, auto onFollowUpClick, auto assistantAvatarUrl, auto assistantName, ChatMessagesProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto messagesEndRef = useRef<HTMLDivElement>(nullptr);
    const auto lastMessageRef = useRef<string>("");

    const auto scrollToBottom = [&](behavior: ScrollBehavior = "smooth") {;
        setTimeout(() => {
            messagesEndRef.current.scrollIntoView({ behavior });
            }, 100);
            };

            useEffect(() => {
                if (messages.length == 0) return;
                const auto lastMessage = messages[messages.length - 1];

                const auto currentText = lastMessage.text || "";
                const auto isNewMessage = currentText != lastMessageRef.current;

                if (isNewMessage) {
                    lastMessageRef.current = currentText;
                    scrollToBottom("instant");
                }
                }, [messages]);

                const auto lastMessageName = messages[messages.length - 1].name;
                const auto lastMessageText = messages[messages.length - 1].text;

                useEffect(() => {
                    if (!messages.length) return;
                    const auto lastMessage = messages[messages.length - 1];

                    const auto isUserMessage = lastMessage.name == USER_NAME;
                    const auto hasContent = lastMessage.text && lastMessage.text.trim() != "";

                    if (isUserMessage && hasContent) {
                        scrollToBottom("smooth");
                    }
                    }, [lastMessageName, lastMessageText, messages]);

                    return (;
                    <div className="flex flex-col">;
                    {messages.map((message, i) => {
                        // Use a combination of message id/timestamp and index to ensure uniqueness
                        const auto messageKey = std::to_string(message.id || message.createdAt || i) + "_" + std::to_string(i);

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
    assistantAvatarUrl={assistantAvatarUrl}
    assistantName={assistantName}
    />;
    </div>;
    );
    })}
    </div>;
    );

}

} // namespace elizaos
