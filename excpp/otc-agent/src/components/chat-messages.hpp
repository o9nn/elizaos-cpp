#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

"use client";

;

;
;
;
;

struct ChatMessagesProps {
    std::vector<ChatMessageType> messages;
    std::unordered_map<double, std::vector<Citation>> citationsMap;
    std::unordered_map<double, std::vector<std::string>> followUpPromptsMap;
    (prompt: string) => void onFollowUpClick;
    std::optional<std::string> assistantAvatarUrl;
    std::optional<std::string> assistantName;
};


: ChatMessagesProps) {
  const messagesEndRef = useRef<HTMLDivElement>(null);
  const lastMessageRef = useRef<string>("");

  const scrollToBottom = (behavior: ScrollBehavior = "smooth") => {
    setTimeout(() => {
      messagesEndRef.current?.scrollIntoView({ behavior });
    }, 100);
  };

  useEffect(() => {
    if (messages.length === 0) return;
    const lastMessage = messages[messages.length - 1];

    const currentText = lastMessage.text ?? "";
    const isNewMessage = currentText !== lastMessageRef.current;

    if (isNewMessage) {
      lastMessageRef.current = currentText;
      scrollToBottom("instant");
    }
  }, [messages]);

  const lastMessageName = messages[messages.length - 1]?.name;
  const lastMessageText = messages[messages.length - 1]?.text;

  useEffect(() => {
    if (!messages.length) return;
    const lastMessage = messages[messages.length - 1];

    const isUserMessage = lastMessage.name === USER_NAME;
    const hasContent = lastMessage.text && lastMessage.text.trim() !== "";

    if (isUserMessage && hasContent) {
      scrollToBottom("smooth");
    }
  }, [lastMessageName, lastMessageText, messages]);

  return (
    <div className="flex flex-col">
      {messages.map((message, i) => {
        // Use a combination of message id/timestamp and index to ensure uniqueness
        const messageKey = `${message.id || message.createdAt || i}_${i}`;

        const assistantIndex =
          message.name !== USER_NAME
            ? messages.slice(0, i + 1).filter((m) => m.name !== USER_NAME)
                .length - 1
            : -1;

        return (
          <div
            key={messageKey}
            ref={i === messages.length - 1 ? messagesEndRef : undefined}
          >
            <ChatMessage
              message={message}
              i={i}
              citations={
                message.name !== USER_NAME ? citationsMap[i] : undefined
              }
              followUpPrompts={
                message.name !== USER_NAME
                  ? followUpPromptsMap[assistantIndex]
                  : undefined
              }
              onFollowUpClick={onFollowUpClick}
              assistantAvatarUrl={assistantAvatarUrl}
              assistantName={assistantName}
            />
          </div>
        );
      })}
    </div>
  );
}

} // namespace elizaos
