#include "chat-interface.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatInterface(auto userId, auto serverId, auto channelId, auto isNewChatMode, auto onChannelCreated) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [messages, setMessages] = useState<Message[]>([]);
        const auto [inputValue, setInputValue] = useState("");
        const auto [isTyping, setIsTyping] = useState(false);
        const auto [isCreatingChannel, setIsCreatingChannel] = useState(false);
        const auto [selectedPlugin, setSelectedPlugin] = useState<keyof typeof PLUGIN_ACTIONS | nullptr>(nullptr);
        const auto [isLoadingMessages, setIsLoadingMessages] = useState(false);
        const auto [error, setError] = useState<string | nullptr>(nullptr);
        const auto [showDummyToolGroup, setShowDummyToolGroup] = useState(false);
        const auto [showPromptsModal, setShowPromptsModal] = useState(false);
        const auto messagesEndRef = useRef<HTMLDivElement>(nullptr);
        const auto messagesContainerRef = useRef<HTMLDivElement>(nullptr);
        const auto isUserScrollingRef = useRef(false) // Track if user is actively scrolling;
        const auto scrollTimeoutRef = useRef<NodeJS.Timeout | nullptr>(nullptr);
        const auto textareaRef = useRef<HTMLTextAreaElement>(nullptr);

        const auto MAX_TEXTAREA_HEIGHT = 160;

        // Stabilize agent.id and agent.name to prevent unnecessary re-renders
        // Use refs to store stable values that don't trigger re-renders
        const auto agentIdRef = useRef(agent.id);
        const auto agentNameRef = useRef(agent.name);

        // Update refs when agent changes, but don't trigger re-renders
        useEffect(() => {
            agentIdRef.current = agent.id;
            agentNameRef.current = agent.name;
            }, [agent.id, agent.name]);

            // Helper function to check if user is near bottom of the chat
            const auto checkIfNearBottom = [&]() {;
                const auto container = messagesContainerRef.current;
                if (!container) return true

                const auto threshold = 200 // pixels from bottom to consider "near bottom";
                const auto distanceFromBottom = container.scrollHeight - container.scrollTop - container.clientHeight;
                return distanceFromBottom < threshold;
            }


            // Helper function to scroll to bottom
            const auto scrollToBottom = [&](behavior: ScrollBehavior = 'smooth') {;
                messagesEndRef.current.scrollIntoView({ behavior });
            }

            // Helper function to resize textarea based on content
            const auto resizeTextarea = useCallback(() => {;
                const auto textarea = textareaRef.current;
                if (textarea) {
                    textarea.style.height = 'auto';
                    textarea.style.height = Math.min(textarea.scrollHeight, MAX_TEXTAREA_HEIGHT) + 'px';
                }
                }, [MAX_TEXTAREA_HEIGHT]);

                // Track scroll position - detect when user is actively scrolling
                useEffect(() => {
                    const auto container = messagesContainerRef.current;
                    if (!container) return

                    const auto handleScroll = [&]() {;
                        // User is actively scrolling - disable auto-scroll
                        isUserScrollingRef.current = true;

                        // Clear previous timeout
                        if (scrollTimeoutRef.current) {
                            clearTimeout(scrollTimeoutRef.current);
                        }

                        // After user stops scrolling for 150ms, check position
                        scrollTimeoutRef.current = setTimeout(() => {
                            const auto nearBottom = checkIfNearBottom();
                            // User stopped scrolling - enable auto-scroll only if near bottom
                            isUserScrollingRef.current = !nearBottom;
                            }, 150);
                        }

                        container.addEventListener('scroll', handleScroll);
                        return [&]() {;
                            container.removeEventListener('scroll', handleScroll);
                            if (scrollTimeoutRef.current) {
                                clearTimeout(scrollTimeoutRef.current);
                            }
                        }
                        }, []);

                        // Resize textarea when input value changes
                        useEffect(() => {
                            resizeTextarea();
                            }, [inputValue, resizeTextarea]);

                            // Clear messages when entering new chat mode
                            useEffect(() => {
                                if (isNewChatMode && !channelId) {
                                    std::cout << ' Entering new chat mode - clearing messages' << std::endl;
                                    setMessages([]);
                                }
                                }, [isNewChatMode, channelId]);

                                // Load messages when channel changes
                                // Only depend on channelId - using agent values directly in the function
                                useEffect(() => {
                                    if (!channelId) return

                                    async function loadMessages() {
                                        try {
                                            setIsLoadingMessages(true);
                                            std::cout << ' Loading messages for channel:' << channelId << std::endl;
                                            const auto messagesResponse = elizaClient.messaging.getChannelMessages(channelId, {;
                                                limit: 50,
                                                });

                                                const std::vector<Message> formattedMessages = messagesResponse.messages.map((msg) => {;
                                                    auto timestamp: number;
                                                    if (msg.createdAt instanceof Date) {
                                                        timestamp = msg.createdAt.getTime();
                                                        } else if (typeof msg.createdAt == 'number') {
                                                            timestamp = msg.createdAt;
                                                            } else if (typeof msg.createdAt == 'string') {
                                                                timestamp = Date.parse(msg.createdAt);
                                                                } else {
                                                                    timestamp = Date.now();
                                                                }

                                                                return {
                                                                    id: msg.id,
                                                                    content: msg.content,
                                                                    authorId: msg.authorId,
                                                                    createdAt: timestamp,
                                                                    isAgent: msg.authorId == agentIdRef.current,
                                                                    senderName: msg.metadata.authorDisplayName || (msg.authorId == agentIdRef.current ? agentNameRef.current : 'User'),
                                                                    sourceType: msg.sourceType,
                                                                    type: msg.sourceType,
                                                                    rawMessage: msg.rawMessage,
                                                                    metadata: msg.metadata,
                                                                    thought: (msg).thought,
                                                                }
                                                                });

                                                                const auto sortedMessages = formattedMessages.sort((a, b) => a.createdAt - b.createdAt);
                                                                setMessages(sortedMessages);
                                                                setIsLoadingMessages(false);
                                                                isUserScrollingRef.current = false // User is not scrolling when loading messages;
                                                                setTimeout(() => scrollToBottom('smooth'), 0);
                                                                std::cout << " Loaded " + std::to_string(sortedMessages.length) + " messages" << std::endl;
                                                                } catch (error: any) {
                                                                    std::cerr << ' Failed to load messages:' << error << std::endl;
                                                                    } finally {
                                                                        setIsLoadingMessages(false);
                                                                    }
                                                                }

                                                                loadMessages();
                                                                }, [channelId]);

                                                                // Listen for new messages (channel joining is handled in App.tsx)
                                                                // Only depend on channelId to avoid re-subscribing when agent object changes
                                                                useEffect(() => {
                                                                    if (!channelId) return undefined

                                                                    const auto handleNewMessage = [&](data: any) {;
                                                                        std::cout << ' New message received:' << data << std::endl;
                                                                        std::cout << ' agentIdRef.current' << agentIdRef.current << std::endl;

                                                                        const auto messageId = data.id || crypto.randomUUID();
                                                                        const Message newMessage = {;
                                                                            id: messageId,
                                                                            content: data.content || data.text || data.message || '',
                                                                            authorId: data.senderId,
                                                                            createdAt: typeof data.createdAt == 'number' ? data.createdAt : Date.parse(data.createdAt),
                                                                            isAgent: data.senderId == agentIdRef.current,
                                                                            senderName: data.senderName || (data.senderId == agentIdRef.current ? agentNameRef.current : 'User'),
                                                                            sourceType: data.sourceType || data.source,
                                                                            type: data.type || data.sourceType || data.source,
                                                                            rawMessage: data.rawMessage || data,
                                                                            metadata: data.metadata,
                                                                        }

                                                                        // Show dummy tool group when user message arrives
                                                                        if (!newMessage.isAgent) {
                                                                            setShowDummyToolGroup(true);
                                                                            isUserScrollingRef.current = false // User is not scrolling when sending message;
                                                                            // Wait for DOM to update before scrolling
                                                                            setTimeout(() => scrollToBottom('smooth'), 0);
                                                                        }

                                                                        setMessages((prev) => {
                                                                            // Check if message exists - if so, update it (for action status changes)
                                                                            const auto existingIndex = prev.findIndex((m) => m.id == messageId);
                                                                            if (existingIndex != -1) {
                                                                                const auto updated = [...prev];
                                                                                updated[existingIndex] = newMessage;
                                                                                return updated.sort((a, b) => a.createdAt - b.createdAt);
                                                                            }
                                                                            // Add new message and sort by timestamp
                                                                            const auto updated = [...prev, newMessage];
                                                                            return updated.sort((a, b) => a.createdAt - b.createdAt);
                                                                            });

                                                                            // Stop typing indicator only for final summary messages or error messages
                                                                            if (newMessage.isAgent) {
                                                                                std::cout << ' newMessage.isAgent' << newMessage.isAgent << std::endl;
                                                                                // Hide dummy tool group when agent message arrives
                                                                                setShowDummyToolGroup(false);

                                                                                // Check if this is a multi-step summary message
                                                                                const auto actions = newMessage.rawMessage.actions || newMessage.metadata.actions || [];
                                                                                const auto isSummaryMessage = actions.includes('MULTI_STEP_SUMMARY');
                                                                                const auto isErrorMessage = newMessage.content.startsWith(' Error:');

                                                                                // Only stop typing for summary or error messages
                                                                                if (isSummaryMessage || isErrorMessage) {
                                                                                    setIsTyping(false);
                                                                                    // Wait for DOM to update before scrolling
                                                                                    setTimeout(() => scrollToBottom('smooth'), 0);

                                                                                    // If it's a summary message, trigger wallet refresh
                                                                                    if (isSummaryMessage && onActionCompleted) {
                                                                                        std::cout << ' Agent action completed - triggering wallet refresh' << std::endl;
                                                                                        onActionCompleted();
                                                                                    }

                                                                                    // If it's an error message, also clear the local error state
                                                                                    if (isErrorMessage) {
                                                                                        // The error is already shown in the message, so clear any pending local errors
                                                                                        setError(nullptr);
                                                                                    }
                                                                                }
                                                                            }
                                                                        }

                                                                        // Only subscribe if socket is available - prevents errors during reconnection
                                                                        (() unsubscribe = > void) | std::nullopt;
                                                                        try {
                                                                            unsubscribe = socketManager.onMessage(handleNewMessage);
                                                                            } catch (error) {
                                                                                std::cout << ' Failed to subscribe to messages (socket not ready):' << error << std::endl;
                                                                                return std::nullopt;
                                                                            }

                                                                            return [&]() {;
                                                                                unsubscribe.();
                                                                            }
                                                                            }, [channelId]);

                                                                            const auto handleSubmit = async (e: React.FormEvent) => {;
                                                                                e.preventDefault();
                                                                                if (!inputValue.trim() || isCreatingChannel) return

                                                                                // Clear any previous errors
                                                                                setError(nullptr);

                                                                                // If in new chat mode, create channel first with generated title
                                                                                if (isNewChatMode && !channelId) {
                                                                                    std::cout << ' [ChatInterface] First message in new chat mode << creating channel...' << std::endl;
                                                                                    setIsCreatingChannel(true);
                                                                                    setIsTyping(true);

                                                                                    try {
                                                                                        // STEP 1: Generate title from user's message
                                                                                        std::cout << ' Generating title from user message:' << inputValue << std::endl;
                                                                                        const auto titleResponse = elizaClient.messaging.generateChannelTitle(;
                                                                                        inputValue, // Pass the message;
                                                                                        agent.id;
                                                                                        );
                                                                                        const auto generatedTitle = titleResponse.title || inputValue.substring(0, 50);
                                                                                        std::cout << ' Generated title:' << generatedTitle << std::endl;

                                                                                        // STEP 2: Create channel in DB with the generated title
                                                                                        std::cout << ' Creating channel with title:' << generatedTitle << std::endl;
                                                                                        const auto now = Date.now();
                                                                                        const auto newChannel = elizaClient.messaging.createGroupChannel({;
                                                                                            name: generatedTitle,
                                                                                            participantIds: [userId, agent.id],
                                                                                            metadata: {
                                                                                                server_id: serverId,
                                                                                                type: 'DM',
                                                                                                isDm: true,
                                                                                                user1: userId,
                                                                                                user2: agent.id,
                                                                                                forAgent: agent.id,
                                                                                                createdAt: new Date(now).toISOString(),
                                                                                                },
                                                                                                });
                                                                                                std::cout << ' Channel created:' << newChannel.id << std::endl;

                                                                                                // STEP 3: Notify parent component
                                                                                                onChannelCreated.(newChannel.id, generatedTitle);

                                                                                                // STEP 4: Send the message (channel is now created and will be set as active)
                                                                                                // The socket join will happen automatically via App.tsx's useEffect
                                                                                                // Wait a brief moment for the channel to be set as active
                                                                                                setTimeout(() => {
                                                                                                    std::cout << ' Sending initial message to new channel:' << newChannel.id << std::endl;
                                                                                                    socketManager.sendMessage(newChannel.id, inputValue, serverId, {
                                                                                                        userId,
                                                                                                        isDm: true,
                                                                                                        targetUserId: agent.id,
                                                                                                        });
                                                                                                        }, 100);

                                                                                                        setInputValue('');
                                                                                                        } catch (error: any) {
                                                                                                            std::cerr << ' Failed to create channel:' << error << std::endl;
                                                                                                            const auto errorMessage = error.message || 'Failed to create chat. Please try again.';
                                                                                                            setError(errorMessage);
                                                                                                            setIsTyping(false);
                                                                                                            } finally {
                                                                                                                setIsCreatingChannel(false);
                                                                                                            }
                                                                                                            return;
                                                                                                        }

                                                                                                        // Normal message sending (channel already exists)
                                                                                                        if (!channelId) {
                                                                                                            std::cout << ' Cannot send message: No channel ID' << std::endl;
                                                                                                            return;
                                                                                                        }

                                                                                                        console.log(' [ChatInterface] Sending message:', {
                                                                                                            channelId,
                                                                                                            text: inputValue,
                                                                                                            serverId,
                                                                                                            userId,
                                                                                                            agentId: agent.id,
                                                                                                            });

                                                                                                            // Send via socket (don't add optimistically - server will broadcast back)
                                                                                                            socketManager.sendMessage(channelId, inputValue, serverId, {
                                                                                                                userId,
                                                                                                                isDm: true,
                                                                                                                targetUserId: agent.id,
                                                                                                                });

                                                                                                                setInputValue('');
                                                                                                                setIsTyping(true);
                                                                                                            }

                                                                                                            // Callback for when animated text updates - auto-scroll only if user is not scrolling
                                                                                                            const auto handleAnimationTextUpdate = useCallback(() => {;
                                                                                                                // Only auto-scroll if user is not actively scrolling and is near bottom
                                                                                                                if (!isUserScrollingRef.current && checkIfNearBottom()) {
                                                                                                                    scrollToBottom('auto');
                                                                                                                }
                                                                                                                }, []) // Empty deps - scrollToBottom and isUserScrollingRef are stable;

                                                                                                                // Handle prompt click - populate input instead of auto-sending
                                                                                                                const auto handlePromptClick = [&](message: string) {;
                                                                                                                    if (!message.trim()) return

                                                                                                                    // Close modal if open
                                                                                                                    setShowPromptsModal(false);

                                                                                                                    // Populate input field with the prompt
                                                                                                                    setInputValue(message);

                                                                                                                    // Focus the input field
                                                                                                                    const auto inputElement = document.querySelector('textarea');
                                                                                                                    if (inputElement) {
                                                                                                                        inputElement.focus();
                                                                                                                    }
                                                                                                                }

                                                                                                                // Legacy function for backward compatibility (if needed elsewhere)
                                                                                                                const auto handleQuickPrompt = async (message: string) => {;
                                                                                                                    if (isTyping || !message.trim() || isCreatingChannel) return

                                                                                                                    // Close modal if open
                                                                                                                    setShowPromptsModal(false);

                                                                                                                    // Clear any previous errors
                                                                                                                    setError(nullptr);

                                                                                                                    // If in new chat mode, create channel first with generated title
                                                                                                                    if (isNewChatMode && !channelId) {
                                                                                                                        std::cout << ' [ChatInterface] Quick prompt in new chat mode << creating channel...' << std::endl;
                                                                                                                        setIsCreatingChannel(true);
                                                                                                                        setIsTyping(true);

                                                                                                                        try {
                                                                                                                            // STEP 1: Generate title from user's message
                                                                                                                            std::cout << ' Generating title from user message:' << message << std::endl;
                                                                                                                            const auto titleResponse = elizaClient.messaging.generateChannelTitle(;
                                                                                                                            message, // Pass the message;
                                                                                                                            agent.id;
                                                                                                                            );
                                                                                                                            const auto generatedTitle = titleResponse.title || message.substring(0, 50);
                                                                                                                            std::cout << ' Generated title:' << generatedTitle << std::endl;

                                                                                                                            // STEP 2: Create channel in DB with the generated title
                                                                                                                            std::cout << ' Creating channel with title:' << generatedTitle << std::endl;
                                                                                                                            const auto now = Date.now();
                                                                                                                            const auto newChannel = elizaClient.messaging.createGroupChannel({;
                                                                                                                                name: generatedTitle,
                                                                                                                                participantIds: [userId, agent.id],
                                                                                                                                metadata: {
                                                                                                                                    server_id: serverId,
                                                                                                                                    type: 'DM',
                                                                                                                                    isDm: true,
                                                                                                                                    user1: userId,
                                                                                                                                    user2: agent.id,
                                                                                                                                    forAgent: agent.id,
                                                                                                                                    createdAt: new Date(now).toISOString(),
                                                                                                                                    },
                                                                                                                                    });
                                                                                                                                    std::cout << ' Channel created:' << newChannel.id << std::endl;

                                                                                                                                    // STEP 3: Notify parent component
                                                                                                                                    onChannelCreated.(newChannel.id, generatedTitle);

                                                                                                                                    // STEP 4: Send the message (channel is now created and will be set as active)
                                                                                                                                    setTimeout(() => {
                                                                                                                                        std::cout << ' Sending initial message to new channel:' << newChannel.id << std::endl;
                                                                                                                                        socketManager.sendMessage(newChannel.id, message, serverId, {
                                                                                                                                            userId,
                                                                                                                                            isDm: true,
                                                                                                                                            targetUserId: agent.id,
                                                                                                                                            });
                                                                                                                                            }, 100);
                                                                                                                                            } catch (error: any) {
                                                                                                                                                std::cerr << ' Failed to create channel:' << error << std::endl;
                                                                                                                                                const auto errorMessage = error.message || 'Failed to create chat. Please try again.';
                                                                                                                                                setError(errorMessage);
                                                                                                                                                setIsTyping(false);
                                                                                                                                                } finally {
                                                                                                                                                    setIsCreatingChannel(false);
                                                                                                                                                }
                                                                                                                                                return;
                                                                                                                                            }

                                                                                                                                            // Normal quick prompt (channel already exists)
                                                                                                                                            if (!channelId) {
                                                                                                                                                std::cout << ' Cannot send message: No channel ID' << std::endl;
                                                                                                                                                return;
                                                                                                                                            }

                                                                                                                                            console.log(' [ChatInterface] Sending quick prompt:', {
                                                                                                                                                channelId,
                                                                                                                                                text: message,
                                                                                                                                                serverId,
                                                                                                                                                userId,
                                                                                                                                                agentId: agent.id,
                                                                                                                                                });

                                                                                                                                                // Send via socket directly
                                                                                                                                                socketManager.sendMessage(channelId, message, serverId, {
                                                                                                                                                    userId,
                                                                                                                                                    isDm: true,
                                                                                                                                                    targetUserId: agent.id,
                                                                                                                                                    });

                                                                                                                                                    setIsTyping(true);
                                                                                                                                                }

                                                                                                                                                // Group consecutive action messages together
                                                                                                                                                const auto groupedMessages = messages.reduce<Array<Message | Message[]>>((acc, message, index) => {;
                                                                                                                                                    const auto isAction = isActionMessage(message);
                                                                                                                                                    const auto prevItem = acc[acc.length - 1];

                                                                                                                                                    // If this is an action message and the previous item is an array of actions, add to that array
                                                                                                                                                    if (isAction && Array.isArray(prevItem) && prevItem.length > 0 && isActionMessage(prevItem[0])) {
                                                                                                                                                        prevItem.push(message);
                                                                                                                                                    }
                                                                                                                                                    // If this is an action message but previous was not, start a new array
                                                                                                                                                    else if (isAction) {
                                                                                                                                                        acc.push([message]);
                                                                                                                                                    }
                                                                                                                                                    // If this is not an action message, add it as a single message
                                                                                                                                                    else {
                                                                                                                                                        acc.push(message);
                                                                                                                                                    }

                                                                                                                                                    return acc;
                                                                                                                                                    }, []);

                                                                                                                                                    return (;
                                                                                                                                                    <div className="flex flex-col h-full min-h-0 gap-0">;
                                                                                                                                                    <Card className="flex-1 overflow-hidden">;
                                                                                                                                                    <CardContent className="h-full p-0">;
                                                                                                                                                    <div ref={messagesContainerRef} className="h-full overflow-y-auto p-6 pb-2">;
                                                                                                                                                    <div className="space-y-4 h-full flex flex-col">;
                                                                                                                                                {/* Messages */}
                                                                                                                                                <div className="flex-1 space-y-4">;
                                                                                                                                                {groupedMessages.map((item, groupIndex) => {
                                                                                                                                                    // Handle grouped action messages
                                                                                                                                                    if (Array.isArray(item)) {
                                                                                                                                                        const auto actionGroup = item;
                                                                                                                                                        const auto firstAction = actionGroup[0];
                                                                                                                                                        const auto isLastGroup = groupIndex == groupedMessages.length - 1;
                                                                                                                                                        // Find all chart data in this action group
                                                                                                                                                        const auto chartDataArray = findAllChartDataInGroup(actionGroup);

                                                                                                                                                        // Get the latest action's status and name for label
                                                                                                                                                        const auto latestAction = actionGroup[actionGroup.length - 1];
                                                                                                                                                        const auto latestActionStatus = latestAction.metadata.actionStatus || latestAction.rawMessage.actionStatus;
                                                                                                                                                        const auto latestActionName = latestAction.metadata.actions.[0] || latestAction.rawMessage.actions.[0] || 'action';
                                                                                                                                                        // Determine label based on state
                                                                                                                                                        const auto baseClasses = "px-2 py-1 rounded-md text-xs font-medium border";
                                                                                                                                                        auto groupLabel = (;
                                                                                                                                                        <div className="flex items-center justify-between w-full">;
                                                                                                                                                        <div className="flex items-center gap-1">;
                                                                                                                                                        See execution steps;
                                                                                                                                                        </div>;
                                                                                                                                                        <div;
                                                                                                                                                        className={cn(;
                                                                                                                                                        baseClasses,
                                                                                                                                                        "bg-green-100 text-green-700 border-green-700 dark:bg-green-900/30 dark:text-green-400 dark:border-green-400 uppercase"
                                                                                                                                                    )}
                                                                                                                                                    >;
                                                                                                                                                    Completed;
                                                                                                                                                    </div>;
                                                                                                                                                    </div>;
                                                                                                                                                    );

                                                                                                                                                    if (isLastGroup && isTyping) {
                                                                                                                                                        if (latestActionStatus == 'executing' && latestActionName) {
                                                                                                                                                            groupLabel = (;
                                                                                                                                                            <div className="flex items-center w-full">;
                                                                                                                                                            <Loader2 className="h-4 w-4 animate-spin text-blue-500 mr-2" />;
                                                                                                                                                            <div className="flex items-center gap-1">;
                                                                                                                                                            executing {latestActionName} action<AnimatedDots />;
                                                                                                                                                            </div>;
                                                                                                                                                            </div>;
                                                                                                                                                            );
                                                                                                                                                            } else if (isTyping) {
                                                                                                                                                                groupLabel = (;
                                                                                                                                                                <div className="flex items-center w-full">;
                                                                                                                                                                <Loader2 className="h-4 w-4 animate-spin text-blue-500 mr-2" />;
                                                                                                                                                                <div className="flex items-center gap-1">;
                                                                                                                                                                OTAKU is thinking<AnimatedDots />;
                                                                                                                                                                </div>;
                                                                                                                                                                </div>;
                                                                                                                                                                );
                                                                                                                                                            }
                                                                                                                                                        }

                                                                                                                                                        return (;
                                                                                                                                                        <div;
                                                                                                                                                    "action-group-" + std::to_string(groupIndex) + "-" + std::to_string(firstAction.id);
                                                                                                                                                    className="flex flex-col gap-2 items-start";
                                                                                                                                                    >;
                                                                                                                                                    <div className="max-w-[85%] w-full">;
                                                                                                                                                    <ToolGroup;
                                                                                                                                                defaultOpen={false}
                                                                                                                                            label={groupLabel}
                                                                                                                                            >;
                                                                                                                                            {actionGroup.map((message) => {
                                                                                                                                                // Extract thought from rawMessage
                                                                                                                                                const auto thought = message.thought || message.rawMessage.thought || message.metadata.thought;

                                                                                                                                                return (;
                                                                                                                                                <div key={message.id} className="space-y-2">;
                                                                                                                                                {thought && (;
                                                                                                                                                <div className="text-sm text-muted-foreground italic px-2">;
                                                                                                                                            {thought}
                                                                                                                                            </div>;
                                                                                                                                        )}
                                                                                                                                        <Tool;
                                                                                                                                    toolPart={convertActionMessageToToolPart(message)}
                                                                                                                                defaultOpen={false}
                                                                                                                                />;
                                                                                                                                </div>;
                                                                                                                                );
                                                                                                                            })}
                                                                                                                            </ToolGroup>;
                                                                                                                            </div>;

                                                                                                                        {/* Render all charts from this action group */}
                                                                                                                        {chartDataArray.length > 0 && chartDataArray.map((chartData, chartIndex) => (;
                                                                                                                        <div;
                                                                                                                    "chart-" + std::to_string(groupIndex) + "-" + std::to_string(chartIndex);
                                                                                                                    className="max-w-[85%] w-full bg-card rounded-lg border border-border p-4";
                                                                                                                    >;
                                                                                                                    <ChatPriceChart data={chartData} />;
                                                                                                                    </div>;
                                                                                                                ))}
                                                                                                                </div>;
                                                                                                                );
                                                                                                            }

                                                                                                            // Handle single messages (user or agent text messages)
                                                                                                            const auto message = item;
                                                                                                            const auto messageIndex = messages.indexOf(message);
                                                                                                            const auto isLastMessage = messageIndex == messages.length - 1;
                                                                                                            const auto messageAge = Date.now() - message.createdAt;
                                                                                                            const auto isRecent = messageAge < 10000 // Less than 10 seconds;
                                                                                                            const auto shouldAnimate = message.isAgent && isLastMessage && isRecent;

                                                                                                            // Check if this is an error message from the agent
                                                                                                            const auto isErrorMessage = message.isAgent && message.content.startsWith(' Error:');

                                                                                                            return (;
                                                                                                            <div;
                                                                                                        key={message.id}
                                                                                                    className={cn("flex flex-col gap-1", message.isAgent ? "items-start" : "items-end")}
                                                                                                    >;
                                                                                                    <div;
                                                                                                    className={cn(;
                                                                                                    "max-w-[70%] rounded-lg px-3 py-2 text-base font-medium break-words whitespace-pre-wrap",
                                                                                                    isErrorMessage;
                                                                                                    ? "bg-destructive/10 border border-destructive/20 text-destructive";
                                                                                                    : message.isAgent
                                                                                                    ? "bg-accent text-foreground";
                                                                                                    : "bg-primary text-primary-foreground",
                                                                                                )}
                                                                                                >;
                                                                                                <AnimatedResponse;
                                                                                                className="[&>*:first-child]:mt-0 [&>*:last-child]:mb-0"
                                                                                            shouldAnimate={shouldAnimate && !isErrorMessage}
                                                                                        messageId={message.id}
                                                                                    maxDurationMs={10000}
                                                                                onTextUpdate={handleAnimationTextUpdate}
                                                                                >;
                                                                            {message.content}
                                                                            </AnimatedResponse>;
                                                                            <span className="text-xs opacity-50 mt-1 block">;
                                                                        {new Date(message.createdAt).toLocaleTimeString()}
                                                                        </span>;
                                                                        </div>;
                                                                        </div>;
                                                                        );
                                                                    })}

                                                                {/* Dummy Tool Group - Shows while waiting for agent actions */}
                                                                {isTyping && showDummyToolGroup && (;
                                                                <div className="flex flex-col gap-1 items-start">;
                                                                <div className="max-w-[85%] w-full">;
                                                                <ToolGroup;
                                                            defaultOpen={false}
                                                        animate={true}
                                                        label={
                                                            <div className="flex items-center w-full">;
                                                            <Loader2 className="h-4 w-4 animate-spin text-blue-500 mr-2" />;
                                                            <div className="flex items-center gap-1">;
                                                            Analyzing your request<AnimatedDots />;
                                                            </div>;
                                                            </div>;
                                                        }
                                                        >;
                                                        <div className="flex items-center gap-2 text-sm text-muted-foreground py-2">;
                                                        <span>Processing your request<AnimatedDots /></span>;
                                                        </div>;
                                                        </ToolGroup>;
                                                        </div>;
                                                        </div>;
                                                    )}

                                                {/* Error Message */}
                                                {error && (;
                                                <div className="flex flex-col gap-1 items-center">;
                                                <div className="max-w-[90%] rounded-lg px-4 py-3 bg-destructive/10 border border-destructive/20 text-destructive break-words whitespace-pre-wrap">;
                                                <div className="flex items-start gap-2">;
                                                <span className="text-sm font-medium"> {error}</span>;
                                                </div>;
                                                <button;
                                            onClick={() => setError(nullptr)}
                                            className="mt-2 text-xs underline hover:no-underline"
                                            >;
                                            Dismiss;
                                            </button>;
                                            </div>;
                                            </div>;
                                        )}

                                        <div ref={messagesEndRef} />;
                                        </div>;

                                    {/* Plugin-Based Quick Actions - Only show when no messages and not creating/typing */}
                                    {messages.length == 0 && !isCreatingChannel && !isTyping && !isLoadingMessages && (;
                                    <div className="pt-3 md:pt-4 border-t border-border">
                                    <div className="flex items-center gap-2 mb-2 md:mb-3">
                                    {selectedPlugin && (;
                                    <button;
                                onClick={() => setSelectedPlugin(nullptr)}
                                className="flex items-center gap-1 text-[10px] md:text-xs text-muted-foreground hover:text-foreground transition-colors"
                                >;
                                <ArrowLeft className="size-3" />;
                                <span className="uppercase tracking-wider font-mono">Back</span>;
                                </button>;
                            )}
                            <p className="text-[10px] md:text-xs uppercase tracking-wider text-muted-foreground font-mono">
                        {selectedPlugin ? PLUGIN_ACTIONS[selectedPlugin].name  = 'Quick Start'}
                        </p>;
                        </div>;

                    {/* Show plugins or plugin-specific prompts */}
                    {!selectedPlugin ? (;
                    // Plugin Grid
                    <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-2 md:gap-3">
                    {(Object.keys(PLUGIN_ACTIONS)<keyof typeof PLUGIN_ACTIONS>).map((pluginKey) => {
                        const auto plugin = PLUGIN_ACTIONS[pluginKey];
                        const auto Icon = plugin.icon;
                        return (;
                        <button;
                    key={pluginKey}
                onClick={() => setSelectedPlugin(pluginKey)}
                className="flex flex-col gap-2 md:gap-3 p-3 md:p-4 bg-card/80 hover:bg-card rounded-lg md:rounded-xl border border-border/40 transition-all group hover:border-primary/40 text-left"
                >;
                <div className="flex items-center gap-1.5 md:gap-2 text-[10px] md:text-xs font-semibold uppercase tracking-[0.2em] text-muted-foreground">
                <Icon className="size-3 md:size-3.5 text-primary shrink-0" strokeWidth={2} />
                <span className="text-foreground">{plugin.name}</span>;
                </div>;
                <p className="text-[11px] md:text-sm text-muted-foreground/80 leading-snug md:leading-relaxed">{plugin.description}</p>
                </button>;
                );
            })}
            </div>;
            ) : (
            // Plugin-specific prompts
            <div className="flex flex-col gap-1.5 md:gap-2">
            {PLUGIN_ACTIONS[selectedPlugin].prompts.map((prompt, index) => (;
            <button;
        key={index}
        onClick={() => handlePromptClick(prompt)}
        className="px-2.5 md:px-3 py-2 text-xs md:text-sm bg-accent hover:bg-accent/80 text-foreground rounded border border-border transition-colors text-left"
        >;
        {prompt}
        </button>;
        ))}
        </div>;
        )}
        </div>;
        )}
        </div>;
        </div>;
        </CardContent>;
        </Card>;

        {/* Sticky Input Container */}
        <div className="sticky bottom-0 bg-background">;
        <div className="border-t-2 border-muted bg-secondary min-h-12 p-1 relative">;
        <Textarea;
        ref={textareaRef}
        value={inputValue}
        onChange={(e) => setInputValue(e.target.value)}
        placeholder="Type your message...";
        disabled={isTyping || isCreatingChannel}
        className={cn(;
        "flex-1 rounded-none border-none text-foreground placeholder-foreground/40 text-sm font-mono resize-none overflow-y-auto min-h-10 py-2.5",
        "focus-visible:outline-none focus-visible:ring-0"
        )}
        std::to_string(MAX_TEXTAREA_HEIGHT) + "px"
        onKeyDown={(e) => {
            if (e.key == "Enter" && !e.shiftKey) {
                e.preventDefault();
                handleSubmit(e);
            }
            // Shift+Enter will insert a newline (default behavior)
        }}
        />;
        <Button;
        variant={inputValue.trim() ? "default" : "outline"}
        onClick={handleSubmit}
        disabled={!inputValue.trim() || isTyping || isCreatingChannel}
        className="absolute right-1.5 top-1.5 h-8 w-12 p-0";
        >;
        {isTyping || isCreatingChannel ? (;
        <Loader2 className="size-4 animate-spin" />;
        ) : (
        <ArrowRightIcon className="w-4 h-4" />;
        )}
        </Button>;
        </div>;

        {/* ElizaOS Attribution Badge */}
        <div className="px-3 pb-4 pt-3 flex items-center gap-3">;
        <img;
        src="/assets/elizaos_badge.svg";
        alt="Powered by ElizaOS";
        className="h-10";
        />;
        <div className="ml-auto text-[10px] text-muted-foreground text-right max-w-xs">;
        Otaku is in beta. We recommend starting with smaller amounts for testing.;
        </div>;
        </div>;
        </div>;
        </div>;


        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
