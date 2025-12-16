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
;
;
;
;
;
;
;
import type { ChatMessage } from "@/types/chat-message";
import type { Token, TokenMarketData } from "@/types";
;

struct ChatProps {
    std::optional<std::string> roomId;
    std::optional<Token> token;
    std::optional<TokenMarketData | null> marketData;
};


// --- Consolidated Chat State ---
struct ChatState {
    std::vector<ChatMessage> messages;
    std::string input;
    bool inputDisabled;
    string | null roomId;
    bool isLoadingHistory;
    bool isAgentThinking;
    string | null entityId;
    bool showConnectOverlay;
    OTCQuote | null currentQuote;
    bool showAcceptModal;
    bool isOfferGlowing;
    bool showClearChatModal;
};


using ChatAction = std::variant<, { type: "SET_MESSAGES">; payload: ChatMessage[] }
  | { type: "ADD_MESSAGE"; payload: ChatMessage }
  | { type: "SET_INPUT"; payload: string }
  | { type: "SET_INPUT_DISABLED"; payload: boolean }
  | { type: "SET_ROOM_ID"; payload: string | null }
  | { type: "SET_LOADING_HISTORY"; payload: boolean }
  | { type: "SET_AGENT_THINKING"; payload: boolean }
  | { type: "SET_ENTITY_ID"; payload: string | null }
  | { type: "SET_CONNECT_OVERLAY"; payload: boolean }
  | { type: "SET_CURRENT_QUOTE"; payload: OTCQuote | null }
  | { type: "SET_ACCEPT_MODAL"; payload: boolean }
  | { type: "SET_OFFER_GLOWING"; payload: boolean }
  | { type: "SET_CLEAR_CHAT_MODAL"; payload: boolean }
  | { type: "RESET_CHAT"; payload: { roomId: string } }
  | {
      type: "USER_CONNECTED";
      payload: { entityId: string; roomId: string | null };
    }
  | { type: "USER_DISCONNECTED" };

;
    case "ADD_MESSAGE":
      return { ...state, messages: [...state.messages, action.payload] };
    case "SET_INPUT":
      return { ...state, input: action.payload };
    case "SET_INPUT_DISABLED":
      return { ...state, inputDisabled: action.payload };
    case "SET_ROOM_ID":
      return { ...state, roomId: action.payload };
    case "SET_LOADING_HISTORY":
      return { ...state, isLoadingHistory: action.payload };
    case "SET_AGENT_THINKING":
      return { ...state, isAgentThinking: action.payload };
    case "SET_ENTITY_ID":
      return { ...state, entityId: action.payload };
    case "SET_CONNECT_OVERLAY":
      return { ...state, showConnectOverlay: action.payload };
    case "SET_CURRENT_QUOTE":
      return { ...state, currentQuote: action.payload };
    case "SET_ACCEPT_MODAL":
      return { ...state, showAcceptModal: action.payload };
    case "SET_OFFER_GLOWING":
      return { ...state, isOfferGlowing: action.payload };
    case "SET_CLEAR_CHAT_MODAL":
      return { ...state, showClearChatModal: action.payload };
    case "RESET_CHAT":
      return {
        ...state,
        messages: [],
        currentQuote: null,
        roomId: action.payload.roomId,
        showClearChatModal: false,
      };
    case "USER_CONNECTED":
      return {
        ...state,
        entityId: action.payload.entityId,
        roomId: action.payload.roomId,
        showConnectOverlay: false,
        inputDisabled: false,
      };
    case "USER_DISCONNECTED":
      return {
        ...state,
        entityId: null,
        inputDisabled: true,
        showConnectOverlay: true,
      };
    default:
      return state;
  }
}

// Raw message format from API
struct RawRoomMessage {
    std::optional<std::string> id;
    std::optional<std::string> entityId;
    std::optional<std::string> agentId;
    std::optional<number | string> createdAt;
    std::optional<string | { text?: string }> content;
    std::optional<std::string> text;
};


