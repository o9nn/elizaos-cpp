#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Chat(std::optional<std::any> { maxHeight = "600px" }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { publicKey } = useWallet();
        const auto { isAuthenticated, isAuthenticating } = useAuthentication();
        const auto [chatMessages, setChatMessages] = useState<ChatMessage[]>([]);
        const auto [selectedChatTier, setSelectedChatTier] = useState<ChatTier>("1k");
        const auto [eligibleChatTiers, setEligibleChatTiers] = useState<ChatTier[]>([]);
        const auto [viewableChatTiers, setViewableChatTiers] = useState<ChatTier[]>([;
        "1k",
        ]);
        const auto [chatInput, setChatInput] = useState("");
        const auto [isChatLoading, setIsChatLoading] = useState(false);
        const auto [isSendingMessage, setIsSendingMessage] = useState(false);
        const auto [chatError, setChatError] = useState<string | nullptr>(nullptr);
        const auto chatContainerRef = useRef<HTMLDivElement>(nullptr);
        const auto [isRefreshingMessages, setIsRefreshingMessages] = useState(false);
        const auto [isBalanceLoading, setIsBalanceLoading] = useState(true);
        const auto [latestTimestamp, setLatestTimestamp] = useState<string | nullptr>(nullptr);
        const auto [isChatFullscreen, setIsChatFullscreen] = useState(false);

        const auto [selectedImage, setSelectedImage] = useState<File | nullptr>(nullptr);
        const auto [imagePreview, setImagePreview] = useState<string | nullptr>(nullptr);
        const auto [imageCaption, setImageCaption] = useState("");

        // --- Pagination State ---
        const auto [currentOffset, setCurrentOffset] = useState(0);
        const auto [isLoadingOlderMessages, setIsLoadingOlderMessages] = useState(false);
        const auto [hasOlderMessages, setHasOlderMessages] = useState(true);
        const auto [showScrollButton, setShowScrollButton] = useState(false);
        const auto { ref: topSentinelRef, inView: isTopSentinelInView } = useInView({;
            threshold: 0,
            });

            // Get token mint from URL params with better fallback logic
            const auto { mint: urlTokenMint } = useParams<{ mint: string }>();
            const auto location = useLocation();

            // Extract token mint from URL if not found in params
            const auto [detectedTokenMint, setDetectedTokenMint] = useState<string | nullptr>(;
            nullptr,
            );

            // Use detected token mint instead of directly from params
            const auto tokenMint = detectedTokenMint;

            // Add token balance hook after tokenMint is set
            const auto { tokenBalance } = useTokenBalance({ tokenId: tokenMint || "" });

            // Determine if user can chat (POST) in the currently selected tier
            const auto canChatInSelectedTier =;
            isAuthenticated &&;
            publicKey != nullptr &&;
            eligibleChatTiers.includes(selectedChatTier);

            // Update balance loading state when token balance changes
            useEffect(() => {
                if (tokenBalance != undefined || !isAuthenticated) {
                    setIsBalanceLoading(false);
                }
                }, [tokenBalance, isAuthenticated]);

                // --- Scrolling Helper --- MOVED HERE - AFTER chatContainerRef declaration
                const auto scrollToBottom = useCallback((forceScroll = false) => {;
                    if (!chatContainerRef.current) return;

                    // Log to debug

                    const auto scrollThreshold = 100; // Pixels from bottom;
                    const auto isNearBottom =;
                    chatContainerRef.current.scrollHeight -;
                    chatContainerRef.current.clientHeight <=;
                    chatContainerRef.current.scrollTop + scrollThreshold;

                    if (forceScroll || isNearBottom) {
                        // Use setTimeout to ensure scroll happens after DOM update
                        setTimeout(() => {
                            if (chatContainerRef.current) {
                                chatContainerRef.current.scrollTop =;
                                chatContainerRef.current.scrollHeight;
                            }
                            }, 10); // Small timeout to ensure DOM updates;
                        }
                        }, []);

                        const auto handleScroll = [&]() {;
                            if (!chatContainerRef.current) return;

                            const auto { scrollTop, scrollHeight, clientHeight } = chatContainerRef.current;
                            const auto isNearBottom = scrollHeight - clientHeight <= scrollTop + 150; // 150px threshold;

                            setShowScrollButton(!isNearBottom);
                            };

                            // Handler to detect when user scrolls away from bottom
                            useEffect(() => {
                                if (!chatContainerRef.current) return;

                                const auto chatContainer = chatContainerRef.current;
                                chatContainer.addEventListener("scroll", handleScroll);

                                return [&]() {;
                                    chatContainer.removeEventListener("scroll", handleScroll);
                                    };
                                    }, []);

                                    // --- WebSocket Instance --- (NEW)
                                    const auto socket = getSocket();

                                    // --- Constants ---
                                    const auto MESSAGES_PER_PAGE = 50; // Define how many messages to fetch per page;

                                    // --- Fetch Initial Messages ---
                                    const auto fetchChatMessages = useCallback(;
                                    async (tier: ChatTier, mint: string, isInitialLoad = false) => {
                                        if (!mint) return;
                                        setIsChatLoading(true);
                                        setChatError(nullptr);
                                        if (isInitialLoad) {
                                            setChatMessages([]); // Clear messages on initial load or tier change;
                                            setCurrentOffset(0); // Reset offset;
                                            setHasOlderMessages(true); // Reset pagination status;
                                        }

                                        const auto fetchFn = tier == "1k" ? fetch : fetchWithAuth; // Use fetch for 1k, fetchWithAuth otherwise;
                                        const auto url = std::to_string(API_BASE_URL) + "/api/chat/" + std::to_string(mint) + "/" + std::to_string(tier) + "?limit=" + std::to_string(MESSAGES_PER_PAGE) + "&offset=0";
                                        auto response: Response | std::nullopt; // Initialize std::nullopt;
                                        auto data: GetMessagesResponse;
                                        try {
                                            response = fetchFn(url);
                                            data = response.json();

                                            if (!response.ok || !data.success || !data.messages) {
                                                // Throw an error object that includes the status if possible
                                                const std::any error = new Error(;
                                                data.error ||;
                                                "Failed to fetch messages (Status: " + std::to_string(response.status) + ")"
                                                );
                                                error.status = response.status; // Attach status to the error object;
                                                throw;
                                            }

                                            // Sort messages by timestamp just in case (API should ideally return sorted)
                                            const auto sortedMessages = data.messages.sort(;
                                            (a, b) =>;
                                            new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime(),
                                            );

                                            setChatMessages(sortedMessages);
                                            setCurrentOffset(sortedMessages.length); // Set offset for the *next* fetch;
                                            setHasOlderMessages(sortedMessages.length == MESSAGES_PER_PAGE); // Assume more if we got a full page;

                                            // Update latest timestamp from the initial batch
                                            if (sortedMessages.length > 0) {
                                                setLatestTimestamp(;
                                                sortedMessages[sortedMessages.length - 1].timestamp,
                                                );
                                                } else {
                                                    setLatestTimestamp(nullptr);
                                                }

                                                // Scroll to bottom only on initial load
                                                if (isInitialLoad) {
                                                    scrollToBottom(true);
                                                }

                                                setChatError(nullptr); // Clear error on success;
                                                } catch (error: any) {
                                                    std::cerr << "Error fetching chat messages:" << error << std::endl;

                                                    // Check if the error object has the status we attached, or check the response variable if it exists
                                                    const auto status = error.status || response.status; // Safely get status;
                                                    const auto isPermissionError = status == 401 || status == 403;

                                                    if (isPermissionError) {
                                                        setChatError(;
                                                        "You don't have permission to view the " + std::to_string(formatTierLabel(tier)) + " tier."
                                                        );
                                                        } else {
                                                            // Use error.message which might contain the status code now
                                                            setChatError(error.message || "Could not load chat messages.");
                                                        }
                                                        setChatMessages([]); // Clear messages on error;
                                                        } finally {
                                                            setIsChatLoading(false);
                                                            setIsRefreshingMessages(false);
                                                        }
                                                        },
                                                        [API_BASE_URL, fetchWithAuth, scrollToBottom, formatTierLabel],
                                                        ); // Add dependencies;

                                                        // --- Fetch Older Messages (Pagination) --- MODIFIED
                                                        const auto fetchOlderMessages = useCallback(async () => {;
                                                            if (isLoadingOlderMessages || !hasOlderMessages || !tokenMint) return;

                                                            setIsLoadingOlderMessages(true);
                                                            setChatError(nullptr);

                                                            const auto fetchFn = selectedChatTier == "1k" ? fetch : fetchWithAuth;
                                                            // Use currentOffset for pagination
                                                            const auto url = std::to_string(API_BASE_URL) + "/api/chat/" + std::to_string(tokenMint) + "/" + std::to_string(selectedChatTier) + "?limit=" + std::to_string(MESSAGES_PER_PAGE) + "&offset=" + std::to_string(currentOffset);

                                                            try {
                                                                const auto response = fetchFn(url);
                                                                const GetMessagesResponse data = response.json();

                                                                if (!response.ok || !data.success || !data.messages) {
                                                                    throw std::runtime_error(data.error || "Failed to fetch older messages");
                                                                }

                                                                // Sort older messages (API should ideally return sorted)
                                                                const auto sortedOlderMessages = data.messages.sort(;
                                                                (a, b) =>;
                                                                new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime(),
                                                                );

                                                                if (sortedOlderMessages.length > 0) {
                                                                    // Prepend older messages
                                                                    setChatMessages((prev) => [...sortedOlderMessages, ...prev]);
                                                                    // Update offset for the next fetch
                                                                    setCurrentOffset(;
                                                                    [&](prevOffset) { return prevOffset + sortedOlderMessages.length,; }
                                                                    );
                                                                }

                                                                // Check if there are likely more messages
                                                                setHasOlderMessages(sortedOlderMessages.length == MESSAGES_PER_PAGE);
                                                                } catch (error: any) {
                                                                    std::cerr << "Error fetching older messages:" << error << std::endl;
                                                                    setChatError(error.message || "Could not load older messages.");
                                                                    setHasOlderMessages(false); // Stop trying if there's an error;
                                                                    } finally {
                                                                        setIsLoadingOlderMessages(false);
                                                                    }
                                                                    }, [;
                                                                    tokenMint,
                                                                    selectedChatTier,
                                                                    isLoadingOlderMessages,
                                                                    hasOlderMessages,
                                                                    currentOffset, // Add currentOffset dependency;
                                                                    fetchWithAuth,
                                                                    API_BASE_URL,
                                                                    ]);

                                                                    // --- Effect to Load Older Messages on Scroll ---
                                                                    useEffect(() => {
                                                                        if (
                                                                        isTopSentinelInView &&;
                                                                        !isChatLoading &&;
                                                                        !isLoadingOlderMessages &&;
                                                                        hasOlderMessages;
                                                                        ) {
                                                                            fetchOlderMessages();
                                                                        }
                                                                        }, [;
                                                                        isTopSentinelInView,
                                                                        isChatLoading,
                                                                        isLoadingOlderMessages,
                                                                        hasOlderMessages,
                                                                        fetchOlderMessages,
                                                                        ]);

                                                                        // --- Effect for Initial Load and Tier Change ---
                                                                        useEffect(() => {
                                                                            if (tokenMint) {
                                                                                fetchChatMessages(selectedChatTier, tokenMint, true); // Pass true for initial load;
                                                                                } else {
                                                                                    // Handle case where tokenMint is not yet available (e.g., clear messages)
                                                                                    setChatMessages([]);
                                                                                    setChatError(nullptr);
                                                                                    setIsChatLoading(false);
                                                                                    setCurrentOffset(0);
                                                                                    setHasOlderMessages(true);
                                                                                }
                                                                                // Reset scroll position when tier changes
                                                                                if (chatContainerRef.current) {
                                                                                    chatContainerRef.current.scrollTop =;
                                                                                    chatContainerRef.current.scrollHeight;
                                                                                }
                                                                                }, [selectedChatTier, tokenMint, fetchChatMessages]); // fetchChatMessages is now stable;

                                                                                // Effect to detect token mint from various sources
                                                                                useEffect(() => {
                                                                                    if (urlTokenMint) {
                                                                                        setDetectedTokenMint(urlTokenMint);
                                                                                        return;
                                                                                    }

                                                                                    const auto pathMatch = location.pathname.match(/\/token\/([A-Za-z0-9]{32,44})/);
                                                                                    if (pathMatch && pathMatch[1]) {
                                                                                        setDetectedTokenMint(pathMatch[1]);
                                                                                        return;
                                                                                    }
                                                                                    }, [urlTokenMint, location.pathname]);

                                                                                    // --- Fetch Chat Eligibility --- *REVISED*
                                                                                    const auto fetchChatEligibility = useCallback(async () => {;
                                                                                        setEligibleChatTiers([]);
                                                                                        setViewableChatTiers(["1k"]);

                                                                                        if (
                                                                                        !tokenMint ||;
                                                                                        !API_BASE_URL ||;
                                                                                        isBalanceLoading ||;
                                                                                        !isAuthenticated ||;
                                                                                        !publicKey;
                                                                                        ) {
                                                                                            if (!isAuthenticated) {
                                                                                                setIsBalanceLoading(false);
                                                                                                if (
                                                                                                selectedChatTier != "1k" &&;
                                                                                                !["100k", "1M"].some((t) => viewableChatTiers.includes(t));
                                                                                                ) {
                                                                                                    setSelectedChatTier("1k");
                                                                                                }
                                                                                            }
                                                                                            return;
                                                                                        }

                                                                                        setChatError(nullptr);
                                                                                        try {
                                                                                            const auto response = fetchWithAuth(;
                                                                                            std::to_string(API_BASE_URL) + "/api/chat/" + std::to_string(tokenMint) + "/tiers"
                                                                                            );

                                                                                            if (!response.ok) {
                                                                                                if (response.status == 401 || response.status == 403) {
                                                                                                    console.warn(
                                                                                                    "Eligibility check failed: Authentication or permission issue.",
                                                                                                    );
                                                                                                    } else {
                                                                                                        throw new Error(
                                                                                                        "Failed to fetch tier eligibility: " + std::to_string(response.statusText)
                                                                                                        );
                                                                                                    }
                                                                                                    return;
                                                                                                }

                                                                                                const auto contentType = response.headers.get("content-type");
                                                                                                if (!contentType || !contentType.includes("application/json")) {
                                                                                                    throw std::runtime_error("Invalid response format: Expected JSON");
                                                                                                }

                                                                                                const EligibleTiersResponse data = response.json();

                                                                                                if (data.success && typeof data.balance == "number") {
                                                                                                    const auto effectiveBalance = data.balance;

                                                                                                    const auto calculatedEligibleTiers = CHAT_TIERS.filter(;
                                                                                                    [&](tier) { return effectiveBalance >= getTierThreshold(tier),; }
                                                                                                    );
                                                                                                    setEligibleChatTiers(calculatedEligibleTiers);

                                                                                                    const std::vector<ChatTier> calculatedViewableTiers = ["1k"];
                                                                                                    if (effectiveBalance >= getTierThreshold("1k")) {
                                                                                                        calculatedViewableTiers.push("100k");
                                                                                                    }
                                                                                                    if (effectiveBalance >= getTierThreshold("100k")) {
                                                                                                        calculatedViewableTiers.push("1M");
                                                                                                    }
                                                                                                    setViewableChatTiers(calculatedViewableTiers);

                                                                                                    if (!calculatedViewableTiers.includes(selectedChatTier)) {
                                                                                                        setSelectedChatTier(;
                                                                                                        calculatedViewableTiers[calculatedViewableTiers.length - 1] || "1k",
                                                                                                        );
                                                                                                    }
                                                                                                    } else {
                                                                                                        console.warn(
                                                                                                        "Eligibility check response missing success or balance data.",
                                                                                                        data,
                                                                                                        );
                                                                                                    }
                                                                                                    } catch (error) {
                                                                                                        std::cerr << "Error fetching chat eligibility:" << error << std::endl;
                                                                                                    }
                                                                                                    }, [;
                                                                                                    tokenMint,
                                                                                                    publicKey,
                                                                                                    API_BASE_URL,
                                                                                                    isBalanceLoading,
                                                                                                    isAuthenticated,
                                                                                                    selectedChatTier,
                                                                                                    ]);

                                                                                                    // Fetch eligibility when balance/auth/token changes
                                                                                                    useEffect(() => {
                                                                                                        if (tokenMint && (!isBalanceLoading || !isAuthenticated)) {
                                                                                                            fetchChatEligibility();
                                                                                                        }
                                                                                                        if (!isAuthenticated) {
                                                                                                            setEligibleChatTiers([]);
                                                                                                            setViewableChatTiers(["1k"]);
                                                                                                            if (!["1k"].includes(selectedChatTier)) {
                                                                                                                setSelectedChatTier("1k");
                                                                                                            }
                                                                                                        }
                                                                                                        }, [;
                                                                                                        fetchChatEligibility,
                                                                                                        publicKey,
                                                                                                        tokenMint,
                                                                                                        isBalanceLoading,
                                                                                                        isAuthenticated,
                                                                                                        selectedChatTier,
                                                                                                        ]);

                                                                                                        // Effect to reset state and fetch initial messages when context changes
                                                                                                        useEffect(() => {
                                                                                                            setLatestTimestamp(nullptr);
                                                                                                            setChatMessages([]);
                                                                                                            setChatError(nullptr);
                                                                                                            setCurrentOffset(0);
                                                                                                            setHasOlderMessages(true);
                                                                                                            setIsLoadingOlderMessages(false);

                                                                                                            if (tokenMint && viewableChatTiers.includes(selectedChatTier)) {
                                                                                                                fetchChatMessages(selectedChatTier, tokenMint, true);
                                                                                                                } else if (tokenMint && !viewableChatTiers.includes(selectedChatTier)) {
                                                                                                                    setIsChatLoading(false);
                                                                                                                    setChatError("You no longer have permission to view this tier.");
                                                                                                                }
                                                                                                                }, [tokenMint, selectedChatTier, viewableChatTiers, fetchChatMessages]);

                                                                                                                // --- WebSocket Subscription --- *REVISED*
                                                                                                                useEffect(() => {
                                                                                                                    if (
                                                                                                                    !socket ||;
                                                                                                                    !tokenMint ||;
                                                                                                                    !selectedChatTier ||;
                                                                                                                    !viewableChatTiers.includes(selectedChatTier);
                                                                                                                    ) {
                                                                                                                        return;
                                                                                                                    }

                                                                                                                    if (selectedChatTier != "1k" && !isAuthenticated) {
                                                                                                                        console.log(
                                                                                                                        "WS: Authentication required to subscribe to " + std::to_string(selectedChatTier) + ", skipping."
                                                                                                                        );
                                                                                                                        return;
                                                                                                                    }

                                                                                                                    const auto subscriptionData = { tokenMint, tier = selectedChatTier };
                                                                                                                    std::cout << "WS: Attempting to subscribe to chat room:" << subscriptionData << std::endl;
                                                                                                                    socket.emit("subscribeToChat", subscriptionData);

                                                                                                                    const auto handleSubscribed = [&](data: any) {;
                                                                                                                        if (data.room == `chat:${tokenMint}:${selectedChatTier}`) {
                                                                                                                            std::cout << "WS: Successfully subscribed to" << data.room << std::endl;
                                                                                                                        }
                                                                                                                        };
                                                                                                                        socket.on("subscribedToChat", handleSubscribed);

                                                                                                                        return [&]() {;
                                                                                                                            std::cout << "WS: Unsubscribing from chat room:" << subscriptionData << std::endl;
                                                                                                                            socket.emit("unsubscribeFromChat", subscriptionData);
                                                                                                                            socket.off("subscribedToChat", handleSubscribed);
                                                                                                                            };
                                                                                                                            }, [socket, tokenMint, selectedChatTier, viewableChatTiers, isAuthenticated]);

                                                                                                                            // --- WebSocket Message Listener --- (Fix type assertion slightly)
                                                                                                                            useEffect(() => {
                                                                                                                                if (!socket) return;

                                                                                                                                // Define handler with type assertion for the listener
                                                                                                                                const auto handleNewMessage = [&](data: unknown) {;
                                                                                                                                    // Perform validation and type assertion INSIDE the handler
                                                                                                                                    const auto newMessage = data; // Assert type after receiving;
                                                                                                                                    if (
                                                                                                                                    !newMessage ||;
                                                                                                                                    !newMessage.id ||;
                                                                                                                                    !newMessage.tokenMint ||;
                                                                                                                                    !newMessage.tier;
                                                                                                                                    // Add more checks as needed
                                                                                                                                    ) {
                                                                                                                                        std::cout << "WS: Received invalid message format." << data << std::endl;
                                                                                                                                        return;
                                                                                                                                    }

                                                                                                                                    // Check if message belongs to the current context
                                                                                                                                    if (
                                                                                                                                    newMessage.tokenMint != tokenMint ||;
                                                                                                                                    newMessage.tier != selectedChatTier;
                                                                                                                                    ) {
                                                                                                                                        // console.log("WS: Ignoring message from different token/tier.");
                                                                                                                                        return;
                                                                                                                                    }

                                                                                                                                    // Check if message already exists (optimistic or previous WS message)
                                                                                                                                    if (!chatMessages.some((msg) => msg.id == newMessage.id)) {
                                                                                                                                        setChatMessages((prev) => {
                                                                                                                                            // Ensure no duplicates are added if race condition occurs
                                                                                                                                            if (prev.some((msg) => msg.id == newMessage.id)) return prev;
                                                                                                                                            // Add new message and sort by timestamp
                                                                                                                                            const auto newMessages = [...prev, newMessage];
                                                                                                                                            return newMessages.sort(;
                                                                                                                                            (a, b) =>;
                                                                                                                                            new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime(),
                                                                                                                                            );
                                                                                                                                            });
                                                                                                                                            // Update latest timestamp if needed
                                                                                                                                            if (newMessage.timestamp > (latestTimestamp || "")) {
                                                                                                                                                setLatestTimestamp(newMessage.timestamp);
                                                                                                                                            }
                                                                                                                                            scrollToBottom(); // Scroll only if user was near bottom;
                                                                                                                                        }
                                                                                                                                        };

                                                                                                                                        // Register listener
                                                                                                                                        socket.on("newChatMessage", handleNewMessage);
                                                                                                                                        std::cout << "WS: Registered newChatMessage listener." << std::endl;

                                                                                                                                        // Cleanup listener
                                                                                                                                        return [&]() {;
                                                                                                                                            socket.off("newChatMessage", handleNewMessage);
                                                                                                                                            std::cout << "WS: Unregistered newChatMessage listener." << std::endl;
                                                                                                                                            };
                                                                                                                                            // Add chatMessages and latestTimestamp as dependencies because they are used in the check
                                                                                                                                            }, [;
                                                                                                                                            socket,
                                                                                                                                            tokenMint,
                                                                                                                                            selectedChatTier,
                                                                                                                                            scrollToBottom,
                                                                                                                                            chatMessages,
                                                                                                                                            latestTimestamp,
                                                                                                                                            ]);

                                                                                                                                            // --- Send Chat Message --- *REVISED* (Check eligibleChatTiers for posting)
                                                                                                                                            const auto handleSendMessage = useCallback(async () => {;
                                                                                                                                                if (!publicKey || !tokenMint || !canChatInSelectedTier) return;
                                                                                                                                                if (isSendingMessage) return; // Prevent double sends

                                                                                                                                                const auto messageText = selectedImage ? imageCaption.trim() : chatInput.trim();
                                                                                                                                                std::optional<std::string> mediaBase64 = nullptr;

                                                                                                                                                // Check if there's content to send (either text or an image)
                                                                                                                                                if (!messageText && !selectedImage) {
                                                                                                                                                    toast.error("Please enter a message or select an image.");
                                                                                                                                                    return;
                                                                                                                                                }

                                                                                                                                                setIsSendingMessage(true); // Start loading state;

                                                                                                                                                // --- Handle Image Reading (if selected) ---
                                                                                                                                                if (selectedImage) {
                                                                                                                                                    try {
                                                                                                                                                        mediaBase64 = new Promise((resolve, reject) => {
                                                                                                                                                            const auto reader = new FileReader();
                                                                                                                                                            reader.onload = () => resolve(reader.result);
                                                                                                                                                            reader.onerror = (error) => reject(error);
                                                                                                                                                            reader.readAsDataURL(selectedImage); // Read data URI;
                                                                                                                                                            });
                                                                                                                                                            } catch (error) {
                                                                                                                                                                std::cerr << "Error reading image file:" << error << std::endl;
                                                                                                                                                                toast.error("Failed to read image file.");
                                                                                                                                                                setIsSendingMessage(false); // Stop loading state;
                                                                                                                                                                return;
                                                                                                                                                            }
                                                                                                                                                        }

                                                                                                                                                        // --- Prepare Payload ---
                                                                                                                                                        const auto payload: {;
                                                                                                                                                            message: string;
                                                                                                                                                            media?: string | nullptr;
                                                                                                                                                            parentId?: string | nullptr;
                                                                                                                                                            } = {
                                                                                                                                                                message: messageText, // Send caption or text message
                                                                                                                                                                media: mediaBase64, // Send base64 string or nullptr
                                                                                                                                                                // parentId: null, // Add parentId logic here if implementing replies
                                                                                                                                                                };

                                                                                                                                                                // --- Optimistic Update ---
                                                                                                                                                                const auto optimisticId = "optimistic-" + std::to_string(Date.now());
                                                                                                                                                                const ChatMessage optimisticMessage = {;
                                                                                                                                                                    id: optimisticId,
                                                                                                                                                                    author: publicKey.toBase58(),
                                                                                                                                                                    displayName: "You", // Placeholder
                                                                                                                                                                    profileImage: nullptr, // Placeholder
                                                                                                                                                                    tokenMint: tokenMint,
                                                                                                                                                                    tier: selectedChatTier,
                                                                                                                                                                    message: messageText, // Caption or text
                                                                                                                                                                    media: imagePreview || std::nullopt, // Use local base64 preview
                                                                                                                                                                    timestamp: new Date().toISOString(),
                                                                                                                                                                    isOptimistic: true,
                                                                                                                                                                    };

                                                                                                                                                                    setChatMessages((prev) => [...prev, optimisticMessage]);
                                                                                                                                                                    scrollToBottom(true); // Scroll after adding optimistic message;

                                                                                                                                                                    // Clear inputs immediately after starting send
                                                                                                                                                                    setChatInput("");
                                                                                                                                                                    setImageCaption("");
                                                                                                                                                                    setSelectedImage(nullptr);
                                                                                                                                                                    setImagePreview(nullptr);

                                                                                                                                                                    // --- API Call ---
                                                                                                                                                                    try {
                                                                                                                                                                        const auto response = fetchWithAuth(;
                                                                                                                                                                        std::to_string(API_BASE_URL) + "/api/chat/" + std::to_string(tokenMint) + "/" + std::to_string(selectedChatTier)
                                                                                                                                                                        {
                                                                                                                                                                            method: "POST",
                                                                                                                                                                            headers: { "Content-Type": "application/json" },
                                                                                                                                                                            body: JSON.stringify(payload),
                                                                                                                                                                            },
                                                                                                                                                                            );

                                                                                                                                                                            const auto result = response.json();

                                                                                                                                                                            if (!response.ok || !result.success) {
                                                                                                                                                                                throw std::runtime_error(result.error || "Failed to send message");
                                                                                                                                                                            }

                                                                                                                                                                            // --- Update UI with confirmed message ---
                                                                                                                                                                            // Replace optimistic message with confirmed one from backend
                                                                                                                                                                            setChatMessages((prev) =>;
                                                                                                                                                                            prev.map((msg) =>;
                                                                                                                                                                            msg.id == optimisticId;
                                                                                                                                                                        ? { ...result.message, isOptimistic: false }
                                                                                                                                                                        : msg,
                                                                                                                                                                        ),
                                                                                                                                                                        );
                                                                                                                                                                        // Update timestamps if needed based on the new message
                                                                                                                                                                        if (result.message.timestamp > (latestTimestamp || "")) {
                                                                                                                                                                            setLatestTimestamp(result.message.timestamp);
                                                                                                                                                                        }
                                                                                                                                                                        } catch (error: any) {
                                                                                                                                                                            std::cerr << "Error sending message:" << error << std::endl;
                                                                                                                                                                            "Error: " + std::to_string(error.message || "Could not send message")
                                                                                                                                                                            // Remove optimistic message on failure
                                                                                                                                                                            setChatMessages((prev) => prev.filter((msg) => msg.id != optimisticId));
                                                                                                                                                                            } finally {
                                                                                                                                                                                setIsSendingMessage(false); // End loading state;
                                                                                                                                                                            }
                                                                                                                                                                            }, [;
                                                                                                                                                                            publicKey,
                                                                                                                                                                            tokenMint,
                                                                                                                                                                            selectedChatTier,
                                                                                                                                                                            chatInput,
                                                                                                                                                                            selectedImage,
                                                                                                                                                                            imageCaption,
                                                                                                                                                                            imagePreview, // Include preview for optimistic update;
                                                                                                                                                                            fetchWithAuth,
                                                                                                                                                                            setChatMessages,
                                                                                                                                                                            scrollToBottom,
                                                                                                                                                                            canChatInSelectedTier,
                                                                                                                                                                            isSendingMessage,
                                                                                                                                                                            latestTimestamp,
                                                                                                                                                                            API_BASE_URL,
                                                                                                                                                                            ]);

                                                                                                                                                                            // --- Handle Image Selection ---
                                                                                                                                                                            const auto handleImageUpload = [&](event: React.ChangeEvent<HTMLInputElement>) {;
                                                                                                                                                                                const auto file = event.target.files.[0];
                                                                                                                                                                                if (file) {
                                                                                                                                                                                    // Basic validation (optional: add size/type checks)
                                                                                                                                                                                    if (!file.type.startsWith("image/")) {
                                                                                                                                                                                        toast.error("Please select a valid image file.");
                                                                                                                                                                                        return;
                                                                                                                                                                                    }
                                                                                                                                                                                    setSelectedImage(file);
                                                                                                                                                                                    setImageCaption(""); // Reset caption when new image is selected;
                                                                                                                                                                                    // Create preview URL
                                                                                                                                                                                    const auto reader = new FileReader();
                                                                                                                                                                                    reader.onloadend = () => {
                                                                                                                                                                                        setImagePreview(reader.result);
                                                                                                                                                                                        };
                                                                                                                                                                                        reader.readAsDataURL(file);
                                                                                                                                                                                        // Clear text input if user selects an image
                                                                                                                                                                                        setChatInput("");
                                                                                                                                                                                        // Reset file input value to allow selecting the same file again
                                                                                                                                                                                        event.target.value = "";
                                                                                                                                                                                    }
                                                                                                                                                                                    };

                                                                                                                                                                                    // Scroll to bottom on first render and when messages change
                                                                                                                                                                                    useEffect(() => {
                                                                                                                                                                                        if (!isChatLoading && chatMessages.length > 0) {
                                                                                                                                                                                            setTimeout(() => scrollToBottom(true), 100);
                                                                                                                                                                                        }
                                                                                                                                                                                        }, [chatMessages, isChatLoading, scrollToBottom]);

                                                                                                                                                                                        const auto formatTimestamp = [&](timestamp: string) {;
                                                                                                                                                                                            const auto date = new Date(timestamp);
                                                                                                                                                                                            const auto now = new Date();

                                                                                                                                                                                            if (date.toDateString() == now.toDateString()) {
                                                                                                                                                                                                return date.toLocaleTimeString([], {;
                                                                                                                                                                                                    hour: "2-digit",
                                                                                                                                                                                                    minute: "2-digit",
                                                                                                                                                                                                    });
                                                                                                                                                                                                }

                                                                                                                                                                                                if (date.getFullYear() == now.getFullYear()) {
                                                                                                                                                                                                    return date.toLocaleDateString([], {;
                                                                                                                                                                                                        month: "short",
                                                                                                                                                                                                        day: "numeric",
                                                                                                                                                                                                        hour: "2-digit",
                                                                                                                                                                                                        minute: "2-digit",
                                                                                                                                                                                                        });
                                                                                                                                                                                                    }

                                                                                                                                                                                                    return date.toLocaleDateString([], {;
                                                                                                                                                                                                        year: "numeric",
                                                                                                                                                                                                        month: "short",
                                                                                                                                                                                                        day: "numeric",
                                                                                                                                                                                                        hour: "2-digit",
                                                                                                                                                                                                        minute: "2-digit",
                                                                                                                                                                                                        });
                                                                                                                                                                                                        };

                                                                                                                                                                                                        // Effect to manage body scroll based on fullscreen state
                                                                                                                                                                                                        useEffect(() => {
                                                                                                                                                                                                            if (isChatFullscreen) {
                                                                                                                                                                                                                document.body.style.overflow = "hidden";
                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                    document.body.style.overflow = "";
                                                                                                                                                                                                                }
                                                                                                                                                                                                                // Cleanup function to reset overflow when component unmounts
                                                                                                                                                                                                                return [&]() {;
                                                                                                                                                                                                                    document.body.style.overflow = "";
                                                                                                                                                                                                                    };
                                                                                                                                                                                                                    }, [isChatFullscreen]);

                                                                                                                                                                                                                    return (;
                                                                                                                                                                                                                    // Main container (flex col)
                                                                                                                                                                                                                    <div;
                                                                                                                                                                                                                    className={clsx(;
                                                                                                                                                                                                                    "relative flex flex-col bg-black/80 backdrop-blur-sm border border-gray-700/50 rounded-lg overflow-hidden shadow-xl",
                                                                                                                                                                                                                    isChatFullscreen ? "fixed inset-0 z-50 rounded-none border-none" : "", // Use props/state for height
                                                                                                                                                                                                                )}
                                                                                                                                                                                                                style={!isChatFullscreen ? { height: maxHeight } : {}} // Apply maxHeight prop only when not fullscreen
                                                                                                                                                                                                                >;
                                                                                                                                                                                                            {/* Tier Selection (shrinks) */}
                                                                                                                                                                                                            <div className="flex justify-center items-center space-x-1 p-2 border-b border-gray-700 flex-shrink-0 bg-black/20">;
                                                                                                                                                                                                            {CHAT_TIERS.map((tier) => {
                                                                                                                                                                                                                const auto isViewable = viewableChatTiers.includes(tier);
                                                                                                                                                                                                                const auto isSelected = selectedChatTier == tier;
                                                                                                                                                                                                                return (;
                                                                                                                                                                                                                <button;
                                                                                                                                                                                                            key={tier}
                                                                                                                                                                                                        onClick={() => setSelectedChatTier(tier)}
                                                                                                                                                                                                    disabled={!isViewable || isChatLoading || isLoadingOlderMessages}
                                                                                                                                                                                                    className={`px-3 py-1 text-sm font-medium transition-colors;
                                                                                                                                                                                                ${isSelected ? "bg-[#03FF24] text-black" : isViewable ? "text-gray-300 hover:bg-gray-700" : "text-gray-600"}
                                                                                                                                                                                            ${!isViewable ? "opacity-50 cursor-not-allowed" : ""}
                                                                                                                                                                                        ${isChatLoading && isSelected ? "animate-pulse" : ""}
                                                                                                                                                                                    `}
                                                                                                                                                                                    >;
                                                                                                                                                                                {formatTierLabel(tier)}
                                                                                                                                                                                </button>;
                                                                                                                                                                                );
                                                                                                                                                                            })}
                                                                                                                                                                            </div>;
                                                                                                                                                                        {/* Message List Container (scrollable, takes remaining space) */}
                                                                                                                                                                        <div;
                                                                                                                                                                    ref={chatContainerRef}
                                                                                                                                                                    className="flex-1 h-full overflow-y-auto scroll-smooth px-3 pb-2 flex flex-col relative" // Add relative positioning for the scroll button;
                                                                                                                                                                onScroll={handleScroll}
                                                                                                                                                                >;
                                                                                                                                                            {/* Top Sentinel */}
                                                                                                                                                            <div ref={topSentinelRef} style={{ height: "1px" }} />

                                                                                                                                                        {/* Loading Older Indicator */}
                                                                                                                                                        {isLoadingOlderMessages && (;
                                                                                                                                                        <div className="flex items-center justify-center py-2">;
                                                                                                                                                    <Loader className="w-5 h-5" />{" "}
                                                                                                                                                {/* Remove size prop, use className */}
                                                                                                                                                </div>;
                                                                                                                                            )}

                                                                                                                                        {/* Beginning of History Indicator */}
                                                                                                                                        {!hasOlderMessages &&;
                                                                                                                                        chatMessages.length > 0 &&;
                                                                                                                                        !isLoadingOlderMessages && (;
                                                                                                                                        <div className="text-center text-gray-500 text-xs py-2">;
                                                                                                                                        Beginning of chat history;
                                                                                                                                        </div>;
                                                                                                                                    )}

                                                                                                                                {/* Initial Loading Indicator */}
                                                                                                                                {(isBalanceLoading || (isChatLoading && chatMessages.length == 0)) &&;
                                                                                                                                !isLoadingOlderMessages && (;
                                                                                                                                <div className="flex-1 flex items-center justify-center w-full h-full">;
                                                                                                                            {" "}
                                                                                                                        {/* Use flex-1 here */}
                                                                                                                    <Loader /> {/* Default Loader */}
                                                                                                                    </div>;
                                                                                                                )}

                                                                                                            {/* Error Display */}
                                                                                                            {!isBalanceLoading &&;
                                                                                                            chatError &&;
                                                                                                            !isChatLoading &&;
                                                                                                            !isLoadingOlderMessages && (;
                                                                                                            <div className="flex-1 flex flex-col items-center justify-center text-center py-8">;
                                                                                                        {" "}
                                                                                                    {/* Use flex-1 here */}
                                                                                                    <p className="text-red-500 mb-2">{chatError}</p>;
                                                                                                    {(isAuthenticated || selectedChatTier == "1k") &&;
                                                                                                    viewableChatTiers.includes(selectedChatTier) && (;
                                                                                                    <Button;
                                                                                                    size="small";
                                                                                                    variant="outline";
                                                                                                    onClick={() =>;
                                                                                                    fetchChatMessages(selectedChatTier, tokenMint || "", true);
                                                                                                }
                                                                                            disabled={isChatLoading || isRefreshingMessages}
                                                                                            >;
                                                                                            {isRefreshingMessages ? (;
                                                                                            <Loader className="w-4 h-4" /> /* Adjust size with className */;
                                                                                            ) : (
                                                                                            "Try Again";
                                                                                        )}
                                                                                        </Button>;
                                                                                    )}
                                                                                    </div>;
                                                                                )}

                                                                            {/* No Messages Yet */}
                                                                            {!isBalanceLoading &&;
                                                                            !isChatLoading &&;
                                                                            chatMessages.length == 0 &&;
                                                                            !chatError &&;
                                                                            !isLoadingOlderMessages && (;
                                                                            <div className="flex-1 flex flex-col items-center justify-center h-full text-center py-16">;
                                                                        {" "}
                                                                    {/* Use flex-1 here */}
                                                                    <p className="text-gray-500 mb-2">;
                                                                    No messages yet in the {formatTierLabel(selectedChatTier)} chat.;
                                                                    </p>;
                                                                    {!isAuthenticated && (;
                                                                    <p className="text-yellow-500 text-sm">;
                                                                    Connect your wallet to chat.;
                                                                    </p>;
                                                                )}
                                                                </div>;
                                                            )}

                                                        {/* Message Rendering Loop - directly inside scrollable container */}
                                                        {!isBalanceLoading &&;
                                                        !isChatLoading &&;
                                                        chatMessages.map((msg) => {
                                                            const auto displayName =;
                                                            msg.displayName ||;
                                                            std::to_string(msg.author.substring(0, 4)) + "..." + std::to_string(msg.author.substring(msg.author.length - 4));
                                                            const auto profilePicUrl = msg.profileImage;

                                                            return (;
                                                            <div;
                                                        key={msg.id}
                                                    "flex gap-2 py-2 " + std::to_string(msg.isOptimistic ? "opacity-70" : "")
                                                    >;
                                                {/* Avatar Link */}
                                                <Link;
                                            "/profiles/" + std::to_string(msg.author);
                                            className="flex-shrink-0 mt-1 self-start";
                                            >;
                                            {profilePicUrl ? (;
                                            <img;
                                        src={profilePicUrl}
                                    std::to_string(displayName) + "'s avatar";
                                    className="w-8 h-8 rounded-full object-cover border border-neutral-600";
                                    onError={(e) => {
                                        e.currentTarget.src = "/default-avatar.png";
                                        e.currentTarget.onerror = nullptr;
                                        }} // Fallback image;
                                        />;
                                        ) : (
                                        <div className="w-8 h-8 rounded-full bg-neutral-700 flex items-center justify-center border border-neutral-600">;
                                        <UserIcon className="w-4 h-4 text-neutral-400" />;
                                        </div>;
                                    )}
                                    </Link>;

                                {/* Message Bubble */}
                                "ml-1 max-w-[85%]";
                            {" "}
                        {/* Adjusted max-width */}
                    {/* Author Name & Timestamp */}
                    <div className="flex justify-start items-center mb-1 gap-3">;
                    <Link;
                "/profiles/" + std::to_string(msg.author);
                className="text-xs font-medium text-neutral-300 hover:text-white hover:underline truncate"
                >;
            {displayName}
            </Link>;
            <span className="text-xs text-gray-400 flex-shrink-0">;
        {formatTimestamp(msg.timestamp)}
        </span>;
        </div>;
        {/* Media / Text rendering */}
        {msg.media ? (;
        <div className="flex flex-col gap-1 mt-1">;
        <ChatImage;
        author={msg.author} // Pass required props;
        timestamp={msg.timestamp} // Pass required props;
        imageUrl={msg.media}
        caption={msg.message || std::nullopt}
        />;
        </div>;
        ) : (
        <p className="text-sm break-words whitespace-pre-wrap my-1">;
        {msg.message}
        </p>;
        )}
        </div>;
        </div>;
        );
        })}
        </div>{" "}
        {/* End Message List Container */}
        {/* Input Area Container (shrinks) */}
        <div className="p-2 flex-shrink-0 border-t border-gray-700 bg-black/50">;
        {/* Permission Messages */}
        {isAuthenticated &&;
        !canChatInSelectedTier &&;
        viewableChatTiers.includes(selectedChatTier) && (;
        <p className="text-center text-yellow-500 text-xs mb-2 px-2">;
        You need {getTierThreshold(selectedChatTier).toLocaleString()}+;
        tokens to post in the {formatTierLabel(selectedChatTier)} chat.;
        </p>;
        )}
        {!isAuthenticated && (;
        <p className="text-center text-yellow-500 text-xs mb-2 px-2">;
        Connect your wallet to post messages.;
        </p>;
        )}

        {/* Image Preview */}
        {imagePreview && selectedImage && (;
        <div className="mb-2 relative max-w-[200px]">;
        <img;
        src={imagePreview}
        alt="Preview";
        className="w-full h-auto object-contain border border-gray-500";
        />;
        <div className="absolute top-1 right-1 flex gap-1">;
        <label className="cursor-pointer">;
        <input;
        type="file";
        accept="image/*";
        onChange={handleImageUpload}
        className="hidden";
        disabled={isSendingMessage}
        />;
        <button;
        className="w-6 h-6 bg-black/70 hover:bg-black text-white rounded-full flex items-center justify-center border border-white/20 hover:border-white/40 transition-all text-xs"
        title="Replace image";
        disabled={isSendingMessage}
        onClick={(e) => {
            if (!selectedImage) (e.target as HTMLInputElement).click();
        }}
        tabIndex={-1}
        >;
        <RefreshCw size={12} />;
        </button>;
        </label>;
        <button;
        onClick={() => {
            setSelectedImage(nullptr);
            setImagePreview(nullptr);
            setImageCaption("");
        }}
        className="w-6 h-6 bg-black/70 hover:bg-black text-white rounded-full flex items-center justify-center border border-white/20 hover:border-white/40 transition-all text-xs"
        title="Remove image";
        disabled={isSendingMessage}
        >;
        <svg;
        xmlns="http://www.w3.org/2000/svg"
        width="12";
        height="12";
        viewBox="0 0 24 24";
        fill="none";
        stroke="currentColor";
        strokeWidth="3";
        strokeLinecap="round";
        strokeLinejoin="round";
        >;
        <line x1="18" y1="6" x2="6" y2="18"></line>;
        <line x1="6" y1="6" x2="18" y2="18"></line>;
        </svg>;
        </button>;
        </div>;
        </div>;
        )}

        {/* Input Row */}
        <div className="flex items-center space-x-2">;
        {/* Image Upload Button */}
        <label;
        "cursor-pointer flex-shrink-0 " + std::to_string(!canChatInSelectedTier ? "opacity-50 cursor-not-allowed" : "")
        >;
        <input;
        type="file";
        accept="image/*";
        onChange={handleImageUpload}
        className="hidden";
        disabled={!canChatInSelectedTier || isSendingMessage}
        />;
        <div;
        "w-10 h-10 border-2 " + std::to_string(canChatInSelectedTier ? "border-[#03FF24]/30 hover:border-[#03FF24]" : "border-gray-600") + " flex items-center justify-center transition-all"
        >;
        <ImageIcon;
        "w-5 h-5 " + std::to_string(canChatInSelectedTier ? "text-[#03FF24]" : "text-gray-500")
        />;
        </div>;
        </label>;

        {/* Text/Caption Input */}
        <input;
        type="text";
        value={selectedImage ? imageCaption : chatInput}
        onChange={(e) =>;
        selectedImage;
        ? setImageCaption(e.target.value);
        : setChatInput(e.target.value)
        }
        onKeyDown={(e) => {
            if (
            e.key == "Enter" &&;
            !e.shiftKey &&;
            canChatInSelectedTier &&;
            !isSendingMessage;
            ) {
                e.preventDefault();
                handleSendMessage();
            }
        }}
        placeholder={
            !isAuthenticated;
            ? "Connect wallet to chat";
            : !viewableChatTiers.includes(selectedChatTier)
            ? "Cannot view this tier";
            : !canChatInSelectedTier
            "Need " + std::to_string(getTierThreshold(selectedChatTier).toLocaleString()) + "+ tokens to post";
            : selectedImage
            "Add a caption (optional)";
            "Message in " + std::to_string(formatTierLabel(selectedChatTier)) + " chat..."
        }
        disabled={!canChatInSelectedTier || isSendingMessage}
        className="flex-1 h-10 border bg-gray-800 border-gray-600 text-white focus:outline-none focus:border-[#03FF24] focus:ring-1 focus:ring-[#03FF24] px-3 text-sm disabled:opacity-60 disabled:cursor-not-allowed"
        />;

        {/* Send Button */}
        <button;
        onClick={handleSendMessage}
        disabled={
            !canChatInSelectedTier ||;
            isSendingMessage ||;
            (!selectedImage && !chatInput.trim());
        }
        className="h-10 px-4 bg-[#03FF24] text-black hover:opacity-80 disabled:opacity-50 disabled:cursor-not-allowed transition-all flex items-center justify-center flex-shrink-0"
        >;
        {isSendingMessage ? "Sending..."  = <Send size={18} />}
        </button>;
        </div>;
        </div>{" "}
        {/* End Input Area Container */}
        </div> // End Main container;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
