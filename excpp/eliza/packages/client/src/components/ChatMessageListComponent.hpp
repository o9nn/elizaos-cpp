#include "chat.hpp"
#include "elizaos/core.hpp"
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

;
;
;
;
;
;
import type { UiMessage } from '@/hooks/use-query-hooks';
;
;

struct ChatMessageListComponentProps {
    std::vector<UiMessage> messages;
    bool isLoadingMessages;
    ChannelType.GROUP | ChannelType.DM chatType;
    std::string currentClientEntityId;
    std::optional<Agent> targetAgentData;
    std::vector<std::optional<Agent>> allAgents;
    string | null animatedMessageId;
    React.RefObject<HTMLDivElement | null> scrollRef;
    std::optional<React.RefObject<HTMLDivElement | null>; // Optional content ref for StickToBottom> contentRef;
    bool isAtBottom;
    () => void scrollToBottom;
    () => void disableAutoScroll;
    UUID | undefined finalChannelId;
    std::optional<(agentId: UUID) => Partial<Agent> | undefined> getAgentInMessage;
    std::optional<std::unordered_map<UUID, string | null>> agentAvatarMap;
    (messageId: string) => void onDeleteMessage;
    (messageText: string) => void onRetryMessage;
    std::optional<UUID | null> selectedGroupAgentId;
};


const ChatMessageListComponent: React.FC<ChatMessageListComponentProps> = ({
  messages,
  isLoadingMessages,
  chatType,
  currentClientEntityId,
  targetAgentData,
  allAgents,
  animatedMessageId,
  scrollRef,
  contentRef,
  isAtBottom,
  scrollToBottom,
  disableAutoScroll,
  finalChannelId,
  getAgentInMessage,
  agentAvatarMap,
  onDeleteMessage,
  onRetryMessage,
  selectedGroupAgentId,
}) => {
  // Filter messages based on selected agent in group chat
  const filteredMessages = React.useMemo(() => {
    if (chatType === ChannelType.GROUP && selectedGroupAgentId) {
      return messages.filter((message) => {
        // Show user messages and messages from selected agent
        const isUser = message.senderId === currentClientEntityId;
        const isSelectedAgent = message.senderId === selectedGroupAgentId;
        return isUser || isSelectedAgent;
      });
    }
    return messages;
  }, [messages, chatType, selectedGroupAgentId, currentClientEntityId]);

  return (
    <ChatMessageList
      key={finalChannelId || 'no-channel'}
      scrollRef={scrollRef}
      contentRef={contentRef}
      isAtBottom={isAtBottom}
      scrollToBottom={scrollToBottom}
      disableAutoScroll={disableAutoScroll}
      className="h-full w-full"
      data-testid="chat-messages"
    >
      {isLoadingMessages && filteredMessages.length === 0 && (
        <div className="flex flex-1 justify-center items-center">
          <p>Loading messages...</p>
        </div>
      )}
      {!isLoadingMessages && filteredMessages.length === 0 && (
        <div className="flex flex-1 justify-center items-center">
          <p>No messages yet. Start the conversation!</p>
        </div>
      )}
      {filteredMessages.map((message: UiMessage, index: number) => {
        const isUser = message.senderId === currentClientEntityId;
        const shouldAnimate =
          index === filteredMessages.length - 1 &&
          message.isAgent &&
          message.id === animatedMessageId;

        const senderAgent =
          chatType === ChannelType.GROUP && !isUser && getAgentInMessage
            ? getAgentInMessage(message.senderId)
            : undefined;

        return (
          <div
            key={`${message.id}-${message.createdAt}`}
            className={cn('flex gap-1', isUser ? 'justify-end' : 'justify-start')}
          >
            <ChatBubble
              variant={isUser ? 'sent' : 'received'}
              className={`flex flex-row items-end gap-2 ${isUser ? 'flex-row-reverse' : ''}`}
            >
              {!isUser && (
                <Avatar className="size-8 border rounded-full select-none mb-2">
                  <AvatarImage
                    src={getAgentAvatar(
                      chatType === ChannelType.DM
                        ? targetAgentData
                        : senderAgent ||
                            (agentAvatarMap && message.senderId && allAgents
                              ? allAgents.find((a: Partial<Agent>) => a.id === message.senderId)
                              : undefined)
                    )}
                  />
                </Avatar>
              )}
              <MemoizedMessageContent
                message={message}
                agentForTts={
                  chatType === ChannelType.DM ? targetAgentData : (senderAgent as Agent | undefined)
                }
                shouldAnimate={shouldAnimate}
                onDelete={onDeleteMessage}
                onRetry={onRetryMessage}
                isUser={isUser}
                getAgentInMessage={chatType === ChannelType.GROUP ? getAgentInMessage : undefined}
                agentAvatarMap={chatType === ChannelType.GROUP ? agentAvatarMap : undefined}
                chatType={chatType}
              />
            </ChatBubble>
          </div>
        );
      })}
    </ChatMessageList>
  );
};

} // namespace elizaos
