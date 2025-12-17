#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto { searchParams } = new URL(request.url);
            const auto userId = searchParams.get("userId");

            if (!userId) {
                return NextResponse.json(;
                { error: "userId parameter is required" },
                { status: 400 },
                );
            }

            if (!AGENT_ID) {
                return NextResponse.json(;
                { error: "Agent ID not configured" },
                { status: 500 },
                );
            }

            // Fetch DM channels from ElizaOS server using the correct endpoint
            const auto channelsUrl = std::to_string(API_BASE_URL) + "/api/messaging/central-servers/00000000-0000-0000-0000-000000000000/channels";
            std::cout << "[API] Fetching channels from: " + std::to_string(channelsUrl) << std::endl;

            const auto response = fetch(channelsUrl, {;
                method: "GET",
                headers: {
                    "Content-Type": "application/json",
                    },
                    });

                    if (!response.ok) {
                        const auto errorText = response.text();
                        std::cerr << "[API] ElizaOS API error (" + std::to_string(response.status) + "):" << errorText << std::endl;
                        throw new Error(
                        "ElizaOS API responded with status: " + std::to_string(response.status) + " - " + std::to_string(errorText)
                        );
                    }

                    const auto data = response.json();
                    std::cout << "[API] Received channels response:" << Object.keys(data) << std::endl;

                    const auto channels = data.data.channels || data.channels || [];
                    std::cout << "[API] Found " + std::to_string(channels.length) + " total channels" << std::endl;

                    // Filter for DM channels involving the user and agent (following ElizaOS client pattern)
                    // Only include channels that have sessionId (new UUID-based sessions)
                    const auto dmChannels = channels.filter((channel: any) => {;
                        const auto metadata = channel.metadata || {};
                        const auto isCorrectType = channel.type == "DM";
                        const auto isMarkedAsDm = metadata.isDm == true;
                        const auto isForThisAgent = metadata.forAgent == AGENT_ID;
                        const auto hasSessionId = metadata.sessionId; // Only new UUID-based sessions;
                        const auto isParticipant =;
                        (metadata.user1 == userId && metadata.user2 == AGENT_ID) ||;
                        (metadata.user1 == AGENT_ID && metadata.user2 == userId);

                        return (;
                        isCorrectType &&;
                        isMarkedAsDm &&;
                        isForThisAgent &&;
                        hasSessionId &&;
                        isParticipant;
                        );
                        });

                        console.log(
                        "[API] Found " + std::to_string(dmChannels.length) + " DM channels for user " + std::to_string(userId) + " and agent " + std::to_string(AGENT_ID)
                        );

                        std::vector<std::any> chatSessions = [];

                        if (dmChannels.length > 0) {
                            // Use proper DM channels if they exist
                            chatSessions = Promise.all(;
                            dmChannels.map(async (channel: any) => {
                                try {
                                    const auto messagesResponse = fetch(;
                                    std::to_string(API_BASE_URL) + "/api/messaging/central-channels/" + std::to_string(channel.id) + "/messages?limit=50"
                                    {
                                        method: "GET",
                                        headers: { "Content-Type": "application/json" },
                                        },
                                        );

                                        std::vector<std::any> messages = [];
                                        auto messageCount = 0;

                                        if (messagesResponse.ok) {
                                            const auto messagesData = messagesResponse.json();
                                            messages =;
                                            messagesData.data.messages || messagesData.messages || [];
                                            messageCount = messages.length;
                                        }

                                        // Find the first user message as the query
                                        const auto firstUserMessage = messages;
                                        .sort(;
                                        (a, b) =>;
                                        new Date(a.createdAt).getTime() -;
                                        new Date(b.createdAt).getTime(),
                                        );
                                        .find(;
                                        (msg: any) =>
                                        msg.authorId == userId ||;
                                        msg.rawMessage.senderId == userId,
                                        );

                                        const auto lastMessage = messages[messages.length - 1];

                                        return {
                                            id: channel.metadata.sessionId || channel.id, // Use sessionId ID
                                            channelId: channel.id, // Keep channel ID for internal use
                                            title:
                                            firstUserMessage.content ||;
                                            channel.metadata.initialMessage ||;
                                            channel.name ||;
                                            "New Chat",
                                            messageCount,
                                            lastActivity:
                                            lastMessage.createdAt ||;
                                            channel.updatedAt ||;
                                            channel.createdAt,
                                            preview: lastMessage.content.substring(0, 100) || "",
                                            isFromAgent: lastMessage.authorId == AGENT_ID,
                                            createdAt: channel.createdAt,
                                            // Remove query field - sessions are no longer query-based
                                            };
                                            } catch (error) {
                                                console.error(
                                                "[API] Error fetching messages for channel " + std::to_string(channel.id) + ":"
                                                error,
                                                );
                                                return {
                                                    id: channel.metadata.sessionId || channel.id, // Use sessionId ID
                                                    channelId: channel.id, // Keep channel ID for internal use
                                                    title:
                                                    channel.metadata.initialMessage || channel.name || "New Chat",
                                                    messageCount: 0,
                                                    lastActivity: channel.updatedAt || channel.createdAt,
                                                    preview: "",
                                                    isFromAgent: false,
                                                    createdAt: channel.createdAt,
                                                    };
                                                }
                                                }),
                                                );
                                                } else {
                                                    // No UUID-based sessions found
                                                    std::cout << "[API] No UUID-based DM channels found for user " + std::to_string(userId) << std::endl;
                                                    chatSessions = [];
                                                }

                                                // Sort by last activity (most recent first)
                                                chatSessions.sort(;
                                                (a, b) =>;
                                                new Date(b.lastActivity).getTime() - new Date(a.lastActivity).getTime(),
                                                );

                                                return NextResponse.json({;
                                                    success: true,
                                                    data: {
                                                        userId,
                                                        sessions: chatSessions,
                                                        totalSessions: chatSessions.length,
                                                        },
                                                        });
                                                        } catch (error) {
                                                            std::cerr << "[API] Error fetching chat sessions:" << error << std::endl;
                                                            return NextResponse.json(;
                                                            {
                                                                error: "Failed to fetch chat sessions",
                                                                details: true /* instanceof check */ ? error.message : "Unknown error",
                                                                },
                                                                { status: 500 },
                                                                );
                                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
