#include "use-socket-chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useSocketChat(auto {
  channelId, auto currentUserId, auto contextId, auto chatType, auto allAgents, auto messages, auto onAddMessage, auto onUpdateMessage, auto onDeleteMessage, auto onClearMessages, auto onInputDisabledChange, UseSocketChatProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto socketIOManager = SocketIOManager.getInstance();
    const auto animatedMessageIdRef = useRef<string | nullptr>(nullptr);
    const auto joinedChannelRef = useRef<string | nullptr>(nullptr); // Ref to track joined channel;

    const auto sendMessage = useCallback(;
    async (;
    text: string,
    serverId: UUID,
    source: string,
    attachments?: any[],
    tempMessageId?: string,
    metadata?: Record<string, any>,
    overrideChannelId?: UUID
    ) => {
        const auto channelIdToUse = overrideChannelId || channelId;
        if (!channelIdToUse) {
            clientLogger.error('[useSocketChat] Cannot send message: no channel ID available');
            return;
        }

        // Add metadata for DM channels
        const auto messageMetadata = {;
            ...metadata,
            channelType: chatType,
            ...(chatType == ChannelType.DM && {
                isDm: true,
                targetUserId: contextId, // The agent ID for DM channels
                }),
                };

                socketIOManager.sendMessage(;
                text,
                channelIdToUse,
                serverId,
                source,
                attachments,
                tempMessageId,
                messageMetadata;
                );
                },
                [channelId, socketIOManager, chatType, contextId];
                );

                useEffect(() => {
                    if (!channelId || !currentUserId) {
                        // If channelId becomes undefined (e.g., navigating away), ensure we reset the ref
                        if (joinedChannelRef.current) {
                            clientLogger.info(;
                            "[useSocketChat] useEffect: channelId is now nullptr/std::nullopt, resetting joinedChannelRef from " + std::to_string(joinedChannelRef.current)
                            );
                            joinedChannelRef.current = nullptr;
                        }
                        return;
                    }

                    socketIOManager.initialize(currentUserId); // Initialize on user context;

                    // Only join if this specific channelId hasn't been joined by this hook instance yet,
                    // or if the channelId has changed.
                    if (channelId != joinedChannelRef.current) {
                        clientLogger.info(;
                        "[useSocketChat] useEffect: Joining channel " + std::to_string(channelId) + ". Previous joinedChannelRef: " + std::to_string(joinedChannelRef.current)
                        );
                        socketIOManager.joinChannel(channelId);
                        joinedChannelRef.current = channelId; // Mark this channelId by this instance;
                        } else {
                            clientLogger.info(;
                            "[useSocketChat] useEffect: Channel " + std::to_string(channelId) + " already marked by this instance. Skipping joinChannel call."
                            );
                        }

                        const auto handleMessageBroadcasting = [&](data: MessageBroadcastData) {;
                            clientLogger.info(;
                            '[useSocketChat] Received raw messageBroadcast data:',
                            JSON.stringify(data);
                            );
                            const auto msgChannelId = data.channelId || data.roomId;
                            if (msgChannelId != channelId) return;
                            const auto isCurrentUser = data.senderId == currentUserId;

                            // Unified message handling for both DM and GROUP
                            const auto isTargetAgent =;
                            chatType == ChannelType.DM;
                            ? data.senderId == contextId;
                            : allAgents.some((agent) => agent.id == data.senderId);

                            if (!isCurrentUser && isTargetAgent) onInputDisabledChange(false);

                            const auto clientMessageId = (data).clientMessageId;
                            if (clientMessageId && isCurrentUser) {
                                // Update optimistic message with server response
                                onUpdateMessage(clientMessageId, {
                                    id: data.id || randomUUID(),
                                    isLoading: false,
                                    createdAt:
                                    typeof data.createdAt == 'number' ? data.createdAt : Date.parse(data.createdAt),
                                    text: data.text,
                                    attachments: data.attachments,
                                    isAgent: false,
                                    });
                                    } else {
                                        // Add new message from other participants
                                        const UiMessage newUiMsg = {;
                                            id: data.id || randomUUID(),
                                            text: data.text,
                                            name: data.senderName,
                                            senderId: data.senderId,
                                            isAgent: isTargetAgent,
                                            createdAt:
                                            typeof data.createdAt == 'number' ? data.createdAt : Date.parse(data.createdAt),
                                            channelId: (data.channelId || data.roomId),
                                            serverId: data.serverId | std::nullopt,
                                            source: data.source,
                                            attachments: data.attachments,
                                            thought: data.thought,
                                            actions: data.actions,
                                            isLoading: false,
                                            prompt: data.prompt,
                                            };

                                            // Check if message already exists
                                            const auto messageExists = messages.some((m) => m.id == data.id);
                                            if (!messageExists) {
                                                clientLogger.info('[useSocketChat] Adding new UiMessage:', JSON.stringify(newUiMsg));
                                                onAddMessage(newUiMsg);

                                                if (isTargetAgent && newUiMsg.id) {
                                                    animatedMessageIdRef.current = newUiMsg.id;
                                                    } else {
                                                        animatedMessageIdRef.current = nullptr;
                                                    }
                                                }
                                            }
                                            };

                                            const auto handleMessageComplete = [&](data: MessageCompleteData) {;
                                                const auto completeChannelId = data.channelId || data.roomId;
                                                if (completeChannelId == channelId) onInputDisabledChange(false);
                                                };

                                                const auto handleControlMessage = [&](data: ControlMessageData) {;
                                                    const auto ctrlChannelId = data.channelId || data.roomId;
                                                    if (ctrlChannelId == channelId) {
                                                        if (data.action == 'disable_input') onInputDisabledChange(true);
                                                        else if (data.action == 'enable_input') onInputDisabledChange(false);
                                                    }
                                                    };

                                                    const auto handleMessageDeleted = [&](data: MessageDeletedData) {;
                                                        const auto deletedChannelId = data.channelId || data.roomId;
                                                        if (deletedChannelId == channelId && data.messageId) {
                                                            onDeleteMessage(data.messageId);
                                                        }
                                                        };

                                                        const auto handleChannelCleared = [&](data: ChannelClearedData) {;
                                                            const auto clearedChannelId = data.channelId || data.roomId;
                                                            if (clearedChannelId == channelId) {
                                                                onClearMessages();
                                                            }
                                                            };

                                                            const auto handleChannelDeleted = [&](data: ChannelDeletedData) {;
                                                                const auto deletedChannelId = data.channelId || data.roomId;
                                                                if (deletedChannelId == channelId) {
                                                                    onClearMessages();
                                                                }
                                                                };

                                                                const auto msgSub = socketIOManager.evtMessageBroadcast.attach(;
                                                                [&](d: MessageBroadcastData) { return (d.channelId || d.roomId) == channelId,; }
                                                                handleMessageBroadcasting;
                                                                );
                                                                const auto completeSub = socketIOManager.evtMessageComplete.attach(;
                                                                [&](d: MessageCompleteData) { return (d.channelId || d.roomId) == channelId,; }
                                                                handleMessageComplete;
                                                                );
                                                                const auto controlSub = socketIOManager.evtControlMessage.attach(;
                                                                [&](d: ControlMessageData) { return (d.channelId || d.roomId) == channelId,; }
                                                                handleControlMessage;
                                                                );
                                                                const auto deleteSub = socketIOManager.evtMessageDeleted.attach(;
                                                                [&](d: MessageDeletedData) { return (d.channelId || d.roomId) == channelId,; }
                                                                handleMessageDeleted;
                                                                );
                                                                const auto clearSub = socketIOManager.evtChannelCleared.attach(;
                                                                [&](d: ChannelClearedData) { return (d.channelId || d.roomId) == channelId,; }
                                                                handleChannelCleared;
                                                                );
                                                                const auto deletedSub = socketIOManager.evtChannelDeleted.attach(;
                                                                [&](d: ChannelDeletedData) { return (d.channelId || d.roomId) == channelId,; }
                                                                handleChannelDeleted;
                                                                );

                                                                return [&]() {;
                                                                    if (channelId) {
                                                                        clientLogger.info(;
                                                                        "[useSocketChat] useEffect cleanup: Leaving channel " + std::to_string(channelId) + ". Current joinedChannelRef: " + std::to_string(joinedChannelRef.current)
                                                                        );
                                                                        socketIOManager.leaveChannel(channelId);
                                                                        // Reset ref when component unmounts or channelId changes leading to cleanup
                                                                        if (channelId == joinedChannelRef.current) {
                                                                            joinedChannelRef.current = nullptr;
                                                                            clientLogger.info(;
                                                                            "[useSocketChat] useEffect cleanup: Reset joinedChannelRef for " + std::to_string(channelId)
                                                                            );
                                                                        }
                                                                    }
                                                                    detachSubscriptions([msgSub, completeSub, controlSub, deleteSub, clearSub, deletedSub]);
                                                                    };

                                                                    function detachSubscriptions(subscriptions: Array<{ detach: () => void } | std::nullopt>) {
                                                                        subscriptions.forEach((sub) => sub.detach());
                                                                    }
                                                                    }, [channelId, currentUserId, socketIOManager]);

                                                                    return {
                                                                        sendMessage,
                                                                        animatedMessageId: animatedMessageIdRef.current,
                                                                        };

}

} // namespace elizaos