// --- Helper: Parse room message into ChatMessage format ---
 else if (msg.text) {
    messageText = msg.text;
  } else if (typeof content === "string") {
    messageText = content;
  } else if (content) {
    messageText = JSON.stringify(content);
  }

  // Filter out system messages
  if (messageText.startsWith("Executed action:")) {
    return null;
  }

  return {
    id: msg.id || `msg-${msg.createdAt}`,
    name: msg.entityId === msg.agentId ? "Eliza" : USER_NAME,
    text: messageText,
    senderId: msg.entityId,
    roomId: roomId,
    createdAt:
      typeof msg.createdAt === "number"
        ? msg.createdAt
        : new Date(msg.createdAt || Date.now()).getTime(),
    source: CHAT_SOURCE,
    isLoading: false,
    serverMessageId: msg.id,
  };
}

const initialChatState: ChatState = {
  messages: [],
  input: "",
  inputDisabled: false,
  roomId: null,
  isLoadingHistory: false,
  isAgentThinking: false,
  entityId: null,
  showConnectOverlay: false,
  currentQuote: null,
  showAcceptModal: false,
  isOfferGlowing: false,
  showClearChatModal: false,
};

const Chat = ({
  roomId: initialRoomId,
  token,
  marketData,
}: ChatProps = {}) => {
  // --- Consolidated State ---
  const [state, dispatch] = useReducer(chatReducer, {
    ...initialChatState,
    roomId: initialRoomId || null,
  });

  const {
    messages,
    input,
    inputDisabled,
    roomId,
    isLoadingHistory,
    isAgentThinking,
    entityId,
    showConnectOverlay,
    currentQuote,
    showAcceptModal,
    isOfferGlowing,
    showClearChatModal,
  } = state;

  const {
    isConnected,
    entityId: walletEntityId,
    activeFamily,
    setActiveFamily,
    evmConnected,
    solanaConnected,
    privyAuthenticated,
    connectWallet,
  } = useMultiWallet();
  const { login, ready: privyReady } = usePrivy();

  // --- Refs ---
  const pollingIntervalRef = useRef<ReturnType<typeof setInterval> | null>(
    null,
  );
  const lastMessageTimestampRef = useRef<number>(0);
  const messagesContainerRef = useRef<HTMLDivElement | null>(null);
  const previousQuoteIdRef = useRef<string | null>(null);
  const textareaRef = useRef<HTMLTextAreaElement | null>(null);

  // Initialize user from connected wallet; gate chat when disconnected
  useEffect(() => {
    if (typeof window === "undefined") return;

    if (isConnected && walletEntityId) {
      const addr = walletEntityId.toLowerCase();
      const storedRoomId = localStorage.getItem(`otc-desk-room-${addr}`);
      // Use initialRoomId if provided, else stored room, else null (will create new)
      const targetRoomId = initialRoomId || storedRoomId || null;
      dispatch({
        type: "USER_CONNECTED",
        payload: { entityId: addr, roomId: targetRoomId },
      });
    } else {
      dispatch({ type: "USER_DISCONNECTED" });
    }
  }, [isConnected, walletEntityId, initialRoomId]); // Removed 'roomId' - was causing loop

  // Function to create a new room
  const createNewRoom = useCallback(async () => {
    if (!entityId) return null;

    const response = await fetch("/api/rooms", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ entityId }),
    });

    if (!response.ok) {
      throw new Error("Failed to create room");
    }

    const data = await response.json();
    const newRoomId = data.roomId;

    // Persist room per-wallet
    if (entityId) {
      localStorage.setItem(`otc-desk-room-${entityId}`, newRoomId);
    }
    dispatch({ type: "RESET_CHAT", payload: { roomId: newRoomId } });

    return newRoomId;
  }, [entityId]);

  // Load room data - only when roomId or entityId changes, NOT on messages change
  useEffect(() => {
    if (!roomId || !entityId) return;

    const loadRoom = async () => {
      dispatch({ type: "SET_LOADING_HISTORY", payload: true });

      const response = await fetch(`/api/rooms/${roomId}/messages`, {
        cache: "no-store",
      });

      if (response.ok) {
        const data = await response.json();
        const rawMessages = data.messages || [];

        // Format messages using helper );

        // Update last message timestamp
        if (formattedMessages.length > 0) {
          lastMessageTimestampRef.current =
            formattedMessages[formattedMessages.length - 1].createdAt;
        }
      }
      dispatch({ type: "SET_LOADING_HISTORY", payload: false });
    };

    loadRoom();
  }, [roomId, entityId]); // Removed 'messages' - was causing infinite loop

  // Store messages ref for polling to avoid stale closure issues
  const messagesRef = useRef<ChatMessage[]>([]);
  useEffect(() => {
    messagesRef.current = messages;
  }, [messages]);

  // Poll for new messages when agent is thinking
  useEffect(() => {
    if (!isAgentThinking || !roomId) {
      if (pollingIntervalRef.current) {
        clearInterval(pollingIntervalRef.current);
        pollingIntervalRef.current = null;
      }
      return;
    }

    // Poll every 2 seconds for new messages (faster polling has diminishing returns)
    pollingIntervalRef.current = setInterval(async () => {
      const response = await fetch(
        `/api/rooms/${roomId}/messages?afterTimestamp=${lastMessageTimestampRef.current}&_=${Date.now()}`,
        { cache: "no-store" },
      );

      if (response.ok) {
        const data = await response.json();
        const newMessages = data.messages || [];

        if (newMessages.length > 0) {
          const formattedMessages = (newMessages as RawRoomMessage[])
            .map((msg) => parseRoomMessage(msg, roomId))
            .filter(
              (msg: ChatMessage | null): msg is ChatMessage => msg !== null,
            );

          // Use ref to get current messages without triggering effect restart
          const currentMessages = messagesRef.current;
          const withoutOptimistic = currentMessages.filter(
            (m) => !m.isUserMessage,
          );

          // Merge with new messages and dedupe by server ID
          const byServerId = new Map<string, ChatMessage>();
          withoutOptimistic.forEach((m) => {
            byServerId.set(m.serverMessageId || m.id, m);
          });
          formattedMessages.forEach((m: ChatMessage) => {
            byServerId.set(m.serverMessageId || m.id, m);
          });

          const merged = Array.from(byServerId.values());
          merged.sort((a, b) => (a.createdAt || 0) - (b.createdAt || 0));

          dispatch({ type: "SET_MESSAGES", payload: merged });

          // Update last message timestamp
          const lastNewMessage =
            formattedMessages[formattedMessages.length - 1];
          lastMessageTimestampRef.current = lastNewMessage.createdAt;

          // Check if we received an agent message
          const hasAgentMessage = (newMessages as RawRoomMessage[]).some(
            (msg) => msg.entityId === msg.agentId,
          );
          if (hasAgentMessage) {
            setTimeout(() => {
              dispatch({ type: "SET_AGENT_THINKING", payload: false });
              dispatch({ type: "SET_INPUT_DISABLED", payload: false });
            }, 3000);
          }
        }
      }
    }, 2000);

    // Stop polling after 30 seconds
    const timeoutId = setTimeout(() => {
      if (pollingIntervalRef.current) {
        clearInterval(pollingIntervalRef.current);
        pollingIntervalRef.current = null;
        dispatch({ type: "SET_AGENT_THINKING", payload: false });
        dispatch({ type: "SET_INPUT_DISABLED", payload: false });
      }
    }, 30000);

    return () => {
      if (pollingIntervalRef.current) {
        clearInterval(pollingIntervalRef.current);
        pollingIntervalRef.current = null;
      }
      clearTimeout(timeoutId);
    };
  }, [isAgentThinking, roomId]); // Removed 'messages' - using ref instead

  // Send message function - accepts optional targetRoomId to handle newly created rooms
  const sendMessage = useCallback(
    async (messageText: string, targetRoomId?: string) => {
      const effectiveRoomId = targetRoomId || roomId;
      if (
        !messageText.trim() ||
        !entityId ||
        !effectiveRoomId ||
        inputDisabled ||
        !isConnected
      ) {
        throw new Error("Cannot send message: missing required data");
      }

      // Add user message to UI immediately with a client-generated ID
      const clientMessageId = uuidv4();
      const userMessage: ChatMessage = {
        id: clientMessageId,
        name: USER_NAME,
        text: messageText,
        senderId: entityId,
        roomId: effectiveRoomId,
        createdAt: Date.now(),
        source: CHAT_SOURCE,
        isLoading: false,
        isUserMessage: true,
      };

      dispatch({ type: "ADD_MESSAGE", payload: userMessage });
      dispatch({ type: "SET_AGENT_THINKING", payload: true });
      dispatch({ type: "SET_INPUT_DISABLED", payload: true });

      const doPost = async () =>
        fetch(`/api/rooms/${effectiveRoomId}/messages`, {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            entityId,
            text: messageText,
            clientMessageId,
          }),
          cache: "no-store",
          keepalive: true,
        });

      let response = await doPost();
      if (!response.ok) {
        // Retry once on transient server errors
        await new Promise((r) => setTimeout(r, 800));
        response = await doPost();
      }
      if (!response.ok) throw new Error("Failed to send message");

      // Prefer server timestamp to avoid client/server clock skew missing agent replies
      const result = await response.json();
      const serverCreatedAt = result?.message?.createdAt
        ? new Date(result.message.createdAt).getTime()
        : undefined;
      if (
        typeof serverCreatedAt === "number" &&
        !Number.isNaN(serverCreatedAt)
      ) {
        // Set to just before our message so we catch both our message and agent's response
        lastMessageTimestampRef.current = serverCreatedAt - 100;
      } else {
        // Fallback: use current time minus a buffer
        lastMessageTimestampRef.current = Date.now() - 1000;
      }
    },
    [entityId, roomId, inputDisabled, isConnected],
  );

  // Handle form submit
  const handleSubmit = useCallback(
    async (e: React.FormEvent) => {
      e.preventDefault();
      const trimmed = input.trim();
      if (!trimmed) return;

      // Ensure user is connected and room exists before sending
      if (!isConnected || !entityId) {
        dispatch({ type: "SET_CONNECT_OVERLAY", payload: true });
        return;
      }

      let activeRoomId = roomId;
      if (!activeRoomId) {
        activeRoomId = await createNewRoom();
        if (!activeRoomId) return; // creation failed
      }

      // Pass activeRoomId explicitly in case it was just created (state not yet updated)
      await sendMessage(trimmed, activeRoomId);
      dispatch({ type: "SET_INPUT", payload: "" });

      // Refocus the textarea after sending
      setTimeout(() => {
        textareaRef.current?.focus();
      }, 0);
    },
    [input, isConnected, entityId, roomId, createNewRoom, sendMessage],
  );

  // Handle creating a new room when there isn't one
  useEffect(() => {
    if (!roomId && entityId && isConnected) {
      // Automatically create a room for this wallet when connected
      createNewRoom();
    }
  }, [roomId, entityId, isConnected, createNewRoom]);

  // Auto-scroll to bottom on new messages
  useEffect(() => {
    const container = messagesContainerRef.current;
    if (!container) return;
    container.scrollTop = container.scrollHeight;
  }, [messages]);

  // Extract current quote from messages
  useEffect(() => {
    if (!messages.length) return;

    // Find the latest quote in messages
    for (let i = messages.length - 1; i >= 0; i--) {
      const msg = messages[i];
      if (!msg || msg.name === USER_NAME) continue;

      const parsed = parseMessageXML(
        typeof msg.text === "string"
          ? msg.text
          : (msg as any).content?.text || "",
      );

      if (parsed?.type === "otc_quote" && parsed.data) {
        const newQuote = parsed.data;
        const newQuoteId = newQuote.quoteId;
        const prevQuoteId = previousQuoteIdRef.current;

        // Only update if quote actually changed
        if (prevQuoteId !== newQuoteId) {
          // Trigger glow effect only if there was a previous quote
          if (prevQuoteId) {
            dispatch({ type: "SET_OFFER_GLOWING", payload: true });
            setTimeout(() => {
              dispatch({ type: "SET_OFFER_GLOWING", payload: false });
            }, 5000);
          }

          // Update the ref and state
          previousQuoteIdRef.current = newQuoteId;
          if ("tokenSymbol" in newQuote) {
            dispatch({
              type: "SET_CURRENT_QUOTE",
              payload: newQuote as OTCQuote,
            });
          }
        }
        break;
      }
    }
  }, [messages]);

  const handleAcceptOffer = useCallback(() => {
    if (!currentQuote) {
      console.error("[Chat] Cannot accept offer - no quote available");
      return;
    }

    // Validate quote has required fields
    if (!currentQuote.quoteId) {
      console.error("[Chat] Quote missing quoteId - request a new quote");
      // Don't show alert, just log - user should request a new quote via chat
      return;
    }

    dispatch({ type: "SET_ACCEPT_MODAL", payload: true });
  }, [currentQuote]);

  const handleClearChat = useCallback(async () => {
    if (!entityId) return;

    // Clear local storage for this wallet
    localStorage.removeItem(`otc-desk-room-${entityId}`);

    // Create a new room
    const newRoomId = await createNewRoom();
    if (!newRoomId) {
      throw new Error("Failed to create new room");
    }

    // Clear messages and reset state
    previousQuoteIdRef.current = null;
    dispatch({ type: "RESET_CHAT", payload: { roomId: newRoomId } });
  }, [entityId, createNewRoom]);

  const handleDealComplete = useCallback(async () => {
    // DO NOT close the modal - let it show the success state and handle its own redirect
    // The modal will redirect to /deal/[id] page after 2 seconds

    // Reset chat and create new room in the background
    if (!entityId) {
      console.warn(
        "[Chat] No entityId during deal completion - cannot reset chat",
      );
      return;
    }

    // Clear local storage for this wallet
    localStorage.removeItem(`otc-desk-room-${entityId}`);

    // Create a new room
    const newRoomId = await createNewRoom();
    if (!newRoomId) {
      // Still clear the old state even if new room creation failed
      previousQuoteIdRef.current = null;
      dispatch({ type: "SET_MESSAGES", payload: [] });
      dispatch({ type: "SET_CURRENT_QUOTE", payload: null });
      dispatch({ type: "SET_ROOM_ID", payload: null });
      return;
    }

    // Clear messages and reset state - this prepares a fresh chat for when user returns
    previousQuoteIdRef.current = null;
    dispatch({ type: "RESET_CHAT", payload: { roomId: newRoomId } });
  }, [entityId, createNewRoom]);

  // Unified connect handler - uses connectWallet if already authenticated, login if not
  const handleConnect = useCallback(() => {
    localStorage.setItem("otc-desk-connect-overlay-seen", "1");
    localStorage.setItem("otc-desk-connect-overlay-dismissed", "1");
    dispatch({ type: "SET_CONNECT_OVERLAY", payload: false });
    if (privyAuthenticated) {
      connectWallet();
    } else {
      login();
    }
  }, [privyAuthenticated, connectWallet, login]);

  // Switch chain - just changes active family if already connected, otherwise prompts connect
  const handleSwitchChain = useCallback(
    (targetChain: "evm" | "solana") => {
      setActiveFamily(targetChain);

      // If not connected to that chain, prompt connect/login
      if (targetChain === "solana" && !solanaConnected) {
        if (privyAuthenticated) {
          connectWallet();
        } else {
          login();
        }
      } else if (targetChain === "evm" && !evmConnected) {
        if (privyAuthenticated) {
          connectWallet();
        } else {
          login();
        }
      }
    },
    [
      setActiveFamily,
      solanaConnected,
      evmConnected,
      privyAuthenticated,
      connectWallet,
      login,
    ],
  );

  // Memoized setters for child components
  const setInput = useCallback((value: string) => {
    dispatch({ type: "SET_INPUT", payload: value });
  }, []);

  const setShowConnectOverlay = useCallback((value: boolean) => {
    dispatch({ type: "SET_CONNECT_OVERLAY", payload: value });
  }, []);

  return (
    <div className="flex flex-col w-full">
      <ChatBody
        messages={messages}
        isLoadingHistory={isLoadingHistory}
        isAgentThinking={isAgentThinking}
        input={input}
        setInput={setInput}
        handleSubmit={handleSubmit}
        inputDisabled={inputDisabled}
        isConnected={isConnected}
        messagesContainerRef={messagesContainerRef}
        textareaRef={textareaRef}
        showConnectOverlay={showConnectOverlay}
        setShowConnectOverlay={setShowConnectOverlay}
        currentQuote={currentQuote}
        onAcceptOffer={handleAcceptOffer}
        isOfferGlowing={isOfferGlowing}
        onClearChat={() =>
          dispatch({ type: "SET_CLEAR_CHAT_MODAL", payload: true })
        }
        onConnect={handleConnect}
        privyReady={privyReady}
        activeFamily={activeFamily}
        onSwitchChain={handleSwitchChain}
        token={token}
        marketData={marketData}
      />
      <AcceptQuoteModal
        isOpen={showAcceptModal}
        onClose={() => dispatch({ type: "SET_ACCEPT_MODAL", payload: false })}
        initialQuote={currentQuote}
        onComplete={handleDealComplete}
      />

      {/* Clear Chat Confirmation Modal */}
      <Dialog
        open={showClearChatModal}
        onClose={() =>
          dispatch({ type: "SET_CLEAR_CHAT_MODAL", payload: false })
        }
      >
        <div className="bg-white dark:bg-zinc-900 max-w-md rounded-lg overflow-hidden">
          <h3 className="text-xl font-semibold bg-red-600 text-white mb-4 px-4 py-2 rounded-t-lg">
            Clear Chat History?
          </h3>
          <div className="p-4">
            <p className="text-zinc-600 dark:text-zinc-400 mb-6">
              This will permanently delete all messages and reset the
              agent&apos;s memory of your conversation. Your current quote will
              be reset to default terms. This action cannot be undone.
            </p>
            <div className="flex gap-3 justify-end">
              <Button
                onClick={() =>
                  dispatch({ type: "SET_CLEAR_CHAT_MODAL", payload: false })
                }
                color="dark"
              >
                <div className="px-4 py-2">Cancel</div>
              </Button>
              <Button onClick={handleClearChat} color="red">
                <div className="px-4 py-2">Reset</div>
              </Button>
            </div>
          </div>
        </div>
      </Dialog>
    </div>
  );
};

