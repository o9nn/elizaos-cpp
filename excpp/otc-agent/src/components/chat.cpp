#include "chat.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ChatState chatReducer(ChatState state, ChatAction action) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (action.type) {
        case "SET_MESSAGES":
        return { ...state, messages: action.payload }
        case "ADD_MESSAGE":
        return { ...state, messages: [...state.messages, action.payload] }
        case "SET_INPUT":
        return { ...state, input: action.payload }
        case "SET_INPUT_DISABLED":
        return { ...state, inputDisabled: action.payload }
        case "SET_ROOM_ID":
        return { ...state, roomId: action.payload }
        case "SET_LOADING_HISTORY":
        return { ...state, isLoadingHistory: action.payload }
        case "SET_AGENT_THINKING":
        return { ...state, isAgentThinking: action.payload }
        case "SET_ENTITY_ID":
        return { ...state, entityId: action.payload }
        case "SET_CONNECT_OVERLAY":
        return { ...state, showConnectOverlay: action.payload }
        case "SET_CURRENT_QUOTE":
        return { ...state, currentQuote: action.payload }
        case "SET_ACCEPT_MODAL":
        return { ...state, showAcceptModal: action.payload }
        case "SET_OFFER_GLOWING":
        return { ...state, isOfferGlowing: action.payload }
        case "SET_CLEAR_CHAT_MODAL":
        return { ...state, showClearChatModal: action.payload }
        case "RESET_CHAT":
        return {
            ...state,
            messages: [],
            currentQuote: nullptr,
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
                    entityId: nullptr,
                    inputDisabled: true,
                    showConnectOverlay: true,
                    };
                    default:
                    return state;
                }

}

std::optional<ChatMessage> parseRoomMessage(RawRoomMessage msg, const std::string& roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Parse message text from various possible formats
    auto messageText = "";
    const auto content = msg.content;
    if (typeof content == "object" && content.text) {
        messageText = content.text;
        } else if (msg.text) {
            messageText = msg.text;
            } else if (typeof content == "string") {
                messageText = content;
                } else if (content) {
                    messageText = JSON.stringify(content);
                }

                // Filter out system messages
                if (messageText.startsWith("Executed action:")) {
                    return nullptr;
                }

                return {
                    "msg-" + std::to_string(msg.createdAt)
                    name: msg.entityId == msg.agentId ? "Eliza" : USER_NAME,
                    text: messageText,
                    senderId: msg.entityId,
                    roomId: roomId,
                    createdAt:
                    typeof msg.createdAt == "number";
                    ? msg.createdAt;
                    : new Date(msg.createdAt || Date.now()).getTime(),
                    source: CHAT_SOURCE,
                    isLoading: false,
                    serverMessageId: msg.id,
                    };

}

} // namespace elizaos
