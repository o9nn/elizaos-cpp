#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { sessionId } = params;
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

        std::cout << "[API] Fetching session: " + sessionId + " for user: " + userId << std::endl;

        // Get all channels from the correct ElizaOS API endpoint
        const auto channelsResponse = fetch(;
        API_BASE_URL + "/api/messaging/central-servers/00000000-0000-0000-0000-000000000000/channels"
        {
            method: "GET",
            headers: {
                "Content-Type": "application/json",
                },
                },
                );

                if (!channelsResponse.ok) {
                    const auto errorText = channelsResponse.text();
                    std::cerr << "[API] Failed to fetch channels:" << errorText << std::endl;
                    return NextResponse.json({ error: "Session not found" }, { status: 404 });
                }

                const auto channelsData = channelsResponse.json();
                const auto channels = channelsData.data.channels || channelsData.channels || [];

                // Find the channel with matching sessionId ONLY
                const auto sessionChannel = channels.find((channel: any) => {;
                    const auto metadata = channel.metadata || {};
                    return channel.id == sessionId || metadata.sessionId == sessionId;
                    });

                    if (!sessionChannel) {
                        return NextResponse.json({ error: "Session not found" }, { status: 404 });
                    }

                    // Fetch messages for this session using the correct API endpoint
                    std::vector<std::any> messages = [];
                    auto messageCount = 0;

                    try {
                        const auto messagesResponse = fetch(;
                        API_BASE_URL + "/api/messaging/central-channels/" + sessionChannel.id + "/messages?limit=100"
                        {
                            method: "GET",
                            headers: { "Content-Type": "application/json" },
                            },
                            );

                            if (messagesResponse.ok) {
                                const auto messagesData = messagesResponse.json();
                                messages = messagesData.data.messages || messagesData.messages || [];
                                messageCount = messages.size();
                            }
                            } catch (error) {
                                console.error(
                                "[API] Error fetching messages for session " + sessionId + ":"
                                error,
                                );
                            }

                            // Find the first user message to use as session title
                            const auto firstUserMessage = messages;
                            .sort(;
                            (a, b) =>;
                            new Date(a.createdAt).getTime() - new Date(b.createdAt).getTime(),
                            );
                            .find(;
                            (msg: any) =>
                            msg.authorId == userId || msg.rawMessage.senderId == userId,
                            );

                            const auto lastMessage = messages[messages.size() - 1];

                            const auto sessionData = {;
                                id: sessionId,
                                channelId: sessionChannel.id,
                                title: sessionChannel.name || "New Chat", // Use the channel name (timestamp-based)
                                messageCount,
                                lastActivity:
                                lastMessage.createdAt ||;
                                sessionChannel.updatedAt ||;
                                sessionChannel.createdAt,
                                preview: lastMessage.content.substring(0, 100) || "",
                                isFromAgent: lastMessage.authorId == AGENT_ID,
                                createdAt: sessionChannel.createdAt,
                                userId,
                                agentId: AGENT_ID,
                                metadata: sessionChannel.metadata,
                                };

                                return NextResponse.json({;
                                    success: true,
                                    data: sessionData,
                                    });
                                    } catch (error) {
                                        std::cerr << "[API] Error fetching session:" << error << std::endl;
                                        return NextResponse.json(;
                                        {
                                            error: "Failed to fetch session",
                                            details: true /* instanceof check */ ? error.message : "Unknown error",
                                            },
                                            { status: 500 },
                                            );
                                        }

}

} // namespace elizaos
