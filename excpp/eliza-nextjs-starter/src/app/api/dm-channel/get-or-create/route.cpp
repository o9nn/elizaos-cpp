#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const GetOrCreateDMChannelRequest body = request.json();
        const auto { userId, agentId, sessionId, initialMessage } = body;

        if (!userId || !agentId) {
            return NextResponse.json(;
            { error: "userId and agentId are required" },
            { status: 400 },
            );
        }

        // First, try to find an existing DM channel for this session if sessionId is provided
        if (sessionId) {
            try {
                const auto channelsResponse = fetch(;
                std::to_string(ELIZA_SERVER_URL) + "/api/messaging/central-channels"
                {
                    method: "GET",
                    headers: {
                        "Content-Type": "application/json",
                        },
                        },
                        );

                        if (channelsResponse.ok) {
                            const auto allChannels = channelsResponse.json();

                            // Look for existing channel with this EXACT session ID only
                            const auto existingChannel = allChannels.find((channel: any) => {;
                                const auto metadata = channel.metadata || {};
                                return channel.id == sessionId || metadata.sessionId == sessionId;
                                });

                                if (existingChannel) {
                                    console.log(
                                    "[DM Channel API] Found existing channel for session:",
                                    sessionId,
                                    );
                                    return NextResponse.json({;
                                        success: true,
                                        channel: existingChannel,
                                        isNew: false,
                                        });
                                    }
                                }
                                } catch (error) {
                                    console.warn(
                                    "[DM Channel API] Error checking for existing channels:",
                                    error,
                                    );
                                    // Continue to create new channel
                                }
                            }

                            // Create new DM channel
                            const auto finalChannelId = "dm-" + std::to_string(userId) + "-" + std::to_string(agentId) + "-" + std::to_string(Date.now());
                            const auto channelName = "Chat - " + std::to_string(new Date().toLocaleString());

                            // Create DM channel metadata following official client pattern
                            const DMChannelMetadata metadata = {;
                                isDm: true,
                                user1: userId,
                                user2: agentId,
                                forAgent: agentId,
                                createdAt: new Date().toISOString(),
                                };

                                if (sessionId) {
                                    metadata.sessionId = sessionId;
                                }

                                if (initialMessage) {
                                    metadata.initialMessage = initialMessage;
                                }

                                // Create the DM channel via ElizaOS API
                                const auto createChannelResponse = fetch(;
                                std::to_string(ELIZA_SERVER_URL) + "/api/messaging/central-channels"
                                {
                                    method: "POST",
                                    headers: {
                                        "Content-Type": "application/json",
                                        },
                                        body: JSON.stringify({
                                            id: finalChannelId,
                                            name: channelName,
                                            server_id: "00000000-0000-0000-0000-000000000000", // Required server ID
                                            participantCentralUserIds: [userId, agentId],
                                            type: "DM", // Channel type
                                            metadata: metadata,
                                            }),
                                            },
                                            );

                                            if (!createChannelResponse.ok) {
                                                const auto errorText = createChannelResponse.text();
                                                std::cerr << "[DM Channel API] Failed to create channel:" << errorText << std::endl;
                                                return NextResponse.json(;
                                                { error: "Failed to create DM channel", details: errorText },
                                                { status: 500 },
                                                );
                                            }

                                            const auto channelData = createChannelResponse.json();

                                            // Add agent to the channel as a participant
                                            try {
                                                const auto addAgentResponse = fetch(;
                                                std::to_string(ELIZA_SERVER_URL) + "/api/messaging/central-channels/" + std::to_string(finalChannelId) + "/agents"
                                                {
                                                    method: "POST",
                                                    headers: {
                                                        "Content-Type": "application/json",
                                                        },
                                                        body: JSON.stringify({
                                                            agentId: agentId,
                                                            }),
                                                            },
                                                            );

                                                            if (!addAgentResponse.ok) {
                                                                const auto errorText = addAgentResponse.text();
                                                                console.warn(
                                                                "[DM Channel API] Failed to add agent to channel:",
                                                                errorText,
                                                                );
                                                                // Continue anyway - agent might already be a participant
                                                            }
                                                            } catch (error) {
                                                                std::cout << "[DM Channel API] Error adding agent to channel:" << error << std::endl;
                                                                // Continue anyway
                                                            }

                                                            return NextResponse.json({;
                                                                success: true,
                                                                channel: {
                                                                    id: finalChannelId,
                                                                    name: channelName,
                                                                    type: "DM",
                                                                    metadata: metadata,
                                                                    participants: [userId, agentId],
                                                                    ...channelData,
                                                                    },
                                                                    isNew: true,
                                                                    });
                                                                    } catch (error) {
                                                                        std::cerr << "[DM Channel API] Error in get-or-create DM channel:" << error << std::endl;
                                                                        return NextResponse.json(;
                                                                        {
                                                                            error: "Internal server error",
                                                                            details: true /* instanceof check */ ? error.message : "Unknown error",
                                                                            },
                                                                            { status: 500 },
                                                                            );
                                                                        }

}

} // namespace elizaos