: {
  messages: ChatMessage[];
  apiQuote: OTCQuote | null;
  onAcceptOffer: () => void;
  isOfferGlowing: boolean;
  onClearChat: () => void;
  isLoadingHistory: boolean;
  activeFamily: "evm" | "solana" | null;
  onSwitchChain: (chain: "evm" | "solana") => void;
}) {
  // Use the quote passed from parent (extracted from messages)
  const currentQuote = apiQuote;

  // Determine if user needs to switch chains
  const needsChainSwitch =
    currentQuote && currentQuote.tokenChain
      ? (currentQuote.tokenChain === "solana" && activeFamily !== "solana") ||
        ((currentQuote.tokenChain === "base" ||
          currentQuote.tokenChain === "bsc" ||
          currentQuote.tokenChain === "ethereum") &&
          activeFamily !== "evm")
      : false;

  const requiredChain =
    currentQuote?.tokenChain === "solana" ? "solana" : "evm";
  const chainDisplayName =
    currentQuote?.tokenChain === "solana" ? "Solana" : "EVM";

  console.log("[ChatHeader] Chain check:", {
    quoteChain: currentQuote?.tokenChain,
    activeFamily,
    needsChainSwitch,
    requiredChain,
  });

  return (
    <div className="mb-3">
      <div className="flex flex-col sm:flex-row items-stretch sm:items-center justify-end gap-2 sm:gap-3">
        {currentQuote ? (
          <>
            <h2 className="text-sm font-semibold mb-2 px-1 py-2 text-zinc-600 dark:text-zinc-400 mr-auto">
              Negotiate a Deal
            </h2>
            {/* Desktop version */}
            <div className="hidden sm:flex items-center gap-6 rounded-xl border border-zinc-200 dark:border-zinc-800 bg-white/50 dark:bg-zinc-900/50 px-4 py-2">
              <div className="text-xs uppercase tracking-wide text-zinc-500 dark:text-zinc-400">
                Offer Ready
              </div>
              <Button
                onClick={
                  needsChainSwitch
                    ? () => onSwitchChain(requiredChain)
                    : onAcceptOffer
                }
                className={`!h-8 !px-3 !text-xs transition-all duration-300 ${
                  needsChainSwitch
                    ? "!bg-blue-500 hover:!bg-blue-600 !text-white !border-blue-600"
                    : `!bg-brand-500 hover:!bg-brand-600 !text-white !border-brand-600 ${
                        isOfferGlowing
                          ? "shadow-lg shadow-brand-500/50 ring-2 ring-brand-400 animate-pulse"
                          : ""
                      }`
                }`}
                color={
                  (needsChainSwitch ? "blue" : "orange") as "blue" | "orange"
                }
                title={
                  needsChainSwitch
                    ? `Switch to ${chainDisplayName}`
                    : `Accept Offer ${isOfferGlowing ? "(GLOWING)" : ""}`
                }
              >
                {needsChainSwitch
                  ? `Switch to ${chainDisplayName}`
                  : "Accept Offer"}
              </Button>
            </div>

            {/* Mobile version */}
            <div className="flex sm:hidden flex-col gap-2 rounded-xl border border-zinc-200 dark:border-zinc-800 bg-white/50 dark:bg-zinc-900/50 p-3">
              <div className="text-xs uppercase tracking-wide text-zinc-500 dark:text-zinc-400">
                Offer Ready
              </div>
              <div className="flex gap-2">
                <Button
                  onClick={
                    needsChainSwitch
                      ? () => onSwitchChain(requiredChain)
                      : onAcceptOffer
                  }
                  className={`flex-1 !h-9 !px-3 !text-sm transition-all duration-300 ${
                    needsChainSwitch
                      ? "!bg-blue-500 hover:!bg-blue-600 !text-white !border-blue-600"
                      : `!bg-brand-500 hover:!bg-brand-600 !text-white !border-brand-600 ${
                          isOfferGlowing
                            ? "shadow-lg shadow-brand-500/50 ring-2 ring-brand-400 animate-pulse"
                            : ""
                        }`
                  }`}
                  color={
                    (needsChainSwitch ? "blue" : "orange") as "blue" | "orange"
                  }
                  title={
                    needsChainSwitch
                      ? `Switch to ${chainDisplayName}`
                      : `Accept Offer ${isOfferGlowing ? "(GLOWING)" : ""}`
                  }
                >
                  {needsChainSwitch
                    ? `Switch to ${chainDisplayName}`
                    : "Accept Offer"}
                </Button>
                {!isLoadingHistory && (
                  <Button
                    onClick={onClearChat}
                    color="red"
                    className="!h-9 !px-3 !text-sm"
                  >
                    Reset
                  </Button>
                )}
              </div>
            </div>
          </>
        ) : (
          !isLoadingHistory && (
            <Button
              onClick={onClearChat}
              color="red"
              className="!h-9 !px-3 !text-sm sm:hidden"
            >
              Reset
            </Button>
          )
        )}
        {!isLoadingHistory && (
          <Button
            onClick={onClearChat}
            color="red"
            className="!h-8 !px-3 !text-xs hidden sm:block"
          >
            Reset
          </Button>
        )}
      </div>
    </div>
  );
}

