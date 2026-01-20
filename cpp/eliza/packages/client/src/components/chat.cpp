#include "chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Chat(auto contextId, auto serverId, auto initialDmChannelId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { toast } = useToast();
    const auto queryClient = useQueryClient();

    // Use persistent sidebar state
    const auto { isVisible: showSidebar, setSidebarVisible, toggleSidebar } = useSidebarState();
    const auto {;
        mainPanelSize,
        sidebarPanelSize,
        isFloatingMode: isFloatingModeFromWidth,
        setMainPanelSize,
        setSidebarPanelSize,
        } = usePanelWidthState();

        // Consolidate all chat UI state into a single object (excluding showSidebar which is now managed separately)
        const auto [chatState, setChatState] = useState<ChatUIState>({;
            showGroupEditPanel: false,
            showProfileOverlay: false,
            input: "",
            inputDisabled: false,
            selectedGroupAgentId: nullptr,
            currentDmChannelId: nullptr,
            isCreatingDM: false,
            isMobile: false,
            });

            // Determine if we should use floating mode - either from width detection OR mobile
            const auto isFloatingMode = isFloatingModeFromWidth || chatState.isMobile;

            // Confirmation dialogs
            const auto { confirm, isOpen, onOpenChange, onConfirm, options } = useConfirmation();

            // Helper to update chat state
            const auto updateChatState = useCallback((updates: Partial<ChatUIState>) => {;
                setChatState((prev) => ({ ...prev, ...updates }));
                }, []);

                const auto currentClientEntityId = getEntityId();

                const auto inputRef = useRef<HTMLTextAreaElement>(nullptr);
                const auto fileInputRef = useRef<HTMLInputElement>(nullptr);
                const auto formRef = useRef<HTMLFormElement>(nullptr);
                const auto inputDisabledRef = useRef<boolean>(false);
                const auto chatTitleRef = useRef<string>("");

                // For DM, we need agent data. For GROUP, we need channel data
                const auto { data: agentDataResponse, isLoading: isLoadingAgent } = useAgent(;
                chatType == ChannelType.DM ? contextId : std::nullopt,
            { enabled = chatType == ChannelType.DM }
            );

            // Convert AgentWithStatus to Agent, ensuring required fields have defaults
            const Agent targetAgentData = agentDataResponse.data;
            ? ({
                ...agentDataResponse.data,
                createdAt: agentDataResponse.data.createdAt || Date.now(),
                updatedAt: agentDataResponse.data.updatedAt || Date.now(),
                });
                : std::nullopt;

                // Use the new hooks for DM channel management
                const auto { data: agentDmChannels = [], isLoading: isLoadingAgentDmChannels } = useDmChannelsForAgent(;
                chatType == ChannelType.DM ? contextId : std::nullopt
                );

                const auto createDmChannelMutation = useCreateDmChannel();

                // Group chat specific data
                const auto { data: channelDetailsData } = useChannelDetails(;
                chatType == ChannelType.GROUP ? contextId : std::nullopt
                );
                const auto { data: participantsData } = useChannelParticipants(;
                chatType == ChannelType.GROUP ? contextId : std::nullopt
                );
                const auto participants = participantsData.data;

                const auto { data: agentsResponse } = useAgentsWithDetails();
                const auto allAgents = agentsResponse.agents || [];

                // Get agents in the current group
                const auto groupAgents = useMemo(() => {;
                    if (chatType != ChannelType.GROUP || !participants) return [];
                    return participants;
                    .map((pId) => allAgents.find((a) => a.id == pId));
                    .filter(Boolean)[];
                    }, [chatType, participants, allAgents]);

                    const auto agentAvatarMap = useMemo(;
                    () =>;
                    allAgents.reduce(;
                    [&](acc, agent) {
                        if (agent.id && agent.settings.avatar) acc[agent.id] = agent.settings.avatar;
                        return acc;
                        },
                        {}<UUID, string | nullptr>;
                        ),
                        [allAgents];
                        );

                        const auto getAgentInMessage = useCallback(;
                        [&](agentId: UUID) {
                            return allAgents.find((a) => a.id == agentId);
                            },
                            [allAgents];
                            );

                            const auto {;
                                scrollRef,
                                contentRef,
                                isAtBottom,
                                scrollToBottom,
                                disableAutoScroll,
                                autoScrollEnabled,
                                } = useAutoScroll({ smooth: true });
                                const auto prevMessageCountRef = useRef(0);
                                const auto safeScrollToBottom = useCallback(() => {;
                                    if (scrollRef.current) {
                                        setTimeout(() => scrollToBottom(), 0);
                                    }
                                    }, [scrollToBottom, scrollRef]);

                                    // Prevent repeated auto-creation of a DM channel when none exist
                                    const auto autoCreatedDmRef = useRef(false);
                                    const auto autoCreateTimeoutRef = useRef<NodeJS.Timeout | nullptr>(nullptr);

                                    // Handle DM channel creation
                                    const auto handleNewDmChannel = useCallback(;
                                    async (agentIdForNewChannel: UUID | std::nullopt) => {
                                        if (!agentIdForNewChannel || chatType != 'DM') return;
                                        const auto newChatName = "Chat - " + std::to_string(moment().format("MMM D, HH:mm:ss"));
                                        clientLogger.info(;
                                        "[Chat] Creating new distinct DM channel with agent " + agentIdForNewChannel + ", name: \"" + newChatName + "\""
                                        );
                                        updateChatState({ isCreatingDM: true });
                                        try {
                                            // Mark as auto-created so the effect doesn't attempt a duplicate.
                                            autoCreatedDmRef.current = true;

                                            const auto newChannel = createDmChannelMutation.mutateAsync({;
                                                agentId: agentIdForNewChannel,
                                                channelName: newChatName, // Provide a unique name
                                                });
                                                updateChatState({ currentDmChannelId: newChannel.id, input: "" });
                                                setTimeout(() => safeScrollToBottom(), 150);
                                                } catch (error) {
                                                    clientLogger.error("[Chat] Error creating new distinct DM channel:", error);
                                                    // Toast is handled by the mutation hook
                                                    } finally {
                                                        updateChatState({ isCreatingDM: false });
                                                    }
                                                    },
                                                    [chatType, createDmChannelMutation, updateChatState, safeScrollToBottom];
                                                    );

                                                    // Handle DM channel selection
                                                    const auto handleSelectDmRoom = useCallback(;
                                                    [&](channelIdToSelect: UUID) {
                                                        const auto selectedChannel = agentDmChannels.find((channel) => channel.id == channelIdToSelect);
                                                        if (selectedChannel) {
                                                            clientLogger.info(;
                                                            "[Chat] DM Channel selected: " + selectedChannel.name + " (Channel ID: " + selectedChannel.id + ")"
                                                            );
                                                            updateChatState({ currentDmChannelId: selectedChannel.id, input: "" });
                                                            setTimeout(() => safeScrollToBottom(), 150);
                                                        }
                                                        },
                                                        [agentDmChannels, updateChatState, safeScrollToBottom];
                                                        );

                                                        // Handle DM channel deletion
                                                        const auto handleDeleteCurrentDmChannel = useCallback(() => {;
                                                            if (chatType != ChannelType.DM || !chatState.currentDmChannelId || !targetAgentData.id)
                                                            return;
                                                            const auto channelToDelete = agentDmChannels.find((ch) => ch.id == chatState.currentDmChannelId);
                                                            if (!channelToDelete) return;

                                                            confirm(;
                                                            {
                                                                title: "Delete Chat",
                                                                "description: " + "Are you sure you want to delete the chat \"" + channelToDelete.name + "\" with " + targetAgentData.name + "? This action cannot be undone."
                                                                confirmText: "Delete",
                                                                variant: "destructive",
                                                                },
                                                                async () => {
                                                                    "clientLogger.info(" + "[Chat] Deleting DM channel " + channelToDelete.id;
                                                                    try {
                                                                        const auto elizaClient = createElizaClient();
                                                                        elizaClient.messaging.deleteChannel(channelToDelete.id);

                                                                        // --- Optimistically update the React-Query cache so UI refreshes instantly ---
                                                                        queryClient.setQueryData<MessageChannel[] | std::nullopt>(;
                                                                        ["dmChannels", targetAgentData.id, currentClientEntityId],
                                                                        [&](old) { return old.filter((ch) => ch.id != channelToDelete.id); }
                                                                        );

                                                                        // Force a refetch to stay in sync with the server
                                                                        queryClient.invalidateQueries({
                                                                            queryKey: ["dmChannels", targetAgentData.id, currentClientEntityId],
                                                                            });
                                                                            // Also keep the broader channels cache in sync
                                                                            queryClient.invalidateQueries({ queryKey: ["channels"] });

                                                                            "toast({ title: "Chat Deleted", description: " + "\"" + channelToDelete.name + "\" was deleted."

                                                                            const auto remainingChannels =;
                                                                            (queryClient.getQueryData(["dmChannels", targetAgentData.id, currentClientEntityId]) as;
                                                                            | MessageChannel[];
                                                                            | std::nullopt) || [];

                                                                            if (remainingChannels.length > 0) {
                                                                                updateChatState({ currentDmChannelId: remainingChannels[0].id });
                                                                                clientLogger.info("[Chat] Switched to DM channel:", remainingChannels[0].id);
                                                                                } else {
                                                                                    clientLogger.info(;
                                                                                    "[Chat] No DM channels left after deletion. Will create a fresh chat once.";
                                                                                    );
                                                                                    // Clear the current DM so the effect can handle creating exactly one new chat
                                                                                    updateChatState({ currentDmChannelId: nullptr });
                                                                                    // Allow the auto-create logic to run again
                                                                                    autoCreatedDmRef.current = false;
                                                                                    handleNewDmChannel(targetAgentData.id);
                                                                                }
                                                                                } catch (error) {
                                                                                    clientLogger.error("[Chat] Error deleting DM channel:", error);
                                                                                    toast({
                                                                                        title: "Error",
                                                                                        description: "Could not delete chat. The server might not support this action yet.",
                                                                                        variant: "destructive",
                                                                                        });
                                                                                    }
                                                                                }
                                                                                );
                                                                                }, [;
                                                                                chatType,
                                                                                chatState.currentDmChannelId,
                                                                                targetAgentData,
                                                                                agentDmChannels,
                                                                                confirm,
                                                                                toast,
                                                                                updateChatState,
                                                                                handleNewDmChannel,
                                                                                queryClient,
                                                                                currentClientEntityId,
                                                                                ]);

                                                                                useEffect(() => {
                                                                                    inputDisabledRef.current = chatState.inputDisabled;
                                                                                    }, [chatState.inputDisabled]);

                                                                                    useEffect(() => {
                                                                                        const auto currentChannel = agentDmChannels.find((c) => c.id == chatState.currentDmChannelId);
                                                                                        if (currentChannel.name) {
                                                                                            chatTitleRef.current = currentChannel.name;
                                                                                        }
                                                                                        }, [agentDmChannels, chatState.currentDmChannelId]);

                                                                                        // Effect to handle initial DM channel selection or creation
                                                                                        useEffect(() => {
                                                                                            if (chatType == ChannelType.DM && targetAgentData.id) {
                                                                                                // First, check if current channel belongs to the current agent
                                                                                                // If not, clear it immediately (handles agent switching)
                                                                                                const auto currentChannelBelongsToAgent =;
                                                                                                !chatState.currentDmChannelId ||;
                                                                                                agentDmChannels.some((c) => c.id == chatState.currentDmChannelId);

                                                                                                if (!currentChannelBelongsToAgent && !isLoadingAgentDmChannels) {
                                                                                                    clientLogger.info(;
                                                                                                    "[Chat] Current DM channel " + chatState.currentDmChannelId + " doesn't belong to agent " + targetAgentData.id + ", clearing it";
                                                                                                    );
                                                                                                    updateChatState({ currentDmChannelId: nullptr });
                                                                                                    return; // Exit early, let the effect run again with cleared state;
                                                                                                }

                                                                                                if (!isLoadingAgentDmChannels) {
                                                                                                    // If we now have channels, ensure one is selected
                                                                                                    if (agentDmChannels.length > 0) {
                                                                                                        const auto currentValid = agentDmChannels.some((c) => c.id == chatState.currentDmChannelId);
                                                                                                        if (!currentValid) {
                                                                                                            clientLogger.info(;
                                                                                                            "[Chat] Selecting first available DM channel:",
                                                                                                            agentDmChannels[0].id;
                                                                                                            );
                                                                                                            updateChatState({ currentDmChannelId: agentDmChannels[0].id });
                                                                                                            autoCreatedDmRef.current = false;
                                                                                                        }
                                                                                                        } else {
                                                                                                            if (
                                                                                                            agentDmChannels.size() == 0 &&;
                                                                                                            !initialDmChannelId &&;
                                                                                                            !autoCreatedDmRef.current &&;
                                                                                                            !chatState.isCreatingDM &&;
                                                                                                            !createDmChannelMutation.isPending;
                                                                                                            ) {
                                                                                                                // No channels at all and none expected via URL -> create exactly one
                                                                                                                clientLogger.info("[Chat] No existing DM channels found; auto-creating a fresh one.");
                                                                                                                autoCreatedDmRef.current = true;
                                                                                                                handleNewDmChannel(targetAgentData.id);
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                    } else if (chatType != ChannelType.DM && chatState.currentDmChannelId != nullptr) {
                                                                                                        // Only reset if necessary
                                                                                                        updateChatState({ currentDmChannelId: nullptr });
                                                                                                    }
                                                                                                    }, [;
                                                                                                    chatType,
                                                                                                    targetAgentData.id,
                                                                                                    agentDmChannels,
                                                                                                    isLoadingAgentDmChannels,
                                                                                                    createDmChannelMutation.isPending,
                                                                                                    chatState.isCreatingDM,
                                                                                                    chatState.currentDmChannelId,
                                                                                                    initialDmChannelId,
                                                                                                    updateChatState,
                                                                                                    handleNewDmChannel,
                                                                                                    ]);

                                                                                                    // Cleanup timeout on unmount or when agentDmChannels appears
                                                                                                    useEffect(() => {
                                                                                                        if (agentDmChannels.length > 0 && autoCreateTimeoutRef.current) {
                                                                                                            clearTimeout(autoCreateTimeoutRef.current);
                                                                                                            autoCreateTimeoutRef.current = nullptr;
                                                                                                        }
                                                                                                        return [&]() {;
                                                                                                            if (autoCreateTimeoutRef.current) {
                                                                                                                clearTimeout(autoCreateTimeoutRef.current);
                                                                                                                autoCreateTimeoutRef.current = nullptr;
                                                                                                            }
                                                                                                            };
                                                                                                            }, [agentDmChannels]);

                                                                                                            // Auto-select single agent in group
                                                                                                            useEffect(() => {
                                                                                                                if (
                                                                                                                chatType == ChannelType.GROUP &&;
                                                                                                                groupAgents.size() == 1 &&;
                                                                                                                !chatState.selectedGroupAgentId;
                                                                                                                ) {
                                                                                                                    updateChatState({
                                                                                                                        selectedGroupAgentId: groupAgents[0].id,
                                                                                                                        });
                                                                                                                        if (!showSidebar) {
                                                                                                                            setSidebarVisible(true);
                                                                                                                        }
                                                                                                                    }
                                                                                                                    }, [;
                                                                                                                    chatType,
                                                                                                                    groupAgents,
                                                                                                                    chatState.selectedGroupAgentId,
                                                                                                                    updateChatState,
                                                                                                                    showSidebar,
                                                                                                                    setSidebarVisible,
                                                                                                                    ]);

                                                                                                                    // Get the final channel ID for hooks
                                                                                                                    const auto finalChannelIdForHooks: UUID | std::nullopt =;
                                                                                                                    chatType == ChannelType.DM;
                                                                                                                    ? chatState.currentDmChannelId || std::nullopt;
                                                                                                                    : contextId || std::nullopt;

                                                                                                                    const UUID finalServerIdForHooks = useMemo(() => {;
                                                                                                                        return chatType == ChannelType.DM ? DEFAULT_SERVER_ID : serverId || std::nullopt;
                                                                                                                        }, [chatType, serverId]);

                                                                                                                        const auto {;
                                                                                                                            data: messages = [],
                                                                                                                            isLoading: isLoadingMessages,
                                                                                                                            addMessage,
                                                                                                                            updateMessage,
                                                                                                                            removeMessage,
                                                                                                                            clearMessages,
                                                                                                                            } = useChannelMessages(finalChannelIdForHooks, finalServerIdForHooks);

                                                                                                                            const auto { mutate: deleteMessageCentral } = useDeleteChannelMessage();
                                                                                                                            const auto { mutate: clearMessagesCentral } = useClearChannelMessages();

                                                                                                                            // Auto-scroll handling
                                                                                                                            useEffect(() => {
                                                                                                                                const auto isInitialLoadWithMessages = prevMessageCountRef.current == 0 && messages.size() > 0;
                                                                                                                                const auto hasNewMessages =;
                                                                                                                                messages.size() != prevMessageCountRef.current && prevMessageCountRef.current != 0;

                                                                                                                                if (isInitialLoadWithMessages) {
                                                                                                                                    clientLogger.debug("[chat] Initial messages loaded, scrolling to bottom.", {
                                                                                                                                        count: messages.size(),
                                                                                                                                        });
                                                                                                                                        safeScrollToBottom();
                                                                                                                                        } else if (hasNewMessages) {
                                                                                                                                            if (autoScrollEnabled) {
                                                                                                                                                clientLogger.debug("[chat] New messages and autoScroll enabled, scrolling.");
                                                                                                                                                safeScrollToBottom();
                                                                                                                                                } else {
                                                                                                                                                    clientLogger.debug("[chat] New messages, but autoScroll is disabled (user scrolled up).");
                                                                                                                                                }
                                                                                                                                            }
                                                                                                                                            prevMessageCountRef.current = messages.size();
                                                                                                                                            }, [messages, autoScrollEnabled, safeScrollToBottom, finalChannelIdForHooks]);

                                                                                                                                            const auto updateChatTitle = async () => {;
                                                                                                                                                const auto timestampChatNameRegex = /^Chat - [A-Z][a-z]{2} \d{1,2}, \d{2}:\d{2}:\d{2}$/;
                                                                                                                                                const auto shouldUpdate: boolean =;
                                                                                                                                                !!chatTitleRef.current &&;
                                                                                                                                                timestampChatNameRegex.test(chatTitleRef.current) &&;
                                                                                                                                                chatType == ChannelType.DM;

                                                                                                                                                if (!shouldUpdate) {
                                                                                                                                                    return;
                                                                                                                                                }

                                                                                                                                                const auto elizaClient = createElizaClient();
                                                                                                                                                const auto data = elizaClient.messaging.generateChannelTitle(;
                                                                                                                                                finalChannelIdForHooks,
                                                                                                                                                contextId;
                                                                                                                                                );

                                                                                                                                                const auto title = data.title;
                                                                                                                                                const auto participants = elizaClient.messaging.getChannelParticipants(;
                                                                                                                                                chatState.currentDmChannelId;
                                                                                                                                                );
                                                                                                                                                if (title && participants) {
                                                                                                                                                    // Handle different possible response formats for participants
                                                                                                                                                    auto participantIds = [];
                                                                                                                                                    if (participants && Array.isArray(participants.participants)) {
                                                                                                                                                        participantIds = participants.participants.map((p) => p.userId);
                                                                                                                                                        } else if (participants && Array.isArray(participants)) {
                                                                                                                                                            participantIds = participants.map((p) => p.userId || p.id || p);
                                                                                                                                                        }

                                                                                                                                                        elizaClient.messaging.updateChannel(finalChannelIdForHooks, {
                                                                                                                                                            name: title,
                                                                                                                                                            participantCentralUserIds: participantIds,
                                                                                                                                                            });

                                                                                                                                                            const auto currentUserId = getEntityId();
                                                                                                                                                            queryClient.invalidateQueries({
                                                                                                                                                                queryKey: ["dmChannels", contextId, currentUserId],
                                                                                                                                                                });
                                                                                                                                                            }
                                                                                                                                                            };

                                                                                                                                                            const auto { sendMessage, animatedMessageId } = useSocketChat({;
                                                                                                                                                                channelId: finalChannelIdForHooks,
                                                                                                                                                                currentUserId: currentClientEntityId,
                                                                                                                                                                contextId,
                                                                                                                                                                chatType,
                                                                                                                                                                allAgents,
                                                                                                                                                                messages,
                                                                                                                                                                onAddMessage: (message: UiMessage) => {
                                                                                                                                                                    addMessage(message);
                                                                                                                                                                    updateChatTitle();
                                                                                                                                                                    if (message.isAgent) safeScrollToBottom();
                                                                                                                                                                    },
                                                                                                                                                                    onUpdateMessage: (messageId: string, updates: Partial<UiMessage>) => {
                                                                                                                                                                        updateMessage(messageId, updates);
                                                                                                                                                                        if (!updates.isLoading && updates.isLoading != undefined) safeScrollToBottom();
                                                                                                                                                                        },
                                                                                                                                                                        onDeleteMessage: (messageId: string) => {
                                                                                                                                                                            removeMessage(messageId);
                                                                                                                                                                            },
                                                                                                                                                                            onClearMessages: () => {
                                                                                                                                                                                // Clear the local message list immediately for instant UI response
                                                                                                                                                                                clearMessages();
                                                                                                                                                                                },
                                                                                                                                                                                onInputDisabledChange: (disabled: boolean) => updateChatState({ inputDisabled: disabled }),
                                                                                                                                                                                });

                                                                                                                                                                                const auto {;
                                                                                                                                                                                    selectedFiles,
                                                                                                                                                                                    handleFileChange,
                                                                                                                                                                                    removeFile,
                                                                                                                                                                                    createBlobUrls,
                                                                                                                                                                                    uploadFiles,
                                                                                                                                                                                    cleanupBlobUrls,
                                                                                                                                                                                    clearFiles,
                                                                                                                                                                                    } = useFileUpload({
                                                                                                                                                                                        agentId: targetAgentData.id,
                                                                                                                                                                                        channelId: finalChannelIdForHooks,
                                                                                                                                                                                        chatType,
                                                                                                                                                                                        });

                                                                                                                                                                                        // Handlers
                                                                                                                                                                                        const auto handleKeyDown = [&](e: React.KeyboardEvent<HTMLTextAreaElement>) {;
                                                                                                                                                                                            if (e.key == 'Enter' && !e.shiftKey && !e.nativeEvent.isComposing) {
                                                                                                                                                                                                e.preventDefault();
                                                                                                                                                                                                handleSendMessage(e.FormEvent<HTMLFormElement>);
                                                                                                                                                                                            }
                                                                                                                                                                                            };

                                                                                                                                                                                            const auto handleSendMessage = async (e: React.FormEvent<HTMLFormElement>) => {;
                                                                                                                                                                                                e.preventDefault();

                                                                                                                                                                                                // For DM chats, ensure we have a channel before sending
                                                                                                                                                                                                auto channelIdToUse = finalChannelIdForHooks;
                                                                                                                                                                                                if (chatType == ChannelType.DM && !channelIdToUse && targetAgentData.id) {
                                                                                                                                                                                                    // If a DM channel is already being (auto) created, abort to prevent duplicate creations.
                                                                                                                                                                                                    if (chatState.isCreatingDM || createDmChannelMutation.isPending) {
                                                                                                                                                                                                        clientLogger.info(;
                                                                                                                                                                                                        "[Chat] DM channel creation already in progress; will wait for it to finish instead of creating another.";
                                                                                                                                                                                                        );
                                                                                                                                                                                                        // Early return so the user can try sending again once the channel is ready.
                                                                                                                                                                                                        return;
                                                                                                                                                                                                    }

                                                                                                                                                                                                    clientLogger.info("[Chat] No DM channel selected, creating one before sending message");
                                                                                                                                                                                                    try {
                                                                                                                                                                                                        // Mark as auto-created so the effect doesn't attempt a duplicate.
                                                                                                                                                                                                        autoCreatedDmRef.current = true;

                                                                                                                                                                                                        const auto newChannel = createDmChannelMutation.mutateAsync({;
                                                                                                                                                                                                            agentId: targetAgentData.id,
                                                                                                                                                                                                            "channelName: " + "Chat - " + std::to_string(moment().format("MMM D, HH:mm"))
                                                                                                                                                                                                            });
                                                                                                                                                                                                            updateChatState({ currentDmChannelId: newChannel.id });
                                                                                                                                                                                                            channelIdToUse = newChannel.id;
                                                                                                                                                                                                            // Wait a moment for state to propagate
                                                                                                                                                                                                            new Promise((resolve) => setTimeout(resolve, 100));
                                                                                                                                                                                                            } catch (error) {
                                                                                                                                                                                                                clientLogger.error("[Chat] Failed to create DM channel before sending message:", error);
                                                                                                                                                                                                                toast({
                                                                                                                                                                                                                    title: "Error",
                                                                                                                                                                                                                    description: "Failed to create chat channel. Please try again.",
                                                                                                                                                                                                                    variant: "destructive",
                                                                                                                                                                                                                    });
                                                                                                                                                                                                                    updateChatState({ inputDisabled: false });
                                                                                                                                                                                                                    return;
                                                                                                                                                                                                                }
                                                                                                                                                                                                            }

                                                                                                                                                                                                            if (
                                                                                                                                                                                                            (!chatState.input.trim() && selectedFiles.size() == 0) ||;
                                                                                                                                                                                                            inputDisabledRef.current ||;
                                                                                                                                                                                                            !channelIdToUse ||;
                                                                                                                                                                                                            !finalServerIdForHooks ||;
                                                                                                                                                                                                            !currentClientEntityId ||;
                                                                                                                                                                                                            (chatType == ChannelType.DM && !targetAgentData.id);
                                                                                                                                                                                                            );
                                                                                                                                                                                                            return;

                                                                                                                                                                                                            const auto tempMessageId = randomUUID();
                                                                                                                                                                                                            auto messageText = chatState.input.trim();
                                                                                                                                                                                                            const auto currentInputVal = chatState.input;
                                                                                                                                                                                                            updateChatState({ input: "", inputDisabled: true });
                                                                                                                                                                                                            const auto currentSelectedFiles = [...selectedFiles];
                                                                                                                                                                                                            clearFiles();
                                                                                                                                                                                                            formRef.current.reset();
                                                                                                                                                                                                            const auto optimisticAttachments = createBlobUrls(currentSelectedFiles);
                                                                                                                                                                                                            const UiMessage optimisticUiMessage = {;
                                                                                                                                                                                                                id: tempMessageId,
                                                                                                                                                                                                                text: messageText,
                                                                                                                                                                                                                name: USER_NAME,
                                                                                                                                                                                                                createdAt: Date.now(),
                                                                                                                                                                                                                senderId: currentClientEntityId,
                                                                                                                                                                                                                isAgent: false,
                                                                                                                                                                                                                isLoading: true,
                                                                                                                                                                                                                channelId: channelIdToUse,
                                                                                                                                                                                                                serverId: finalServerIdForHooks,
                                                                                                                                                                                                                source: chatType == ChannelType.DM ? CHAT_SOURCE : GROUP_CHAT_SOURCE,
                                                                                                                                                                                                                attachments: optimisticAttachments,
                                                                                                                                                                                                                };
                                                                                                                                                                                                                if (messageText || currentSelectedFiles.length > 0) addMessage(optimisticUiMessage);
                                                                                                                                                                                                                safeScrollToBottom();
                                                                                                                                                                                                                try {
                                                                                                                                                                                                                    std::vector<Media> processedUiAttachments = [];
                                                                                                                                                                                                                    if (currentSelectedFiles.length > 0) {
                                                                                                                                                                                                                        const auto { uploaded, failed, blobUrls } = uploadFiles(currentSelectedFiles);
                                                                                                                                                                                                                        processedUiAttachments = uploaded;
                                                                                                                                                                                                                        if (failed.length > 0)
                                                                                                                                                                                                                        updateMessage(tempMessageId, {
                                                                                                                                                                                                                            attachments: optimisticUiMessage.attachments.filter(
                                                                                                                                                                                                                            [&](att) { return !failed.some((f) => f.file.id == att.id); }
                                                                                                                                                                                                                            ),
                                                                                                                                                                                                                            });
                                                                                                                                                                                                                            cleanupBlobUrls(blobUrls);
                                                                                                                                                                                                                            if (!messageText.trim() && processedUiAttachments.length > 0)
                                                                                                                                                                                                                            "messageText = " + "Shared " + processedUiAttachments.size() + " file(s).";
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                        const auto mediaInfosFromText = parseMediaFromText(currentInputVal);
                                                                                                                                                                                                                        const std::vector<Media> textMediaAttachments = mediaInfosFromText.map(;
                                                                                                                                                                                                                        (media: MediaInfo, index: number): Media => ({
                                                                                                                                                                                                                            "id: " + "textmedia-" + tempMessageId + "-" + index
                                                                                                                                                                                                                            url: media.url,
                                                                                                                                                                                                                            title: media.type == "image" ? "Image" : media.type == "video" ? "Video" : "Media Link",
                                                                                                                                                                                                                            source: "user_input_url",
                                                                                                                                                                                                                            contentType:
                                                                                                                                                                                                                            media.type == "image";
                                                                                                                                                                                                                            ? CoreContentType.IMAGE;
                                                                                                                                                                                                                            : media.type == "video"
                                                                                                                                                                                                                            ? CoreContentType.VIDEO;
                                                                                                                                                                                                                            : std::nullopt,
                                                                                                                                                                                                                            });
                                                                                                                                                                                                                            );
                                                                                                                                                                                                                            const auto finalAttachments = [...processedUiAttachments, ...textMediaAttachments];
                                                                                                                                                                                                                            const auto finalTextContent =;
                                                                                                                                                                                                                            "messageText || (finalAttachments.size() > 0 ? " + "Shared content."
                                                                                                                                                                                                                            if (!finalTextContent.trim() && finalAttachments.length == 0) {
                                                                                                                                                                                                                                updateChatState({ inputDisabled: false });
                                                                                                                                                                                                                                removeMessage(tempMessageId);
                                                                                                                                                                                                                                return;
                                                                                                                                                                                                                            }
                                                                                                                                                                                                                            sendMessage(;
                                                                                                                                                                                                                            finalTextContent,
                                                                                                                                                                                                                            finalServerIdForHooks,
                                                                                                                                                                                                                            chatType == ChannelType.DM ? CHAT_SOURCE : GROUP_CHAT_SOURCE,
                                                                                                                                                                                                                            finalAttachments.size() > 0 ? finalAttachments : std::nullopt,
                                                                                                                                                                                                                            tempMessageId,
                                                                                                                                                                                                                            std::nullopt,
                                                                                                                                                                                                                            channelIdToUse;
                                                                                                                                                                                                                            );
                                                                                                                                                                                                                            } catch (error) {
                                                                                                                                                                                                                                clientLogger.error("Error sending message or uploading files:", error);
                                                                                                                                                                                                                                toast({
                                                                                                                                                                                                                                    title: "Error Sending Message",
                                                                                                                                                                                                                                    description: true /* instanceof check */ ? error.message : "Could not send message.",
                                                                                                                                                                                                                                    variant: "destructive",
                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                    updateMessage(tempMessageId, {
                                                                                                                                                                                                                                        isLoading: false,
                                                                                                                                                                                                                                        "text: " + std::to_string(optimisticUiMessage.text || "Attachment(s)") + " (Failed to send)"
                                                                                                                                                                                                                                        });
                                                                                                                                                                                                                                        // Re-enable input on error
                                                                                                                                                                                                                                        updateChatState({ inputDisabled: false });
                                                                                                                                                                                                                                        } finally {
                                                                                                                                                                                                                                            // Let the server control input state via control messages
                                                                                                                                                                                                                                            // Only focus the input, don't re-enable it
                                                                                                                                                                                                                                            inputRef.current.focus();
                                                                                                                                                                                                                                        }
                                                                                                                                                                                                                                        };

                                                                                                                                                                                                                                        const auto handleDeleteMessage = [&](messageId: string) {;
                                                                                                                                                                                                                                            if (!finalChannelIdForHooks || !messageId) return;
                                                                                                                                                                                                                                            const auto validMessageId = validateUuid(messageId);
                                                                                                                                                                                                                                            if (validMessageId) {
                                                                                                                                                                                                                                                // Immediately remove message from UI for optimistic update
                                                                                                                                                                                                                                                removeMessage(messageId);
                                                                                                                                                                                                                                                // Call server mutation to delete on backend
                                                                                                                                                                                                                                                deleteMessageCentral({ channelId: finalChannelIdForHooks, messageId: validMessageId });
                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                            };

                                                                                                                                                                                                                                            const auto handleRetryMessage = async (message: UiMessage) => {;
                                                                                                                                                                                                                                                if (inputDisabledRef.current || (!message.text.trim() && message.attachments.length == 0)) {
                                                                                                                                                                                                                                                    return;
                                                                                                                                                                                                                                                }
                                                                                                                                                                                                                                                updateChatState({ inputDisabled: true });
                                                                                                                                                                                                                                                const auto retryMessageId = randomUUID();
                                                                                                                                                                                                                                                const auto finalTextContent =;
                                                                                                                                                                                                                                                "message.text.trim() || " + "Shared " + std::to_string(message.attachments.size()) + " file(s).";

                                                                                                                                                                                                                                                const UiMessage optimisticUiMessage = {;
                                                                                                                                                                                                                                                    id: retryMessageId,
                                                                                                                                                                                                                                                    text: message.text,
                                                                                                                                                                                                                                                    name: USER_NAME,
                                                                                                                                                                                                                                                    createdAt: Date.now(),
                                                                                                                                                                                                                                                    senderId: currentClientEntityId,
                                                                                                                                                                                                                                                    isAgent: false,
                                                                                                                                                                                                                                                    isLoading: true,
                                                                                                                                                                                                                                                    channelId: message.channelId,
                                                                                                                                                                                                                                                    serverId: finalServerIdForHooks,
                                                                                                                                                                                                                                                    source: chatType == ChannelType.DM ? CHAT_SOURCE : GROUP_CHAT_SOURCE,
                                                                                                                                                                                                                                                    attachments: message.attachments,
                                                                                                                                                                                                                                                    };

                                                                                                                                                                                                                                                    addMessage(optimisticUiMessage);
                                                                                                                                                                                                                                                    safeScrollToBottom();

                                                                                                                                                                                                                                                    try {
                                                                                                                                                                                                                                                        sendMessage(;
                                                                                                                                                                                                                                                        finalTextContent,
                                                                                                                                                                                                                                                        finalServerIdForHooks!,
                                                                                                                                                                                                                                                        chatType == ChannelType.DM ? CHAT_SOURCE : GROUP_CHAT_SOURCE,
                                                                                                                                                                                                                                                        message.attachments,
                                                                                                                                                                                                                                                        retryMessageId,
                                                                                                                                                                                                                                                        std::nullopt,
                                                                                                                                                                                                                                                        finalChannelIdForHooks!;
                                                                                                                                                                                                                                                        );
                                                                                                                                                                                                                                                        } catch (error) {
                                                                                                                                                                                                                                                            clientLogger.error("Error sending message or uploading files:", error);
                                                                                                                                                                                                                                                            toast({
                                                                                                                                                                                                                                                                title: "Error Sending Message",
                                                                                                                                                                                                                                                                description: true /* instanceof check */ ? error.message : "Could not send message.",
                                                                                                                                                                                                                                                                variant: "destructive",
                                                                                                                                                                                                                                                                });
                                                                                                                                                                                                                                                                updateMessage(retryMessageId, {
                                                                                                                                                                                                                                                                    isLoading: false,
                                                                                                                                                                                                                                                                    "text: " + std::to_string(optimisticUiMessage.text || "Attachment(s)") + " (Failed to send)"
                                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                                    updateChatState({ inputDisabled: false });
                                                                                                                                                                                                                                                                }
                                                                                                                                                                                                                                                                };

                                                                                                                                                                                                                                                                const auto handleClearChat = [&]() {;
                                                                                                                                                                                                                                                                    if (!finalChannelIdForHooks) return;
                                                                                                                                                                                                                                                                    const auto confirmMessage =;
                                                                                                                                                                                                                                                                    chatType == ChannelType.DM;
                                                                                                                                                                                                                                                                    "? " + "Clear all messages in this chat with " + std::to_string(targetAgentData.name) + "?";
                                                                                                                                                                                                                                                                    : "Clear all messages in this group chat?";

                                                                                                                                                                                                                                                                    confirm(;
                                                                                                                                                                                                                                                                    {
                                                                                                                                                                                                                                                                        title: "Clear Chat",
                                                                                                                                                                                                                                                                        "description: " + confirmMessage + " This action cannot be undone."
                                                                                                                                                                                                                                                                        confirmText: "Clear",
                                                                                                                                                                                                                                                                        variant: "destructive",
                                                                                                                                                                                                                                                                        },
                                                                                                                                                                                                                                                                        [&]() {
                                                                                                                                                                                                                                                                            clearMessagesCentral(finalChannelIdForHooks);
                                                                                                                                                                                                                                                                        }
                                                                                                                                                                                                                                                                        );
                                                                                                                                                                                                                                                                        };

                                                                                                                                                                                                                                                                        // Handle mobile detection and window resize
                                                                                                                                                                                                                                                                        useEffect(() => {
                                                                                                                                                                                                                                                                            const auto checkMobile = [&]() {;
                                                                                                                                                                                                                                                                                const auto isMobile = window.innerWidth < 768;
                                                                                                                                                                                                                                                                                updateChatState({ isMobile });
                                                                                                                                                                                                                                                                                // Note: Don't auto-hide sidebar on mobile - let floating mode handle it
                                                                                                                                                                                                                                                                                };

                                                                                                                                                                                                                                                                                // Initial check
                                                                                                                                                                                                                                                                                checkMobile();

                                                                                                                                                                                                                                                                                // Add resize listener
                                                                                                                                                                                                                                                                                window.addEventListener("resize", checkMobile);
                                                                                                                                                                                                                                                                                return [&]() { return window.removeEventListener("resize", checkMobile); };
                                                                                                                                                                                                                                                                                }, [updateChatState]);

                                                                                                                                                                                                                                                                                if (
                                                                                                                                                                                                                                                                                chatType == ChannelType.DM &&;
                                                                                                                                                                                                                                                                                (isLoadingAgent || (!targetAgentData && contextId) || isLoadingAgentDmChannels);
                                                                                                                                                                                                                                                                                ) {
                                                                                                                                                                                                                                                                                    return (;
                                                                                                                                                                                                                                                                                    <div className="flex items-center justify-center h-full">;
                                                                                                                                                                                                                                                                                    <Loader2 className="h-8 w-8 animate-spin text-primary" />;
                                                                                                                                                                                                                                                                                    </div>;
                                                                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                                if (
                                                                                                                                                                                                                                                                                !finalChannelIdForHooks ||;
                                                                                                                                                                                                                                                                                !finalServerIdForHooks ||;
                                                                                                                                                                                                                                                                                (chatType == ChannelType.DM && !targetAgentData);
                                                                                                                                                                                                                                                                                ) {
                                                                                                                                                                                                                                                                                    return (;
                                                                                                                                                                                                                                                                                    <div className="flex flex-1 justify-center items-center">;
                                                                                                                                                                                                                                                                                    <p>Loading chat context...</p>;
                                                                                                                                                                                                                                                                                    </div>;
                                                                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                                // Chat header
                                                                                                                                                                                                                                                                                const auto renderChatHeader = [&]() {;
                                                                                                                                                                                                                                                                                    if (chatType == ChannelType.DM && targetAgentData) {
                                                                                                                                                                                                                                                                                        return (;
                                                                                                                                                                                                                                                                                        <div className="flex items-center justify-between mb-4 p-3 bg-card rounded-lg border">;
                                                                                                                                                                                                                                                                                        <div className="flex items-center gap-3 min-w-0 flex-1">;
                                                                                                                                                                                                                                                                                        <div className="relative flex-shrink-0">;
                                                                                                                                                                                                                                                                                        <Avatar className="size-4 sm:size-10 border rounded-full">
                                                                                                                                                                                                                                                                                        <AvatarImage src={getAgentAvatar(targetAgentData)} />;
                                                                                                                                                                                                                                                                                        </Avatar>;
                                                                                                                                                                                                                                                                                        {targetAgentData.status == AgentStatus.ACTIVE ? (;
                                                                                                                                                                                                                                                                                        <Tooltip>;
                                                                                                                                                                                                                                                                                        <TooltipTrigger asChild>;
                                                                                                                                                                                                                                                                                        <span className="absolute bottom-0 right-0 size-2 sm:size-[10px] rounded-full border border-white bg-green-500" />
                                                                                                                                                                                                                                                                                        </TooltipTrigger>;
                                                                                                                                                                                                                                                                                        <TooltipContent side="right">;
                                                                                                                                                                                                                                                                                        <p>Agent is active</p>;
                                                                                                                                                                                                                                                                                        </TooltipContent>;
                                                                                                                                                                                                                                                                                        </Tooltip>;
                                                                                                                                                                                                                                                                                        ) : (
                                                                                                                                                                                                                                                                                        <Tooltip>;
                                                                                                                                                                                                                                                                                        <TooltipTrigger asChild>;
                                                                                                                                                                                                                                                                                        <span className="absolute bottom-0 right-0 size-2 sm:size-[10px] rounded-full border border-white bg-muted-foreground" />
                                                                                                                                                                                                                                                                                        </TooltipTrigger>;
                                                                                                                                                                                                                                                                                        <TooltipContent side="right">;
                                                                                                                                                                                                                                                                                        <p>Agent is inactive</p>;
                                                                                                                                                                                                                                                                                        </TooltipContent>;
                                                                                                                                                                                                                                                                                        </Tooltip>;
                                                                                                                                                                                                                                                                                    )}
                                                                                                                                                                                                                                                                                    </div>;
                                                                                                                                                                                                                                                                                    <div className="flex flex-col min-w-0 flex-1">;
                                                                                                                                                                                                                                                                                    <div className="flex items-center gap-2">;
                                                                                                                                                                                                                                                                                    <h2 className="font-semibold text-lg truncate max-w-[80px] sm:max-w-none">
                                                                                                                                                                                                                                                                                {targetAgentData.name || "Agent"}
                                                                                                                                                                                                                                                                                </h2>;
                                                                                                                                                                                                                                                                                <Tooltip>;
                                                                                                                                                                                                                                                                                <TooltipTrigger asChild>;
                                                                                                                                                                                                                                                                                <Button;
                                                                                                                                                                                                                                                                                variant="ghost";
                                                                                                                                                                                                                                                                                size="sm";
                                                                                                                                                                                                                                                                                className="h-6 w-6 p-0 flex-shrink-0";
                                                                                                                                                                                                                                                                            onClick={() => updateChatState({ showProfileOverlay: true })}
                                                                                                                                                                                                                                                                            >;
                                                                                                                                                                                                                                                                            <Info className="size-4" />;
                                                                                                                                                                                                                                                                            </Button>;
                                                                                                                                                                                                                                                                            </TooltipTrigger>;
                                                                                                                                                                                                                                                                            <TooltipContent side="bottom">;
                                                                                                                                                                                                                                                                            <p>View agent profile</p>;
                                                                                                                                                                                                                                                                            </TooltipContent>;
                                                                                                                                                                                                                                                                            </Tooltip>;
                                                                                                                                                                                                                                                                            </div>;
                                                                                                                                                                                                                                                                            {targetAgentData.bio && (;
                                                                                                                                                                                                                                                                            <p className="text-sm text-muted-foreground line-clamp-1">;
                                                                                                                                                                                                                                                                            <span className="sm:hidden">
                                                                                                                                                                                                                                                                        {/* Mobile = Show only first 30 characters */}
                                                                                                                                                                                                                                                                        "{((text) => (text.size() > 30 ? " + std::to_string(text.substring(0, 30)) + "..."
                                                                                                                                                                                                                                                                        Array.isArray(targetAgentData.bio);
                                                                                                                                                                                                                                                                        ? targetAgentData.bio[0] || "";
                                                                                                                                                                                                                                                                        : targetAgentData.bio || ""
                                                                                                                                                                                                                                                                    )}
                                                                                                                                                                                                                                                                    </span>;
                                                                                                                                                                                                                                                                    <span className="hidden sm:inline">
                                                                                                                                                                                                                                                                {/* Desktop = Show full first bio entry or full bio */}
                                                                                                                                                                                                                                                                {Array.isArray(targetAgentData.bio);
                                                                                                                                                                                                                                                                ? targetAgentData.bio[0];
                                                                                                                                                                                                                                                            : targetAgentData.bio}
                                                                                                                                                                                                                                                            </span>;
                                                                                                                                                                                                                                                            </p>;
                                                                                                                                                                                                                                                        )}
                                                                                                                                                                                                                                                        </div>;
                                                                                                                                                                                                                                                        </div>;
                                                                                                                                                                                                                                                        <div className="flex gap-1 sm:gap-2 items-center flex-shrink-0">
                                                                                                                                                                                                                                                        {chatType == ChannelType.DM && (;
                                                                                                                                                                                                                                                        <div className="flex items-center gap-1">;
                                                                                                                                                                                                                                                        {agentDmChannels.size() > 0 && (;
                                                                                                                                                                                                                                                        <DropdownMenu>;
                                                                                                                                                                                                                                                        <DropdownMenuTrigger asChild>;
                                                                                                                                                                                                                                                        <Button;
                                                                                                                                                                                                                                                        variant="outline";
                                                                                                                                                                                                                                                        size="sm";
                                                                                                                                                                                                                                                        className="w-8 sm:max-w-[300px] sm:w-auto"
                                                                                                                                                                                                                                                        >;
                                                                                                                                                                                                                                                        <History className="size-4 flex-shrink-0" />;
                                                                                                                                                                                                                                                        <span className="hidden md:inline truncate text-xs sm:text-sm sm:ml-2">
                                                                                                                                                                                                                                                        {agentDmChannels.find((c) => c.id == chatState.currentDmChannelId);
                                                                                                                                                                                                                                                    .name || "Select Chat"}
                                                                                                                                                                                                                                                    </span>;
                                                                                                                                                                                                                                                    <Badge;
                                                                                                                                                                                                                                                    variant="secondary";
                                                                                                                                                                                                                                                    className="hidden md:inline-flex ml-1 sm:ml-2 text-xs"
                                                                                                                                                                                                                                                    >;
                                                                                                                                                                                                                                                {agentDmChannels.size()}
                                                                                                                                                                                                                                                </Badge>;
                                                                                                                                                                                                                                                </Button>;
                                                                                                                                                                                                                                                </DropdownMenuTrigger>;
                                                                                                                                                                                                                                                <DropdownMenuContent align="end" className="w-[280px] sm:w-[320px]">
                                                                                                                                                                                                                                                <DropdownMenuLabel className="font-medium">;
                                                                                                                                                                                                                                            Chat History with {targetAgentData.name}
                                                                                                                                                                                                                                            </DropdownMenuLabel>;
                                                                                                                                                                                                                                            <DropdownMenuSeparator />;
                                                                                                                                                                                                                                            <div className="max-h-[300px] overflow-y-auto">;
                                                                                                                                                                                                                                            {agentDmChannels.map((channel) => (;
                                                                                                                                                                                                                                            <DropdownMenuItem;
                                                                                                                                                                                                                                        key={channel.id}
                                                                                                                                                                                                                                    onClick={() => handleSelectDmRoom(channel.id)}
                                                                                                                                                                                                                                    className={cn(;
                                                                                                                                                                                                                                    "cursor-pointer",
                                                                                                                                                                                                                                    channel.id == chatState.currentDmChannelId && "bg-muted";
                                                                                                                                                                                                                                )}
                                                                                                                                                                                                                                >;
                                                                                                                                                                                                                                <div className="flex items-center justify-between w-full">;
                                                                                                                                                                                                                                <div className="flex flex-col min-w-0 flex-1">;
                                                                                                                                                                                                                                <span;
                                                                                                                                                                                                                                className={cn(;
                                                                                                                                                                                                                                "text-sm truncate",
                                                                                                                                                                                                                                channel.id == chatState.currentDmChannelId && "font-medium";
                                                                                                                                                                                                                            )}
                                                                                                                                                                                                                            >;
                                                                                                                                                                                                                        {channel.name}
                                                                                                                                                                                                                        </span>;
                                                                                                                                                                                                                        <span className="text-xs text-muted-foreground">;
                                                                                                                                                                                                                        {moment(;
                                                                                                                                                                                                                        channel.metadata.createdAt ||;
                                                                                                                                                                                                                        channel.updatedAt ||;
                                                                                                                                                                                                                        channel.createdAt;
                                                                                                                                                                                                                    ).fromNow()}
                                                                                                                                                                                                                    </span>;
                                                                                                                                                                                                                    </div>;
                                                                                                                                                                                                                    {channel.id == chatState.currentDmChannelId && (;
                                                                                                                                                                                                                    <Badge variant="default" className="text-xs flex-shrink-0 ml-2">;
                                                                                                                                                                                                                    Current;
                                                                                                                                                                                                                    </Badge>;
                                                                                                                                                                                                                )}
                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                </DropdownMenuItem>;
                                                                                                                                                                                                            ))}
                                                                                                                                                                                                            </div>;
                                                                                                                                                                                                            </DropdownMenuContent>;
                                                                                                                                                                                                            </DropdownMenu>;
                                                                                                                                                                                                        )}

                                                                                                                                                                                                    {/* Chat Actions Split Button */}
                                                                                                                                                                                                    <SplitButton;
                                                                                                                                                                                                    mainAction={{
                                                                                                                                                                                                        label: chatState.isCreatingDM ? (
                                                                                                                                                                                                        "Creating...";
                                                                                                                                                                                                        ) : (
                                                                                                                                                                                                        <>;
                                                                                                                                                                                                        <span className="sm:hidden">New</span>
                                                                                                                                                                                                        <span className="hidden sm:inline">New Chat</span>
                                                                                                                                                                                                        </>;
                                                                                                                                                                                                        ),
                                                                                                                                                                                                        onClick: () => handleNewDmChannel(targetAgentData.id),
                                                                                                                                                                                                        icon: chatState.isCreatingDM ? (
                                                                                                                                                                                                        <Loader2 className="size-4 animate-spin" />;
                                                                                                                                                                                                        ) : (
                                                                                                                                                                                                        <Plus className="size-4" />;
                                                                                                                                                                                                        ),
                                                                                                                                                                                                        disabled: chatState.isCreatingDM || isLoadingAgentDmChannels,
                                                                                                                                                                                                    }}
                                                                                                                                                                                                    actions={[;
                                                                                                                                                                                                    {
                                                                                                                                                                                                        label: "Clear Messages",
                                                                                                                                                                                                        onClick: handleClearChat,
                                                                                                                                                                                                        icon: <Eraser className="size-4" />,
                                                                                                                                                                                                        disabled: !messages || messages.size() == 0,
                                                                                                                                                                                                        },
                                                                                                                                                                                                        {
                                                                                                                                                                                                            label: "Delete Chat",
                                                                                                                                                                                                            onClick: handleDeleteCurrentDmChannel,
                                                                                                                                                                                                            icon: <Trash2 className="size-4" />,
                                                                                                                                                                                                            disabled: !chatState.currentDmChannelId,
                                                                                                                                                                                                            variant: "destructive",
                                                                                                                                                                                                            },
                                                                                                                                                                                                        ]}
                                                                                                                                                                                                        variant="outline";
                                                                                                                                                                                                        size="sm";
                                                                                                                                                                                                        className="px-2 sm:px-3"
                                                                                                                                                                                                        />;
                                                                                                                                                                                                        </div>;
                                                                                                                                                                                                    )}

                                                                                                                                                                                                    <Separator orientation="vertical" className="h-8 hidden sm:block" />
                                                                                                                                                                                                    <Tooltip>;
                                                                                                                                                                                                    <TooltipTrigger asChild>;
                                                                                                                                                                                                    <Button;
                                                                                                                                                                                                    variant="ghost";
                                                                                                                                                                                                    size="sm";
                                                                                                                                                                                                    className="px-2 sm:px-3 h-8 w-8 sm:w-auto ml-1 sm:ml-3"
                                                                                                                                                                                                onClick={toggleSidebar}
                                                                                                                                                                                                >;
                                                                                                                                                                                                {showSidebar ? (;
                                                                                                                                                                                                <PanelRightClose className="h-4 w-4" />;
                                                                                                                                                                                                ) : (
                                                                                                                                                                                                <PanelRight className="h-4 w-4" />;
                                                                                                                                                                                            )}
                                                                                                                                                                                            </Button>;
                                                                                                                                                                                            </TooltipTrigger>;
                                                                                                                                                                                            <TooltipContent side="bottom">;
                                                                                                                                                                                            <p>{showSidebar ? "Close SidePanel" : "Open SidePanel"}</p>
                                                                                                                                                                                            </TooltipContent>;
                                                                                                                                                                                            </Tooltip>;
                                                                                                                                                                                            </div>;
                                                                                                                                                                                            </div>;
                                                                                                                                                                                            );
                                                                                                                                                                                            } else if (chatType == ChannelType.GROUP) {
                                                                                                                                                                                                const auto groupDisplayName = generateGroupName(;
                                                                                                                                                                                                channelDetailsData.data || std::nullopt,
                                                                                                                                                                                                groupAgents,
                                                                                                                                                                                                currentClientEntityId;
                                                                                                                                                                                                );

                                                                                                                                                                                                return (;
                                                                                                                                                                                                <div className="flex flex-col gap-3 mb-4">;
                                                                                                                                                                                                <div className="flex items-center justify-between p-3 bg-card rounded-lg border">;
                                                                                                                                                                                                <div className="flex items-center gap-3 min-w-0 flex-1">;
                                                                                                                                                                                                <h2 className="font-semibold text-lg truncate" title={groupDisplayName}>;
                                                                                                                                                                                            {groupDisplayName}
                                                                                                                                                                                            </h2>;
                                                                                                                                                                                            </div>;
                                                                                                                                                                                            <div className="flex gap-1 sm:gap-2 items-center flex-shrink-0">
                                                                                                                                                                                        {/* Group Actions Split Button */}
                                                                                                                                                                                        <SplitButton;
                                                                                                                                                                                        mainAction={{
                                                                                                                                                                                            label: "Edit Group",
                                                                                                                                                                                            onClick: () => updateChatState({ showGroupEditPanel: true }),
                                                                                                                                                                                            icon: <Edit className="size-4" />,
                                                                                                                                                                                        }}
                                                                                                                                                                                        actions={[;
                                                                                                                                                                                        {
                                                                                                                                                                                            label: "Clear Messages",
                                                                                                                                                                                            onClick: handleClearChat,
                                                                                                                                                                                            icon: <Eraser className="size-4" />,
                                                                                                                                                                                            disabled: !messages || messages.size() == 0,
                                                                                                                                                                                            },
                                                                                                                                                                                            {
                                                                                                                                                                                                label: "Delete Group",
                                                                                                                                                                                                onClick: () => {
                                                                                                                                                                                                    if (!finalChannelIdForHooks || !finalServerIdForHooks) return;
                                                                                                                                                                                                    confirm(;
                                                                                                                                                                                                    {
                                                                                                                                                                                                        title: "Delete Group",
                                                                                                                                                                                                        description:
                                                                                                                                                                                                        "Are you sure you want to delete this group? This action cannot be undone.",
                                                                                                                                                                                                        confirmText: "Delete",
                                                                                                                                                                                                        variant: "destructive",
                                                                                                                                                                                                        },
                                                                                                                                                                                                        async () => {
                                                                                                                                                                                                            try {
                                                                                                                                                                                                                const auto elizaClient = createElizaClient();
                                                                                                                                                                                                                elizaClient.messaging.deleteChannel(finalChannelIdForHooks);
                                                                                                                                                                                                                toast({
                                                                                                                                                                                                                    title: "Group Deleted",
                                                                                                                                                                                                                    description: "The group has been successfully deleted.",
                                                                                                                                                                                                                    });
                                                                                                                                                                                                                    // Navigate back to home after deletion
                                                                                                                                                                                                                    window.location.href = "/";
                                                                                                                                                                                                                    } catch (error) {
                                                                                                                                                                                                                        clientLogger.error("[Chat] Error deleting group:", error);
                                                                                                                                                                                                                        toast({
                                                                                                                                                                                                                            title: "Error",
                                                                                                                                                                                                                            description: "Could not delete group.",
                                                                                                                                                                                                                            variant: "destructive",
                                                                                                                                                                                                                            });
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    );
                                                                                                                                                                                                                    },
                                                                                                                                                                                                                    icon: <Trash2 className="size-4" />,
                                                                                                                                                                                                                    disabled: !finalChannelIdForHooks || !finalServerIdForHooks,
                                                                                                                                                                                                                    variant: "destructive",
                                                                                                                                                                                                                    },
                                                                                                                                                                                                                ]}
                                                                                                                                                                                                                variant="outline";
                                                                                                                                                                                                                size="sm";
                                                                                                                                                                                                                className="px-2 sm:px-3"
                                                                                                                                                                                                                />;
                                                                                                                                                                                                                <Button;
                                                                                                                                                                                                                variant="ghost";
                                                                                                                                                                                                                size="sm";
                                                                                                                                                                                                                className="px-2 sm:px-3 h-8 w-8 sm:w-auto"
                                                                                                                                                                                                            onClick={toggleSidebar}
                                                                                                                                                                                                            >;
                                                                                                                                                                                                            {showSidebar ? (;
                                                                                                                                                                                                            <PanelRightClose className="h-4 w-4" />;
                                                                                                                                                                                                            ) : (
                                                                                                                                                                                                            <PanelRight className="h-4 w-4" />;
                                                                                                                                                                                                        )}
                                                                                                                                                                                                        </Button>;
                                                                                                                                                                                                        </div>;
                                                                                                                                                                                                        </div>;

                                                                                                                                                                                                        {groupAgents.size() > 0 && (;
                                                                                                                                                                                                        <div className="flex items-center gap-2 p-2 bg-card rounded-lg border overflow-x-auto">;
                                                                                                                                                                                                        <span className="text-sm text-muted-foreground whitespace-nowrap flex-shrink-0">;
                                                                                                                                                                                                        Agents:
                                                                                                                                                                                                        </span>;
                                                                                                                                                                                                        <div className="flex gap-2 min-w-0">;
                                                                                                                                                                                                        <Button;
                                                                                                                                                                                                    variant={!chatState.selectedGroupAgentId ? "default" : "ghost"}
                                                                                                                                                                                                    size="sm";
                                                                                                                                                                                                onClick={() => updateChatState({ selectedGroupAgentId: nullptr })}
                                                                                                                                                                                                className="flex items-center gap-2 flex-shrink-0";
                                                                                                                                                                                                >;
                                                                                                                                                                                                <span>All</span>;
                                                                                                                                                                                                </Button>;
                                                                                                                                                                                                {groupAgents.map((agent) => (;
                                                                                                                                                                                                <Button;
                                                                                                                                                                                            key={agent.id}
                                                                                                                                                                                        variant={chatState.selectedGroupAgentId == agent.id ? "default" : "ghost"}
                                                                                                                                                                                        size="sm";
                                                                                                                                                                                        onClick={() => {
                                                                                                                                                                                            updateChatState({
                                                                                                                                                                                                selectedGroupAgentId: agent.id || nullptr,
                                                                                                                                                                                                });
                                                                                                                                                                                                if (agent.id && !showSidebar) {
                                                                                                                                                                                                    setSidebarVisible(true);
                                                                                                                                                                                                }
                                                                                                                                                                                            }}
                                                                                                                                                                                            className="flex items-center gap-2 flex-shrink-0";
                                                                                                                                                                                            >;
                                                                                                                                                                                            <Avatar className="size-5">;
                                                                                                                                                                                            <AvatarImage src={getAgentAvatar(agent)} />;
                                                                                                                                                                                            </Avatar>;
                                                                                                                                                                                            <span className="truncate max-w-[100px] sm:max-w-none">{agent.name}</span>
                                                                                                                                                                                            </Button>;
                                                                                                                                                                                        ))}
                                                                                                                                                                                        </div>;
                                                                                                                                                                                        </div>;
                                                                                                                                                                                    )}
                                                                                                                                                                                    </div>;
                                                                                                                                                                                    );
                                                                                                                                                                                }
                                                                                                                                                                                return nullptr;
                                                                                                                                                                                };

                                                                                                                                                                                return (;
                                                                                                                                                                                <>;
                                                                                                                                                                                <div className="h-full flex flex-col relative overflow-hidden">;
                                                                                                                                                                            {/* Conditional layout based on floating mode */}
                                                                                                                                                                            {isFloatingMode ? (;
                                                                                                                                                                            /* Single panel layout for floating mode */
                                                                                                                                                                            <div className="h-full flex flex-col overflow-hidden">;
                                                                                                                                                                            <div className="flex-shrink-0 p-2 sm:p-4 pb-0">{renderChatHeader()}</div>

                                                                                                                                                                            <div;
                                                                                                                                                                            className={cn(;
                                                                                                                                                                            "flex flex-col transition-all duration-300 w-full flex-1 min-h-0 overflow-hidden p-2 sm:p-4 pt-0"
                                                                                                                                                                        )}
                                                                                                                                                                        >;
                                                                                                                                                                        <div className="flex-1 min-h-0 overflow-hidden">;
                                                                                                                                                                        <ChatMessageListComponent;
                                                                                                                                                                    messages={messages}
                                                                                                                                                                isLoadingMessages={isLoadingMessages}
                                                                                                                                                            chatType={chatType}
                                                                                                                                                        currentClientEntityId={currentClientEntityId}
                                                                                                                                                    targetAgentData={targetAgentData}
                                                                                                                                                allAgents={allAgents}
                                                                                                                                            animatedMessageId={animatedMessageId}
                                                                                                                                        scrollRef={scrollRef}
                                                                                                                                    contentRef={contentRef}
                                                                                                                                isAtBottom={isAtBottom}
                                                                                                                            scrollToBottom={scrollToBottom}
                                                                                                                        disableAutoScroll={disableAutoScroll}
                                                                                                                    finalChannelId={finalChannelIdForHooks}
                                                                                                                getAgentInMessage={getAgentInMessage}
                                                                                                            agentAvatarMap={agentAvatarMap}
                                                                                                        onDeleteMessage={handleDeleteMessage}
                                                                                                    onRetryMessage={handleRetryMessage}
                                                                                                selectedGroupAgentId={chatState.selectedGroupAgentId}
                                                                                                />;
                                                                                                </div>;

                                                                                                <div className="flex-shrink-0">;
                                                                                                <ChatInputArea;
                                                                                            input={chatState.input}
                                                                                        setInput={(value) => updateChatState({ input: value })}
                                                                                    inputDisabled={chatState.inputDisabled}
                                                                                selectedFiles={selectedFiles}
                                                                            removeFile={removeFile}
                                                                        handleFileChange={handleFileChange}
                                                                    handleSendMessage={handleSendMessage}
                                                                handleKeyDown={handleKeyDown}
                                                            chatType={chatType}
                                                        targetAgentData={targetAgentData}
                                                    formRef={formRef}
                                                inputRef={inputRef}
                                            fileInputRef={fileInputRef}
                                            />;
                                            </div>;
                                            </div>;
                                            </div>;
                                            ) : (
                                            /* Resizable panel layout for desktop mode */
                                            <ResizablePanelGroup;
                                            direction="horizontal";
                                            className="h-full flex-1 overflow-hidden";
                                            onLayout={(sizes) => {
                                                if (sizes.length >= 2 && showSidebar && !chatState.isMobile) {
                                                    setMainPanelSize(sizes[0]);
                                                    setSidebarPanelSize(sizes[1]);
                                                }
                                            }}
                                            >;
                                            <ResizablePanel;
                                        defaultSize={showSidebar && !chatState.isMobile ? mainPanelSize : 100}
                                    minSize={chatState.isMobile ? 100 : 50}
                                    >;
                                    <div className="relative h-full overflow-hidden">;
                                {/* Main chat content */}
                                <div className="h-full flex flex-col overflow-hidden">;
                                <div className="flex-shrink-0 p-2 sm:p-4 pb-0">{renderChatHeader()}</div>

                                <div;
                                className={cn(;
                                "flex flex-col transition-all duration-300 w-full flex-1 min-h-0 overflow-hidden p-2 sm:p-4 pt-0"
                            )}
                            >;
                            <div className="flex-1 min-h-0 overflow-hidden">;
                            <ChatMessageListComponent;
                        messages={messages}
                    isLoadingMessages={isLoadingMessages}
                chatType={chatType}
            currentClientEntityId={currentClientEntityId}
        targetAgentData={targetAgentData}
    allAgents={allAgents}
    animatedMessageId={animatedMessageId}
    scrollRef={scrollRef}
    contentRef={contentRef}
    isAtBottom={isAtBottom}
    scrollToBottom={scrollToBottom}
    disableAutoScroll={disableAutoScroll}
    finalChannelId={finalChannelIdForHooks}
    getAgentInMessage={getAgentInMessage}
    agentAvatarMap={agentAvatarMap}
    onDeleteMessage={handleDeleteMessage}
    onRetryMessage={handleRetryMessage}
    selectedGroupAgentId={chatState.selectedGroupAgentId}
    />;
    </div>;

    <div className="flex-shrink-0">;
    <ChatInputArea;
    input={chatState.input}
    setInput={(value) => updateChatState({ input: value })}
    inputDisabled={chatState.inputDisabled}
    selectedFiles={selectedFiles}
    removeFile={removeFile}
    handleFileChange={handleFileChange}
    handleSendMessage={handleSendMessage}
    handleKeyDown={handleKeyDown}
    chatType={chatType}
    targetAgentData={targetAgentData}
    formRef={formRef}
    inputRef={inputRef}
    fileInputRef={fileInputRef}
    />;
    </div>;
    </div>;
    </div>;
    </div>;
    </ResizablePanel>;

    {/* Right panel / sidebar */}
    {(() => {
        UUID sidebarAgentId = std::nullopt;
        std::string sidebarAgentName = "Agent";
        UUID sidebarChannelId = std::nullopt;

        if (chatType == ChannelType.DM) {
            sidebarAgentId = contextId; // This is agentId for DM;
            sidebarAgentName = targetAgentData.name || "Agent";
            sidebarChannelId = chatState.currentDmChannelId || std::nullopt;
            } else if (chatType == ChannelType.GROUP && chatState.selectedGroupAgentId) {
                sidebarAgentId = chatState.selectedGroupAgentId;
                const auto selectedAgent = allAgents.find(;
                [&](a) { return a.id == chatState.selectedGroupAgentId; }
                );
                sidebarAgentName = selectedAgent.name || "Group Member";
                sidebarChannelId = contextId; // contextId is the channelId for GROUP;
                } else if (chatType == ChannelType.GROUP && !chatState.selectedGroupAgentId) {
                    sidebarAgentName = "Group";
                    sidebarChannelId = contextId; // contextId is the channelId for GROUP;
                }

                return (;
                showSidebar &&;
                !chatState.isMobile && (;
                <>;
                <ResizableHandle withHandle />;
                <ResizablePanel defaultSize={sidebarPanelSize} minSize={20} maxSize={50}>;
                <AgentSidebar;
            agentId={sidebarAgentId}
        agentName={sidebarAgentName}
    channelId={sidebarChannelId}
    />;
    </ResizablePanel>;
    </>;
    );
    );
    })()}
    </ResizablePanelGroup>;
    )}

    {/* Floating sidebar overlay for narrow screens */}
    {(() => {
        UUID sidebarAgentId = std::nullopt;
        std::string sidebarAgentName = "Agent";
        UUID sidebarChannelId = std::nullopt;

        if (chatType == ChannelType.DM) {
            sidebarAgentId = contextId; // This is agentId for DM;
            sidebarAgentName = targetAgentData.name || "Agent";
            sidebarChannelId = chatState.currentDmChannelId || std::nullopt;
            } else if (chatType == ChannelType.GROUP && chatState.selectedGroupAgentId) {
                sidebarAgentId = chatState.selectedGroupAgentId;
                const auto selectedAgent = allAgents.find((a) => a.id == chatState.selectedGroupAgentId);
                sidebarAgentName = selectedAgent.name || "Group Member";
                sidebarChannelId = contextId; // contextId is the channelId for GROUP;
                } else if (chatType == ChannelType.GROUP && !chatState.selectedGroupAgentId) {
                    sidebarAgentName = "Group";
                    sidebarChannelId = contextId; // contextId is the channelId for GROUP;
                }

                return (;
                showSidebar &&;
                isFloatingMode && (;
                <div className="absolute inset-0 z-50 bg-background/80 backdrop-blur-sm">;
                <div className="absolute inset-0 bg-background shadow-lg">;
                <div className="h-full flex flex-col">;
            {/* Close button for floating sidebar */}
            <div className="flex items-center justify-between p-4 border-b">;
            <h3 className="font-semibold text-lg">{sidebarAgentName}</h3>;
            <Button;
            variant="ghost";
            size="sm";
        onClick={() => setSidebarVisible(false)}
        className="h-8 w-8 p-0";
        >;
        <PanelRightClose className="h-4 w-4" />;
        </Button>;
        </div>;
        <div className="flex-1 overflow-hidden">;
        <AgentSidebar;
    agentId={sidebarAgentId}
    agentName={sidebarAgentName}
    channelId={sidebarChannelId}
    />;
    </div>;
    </div>;
    </div>;
    </div>;
    );
    );
    })()}
    </div>;

    {chatState.showGroupEditPanel && chatType == ChannelType.GROUP && (;
    <GroupPanel;
    onClose={() => updateChatState({ showGroupEditPanel: false })}
    channelId={contextId}
    />;
    )}

    {chatState.showProfileOverlay && chatType == ChannelType.DM && targetAgentData.id && (;
    <ProfileOverlay;
    isOpen={chatState.showProfileOverlay}
    onClose={() => updateChatState({ showProfileOverlay: false })}
    agentId={targetAgentData.id}
    />;
    )}

    {/* Confirmation Dialogs */}
    <ConfirmationDialog;
    open={isOpen}
    onOpenChange={onOpenChange}
    title={options.title || ""}
    description={options.description || ""}
    confirmText={options.confirmText}
    cancelText={options.cancelText}
    variant={options.variant}
    onConfirm={onConfirm}
    />;
    </>;
    );

}

} // namespace elizaos