: {
  messages: ChatMessage[];
  isLoadingHistory: boolean;
  isAgentThinking: boolean;
  input: string;
  setInput: (s: string) => void;
  handleSubmit: (e: React.FormEvent) => void;
  inputDisabled: boolean;
  isConnected: boolean;
  messagesContainerRef: React.RefObject<HTMLDivElement | null>;
  textareaRef: React.RefObject<HTMLTextAreaElement | null>;
  showConnectOverlay: boolean;
  setShowConnectOverlay: (v: boolean) => void;
  currentQuote: OTCQuote | null;
  onAcceptOffer: () => void;
  isOfferGlowing: boolean;
  onClearChat: () => void;
  onConnect: () => void;
  privyReady: boolean;
  activeFamily: "evm" | "solana" | null;
  onSwitchChain: (chain: "evm" | "solana") => void;
  token?: Token;
  marketData?: TokenMarketData | null;
}) {
  return (
    <div className="flex flex-col w-full">
      {/* Connect wallet overlay */}
      <Dialog
        open={showConnectOverlay}
        onClose={() => {
          localStorage.setItem("otc-desk-connect-overlay-seen", "1");
          localStorage.setItem("otc-desk-connect-overlay-dismissed", "1");
          setShowConnectOverlay(false);
        }}
      >
        <div className="w-full rounded-2xl overflow-hidden bg-zinc-50 dark:bg-zinc-900 ring-1 ring-zinc-200 dark:ring-zinc-800 shadow-2xl mx-auto max-h-[90dvh] overflow-y-auto">
          <div className="relative w-full">
            <div className="relative min-h-[200px] sm:min-h-[280px] w-full bg-gradient-to-br from-zinc-900 to-zinc-800 py-6 sm:py-8">
              <div
                aria-hidden
                className="absolute inset-0 opacity-30 bg-no-repeat bg-right-bottom"
                style={{
                  backgroundImage: "url('/business.png')",
                  backgroundSize: "contain",
                }}
              />
              <div className="relative z-10 h-full w-full flex flex-col items-center justify-center text-center px-4 sm:px-6">
                <h2 className="text-xl sm:text-2xl font-semibold text-white tracking-tight mb-2">
                  Sign in to continue
                </h2>
                <Button
                  onClick={onConnect}
                  disabled={!privyReady}
                  color="brand"
                  className="!px-6 sm:!px-8 !py-2 sm:!py-3 !text-base sm:!text-lg"
                >
                  {privyReady ? "Connect Wallet" : "Loading..."}
                </Button>
              </div>
              <button
                type="button"
                onClick={() => {
                  localStorage.setItem("otc-desk-connect-overlay-seen", "1");
                  localStorage.setItem(
                    "otc-desk-connect-overlay-dismissed",
                    "1",
                  );
                  setShowConnectOverlay(false);
                }}
                className="absolute top-2 right-2 rounded-full bg-white/10 text-white hover:bg-white/20 p-1"
                aria-label="Close"
              >
                <svg
                  className="h-5 w-5"
                  viewBox="0 0 20 20"
                  fill="currentColor"
                >
                  <path
                    fillRule="evenodd"
                    d="M4.293 4.293a1 1 0 011.414 0L10 8.586l4.293-4.293a1 1 0 111.414 1.414L11.414 10l4.293 4.293a1 1 0 01-1.414 1.414L10 11.414l-4.293 4.293a1 1 0 01-1.414-1.414L8.586 10 4.293 5.707a1 1 0 010-1.414z"
                    clipRule="evenodd"
                  />
                </svg>
              </button>
            </div>
          </div>
        </div>
      </Dialog>

      {/* Main container - full width */}
      <div className="relative z-10 flex flex-col border border-zinc-200 dark:border-zinc-800 rounded-lg overflow-hidden h-full max-h-[calc(100dvh-120px)]">
        {/* Chat section - Full width */}
        <div className="flex flex-col p-2 sm:p-3 h-full min-h-0">
          {/* Token header inside chat when token is provided */}
          {token && (
            <div className="mb-3">
              <TokenHeader token={token} marketData={marketData ?? null} />
            </div>
          )}
          <ChatHeader
            messages={messages}
            apiQuote={currentQuote}
            onAcceptOffer={onAcceptOffer}
            isOfferGlowing={isOfferGlowing}
            onClearChat={onClearChat}
            isLoadingHistory={isLoadingHistory}
            activeFamily={activeFamily}
            onSwitchChain={onSwitchChain}
          />

          {/* Chat Messages - only scrollable area */}
          <div
            ref={messagesContainerRef}
            className="overflow-y-auto px-2 mb-2 flex-1 min-h-[200px] max-h-[60dvh] sm:max-h-[65dvh]"
          >
            {isLoadingHistory ? (
              <div className="flex items-center justify-center min-h-full">
                <div className="flex items-center gap-2">
                  <LoadingSpinner />
                  <span className="text-zinc-600 dark:text-zinc-400">
                    Loading conversation...
                  </span>
                </div>
              </div>
            ) : messages.length === 0 ? (
              <div className="flex items-center justify-center min-h-[300px] text-center">
                <div>
                  <h2 className="text-xl font-semibold text-zinc-700 dark:text-zinc-300 mb-2">
                    Welcome to AI OTC Desk
                  </h2>
                  <p className="text-zinc-500 dark:text-zinc-400">
                    {isConnected
                      ? "Ask me about quotes and discounted token deals!"
                      : "Connect your wallet to get a quote and start chatting."}
                  </p>
                </div>
              </div>
            ) : (
              <>
                <ChatMessages
                  messages={messages}
                  citationsMap={{}}
                  followUpPromptsMap={{}}
                  onFollowUpClick={(prompt) => {
                    setInput(prompt);
                  }}
                  assistantAvatarUrl={token?.logoUrl}
                  assistantName={token?.name}
                />
                {isAgentThinking && (
                  <div className="flex items-center gap-2 py-4 text-zinc-600 dark:text-zinc-400">
                    <LoadingSpinner />
                    <span>Eliza is thinking...</span>
                  </div>
                )}
              </>
            )}
          </div>

          {/* Input Area - pinned to bottom of chat */}
          <div className="mt-4">
            <TextareaWithActions
              ref={textareaRef}
              input={input}
              onInputChange={(e) => setInput(e.target.value)}
              onSubmit={handleSubmit}
              isLoading={isAgentThinking || inputDisabled || !isConnected}
              placeholder={
                isConnected
                  ? currentQuote?.tokenSymbol
                    ? `Negotiate a deal for $${currentQuote.tokenSymbol}!`
                    : "Ask about available tokens or request a quote!"
                  : "Connect wallet to chat"
              }
            />
          </div>
        </div>
      </div>
    </div>
  );
}

default Chat;

} // namespace elizaos
