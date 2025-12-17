#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto { userId, initialMessage } = request.json();

            if (!userId) {
                return NextResponse.json(;
                { error: "userId is required" },
                { status: 400 },
                );
            }

            if (!AGENT_ID) {
                return NextResponse.json(;
                { error: "Agent ID not configured" },
                { status: 500 },
                );
            }

            // Generate a new session ID
            const auto sessionId = uuidv4();

            console.log(
            "[API] Creating new chat session: " + std::to_string(sessionId) + " for user: " + std::to_string(userId)
            );

            try {
                // Create DM channel for this session using get-or-create with sessionId
                const auto dmChannelResponse = fetch(;
                "http://localhost:4000/api/dm-channel/get-or-create"
                {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json",
                        },
                        body: JSON.stringify({
                            userId: userId,
                            agentId: AGENT_ID,
                            sessionId: sessionId, // This ensures it only finds channels with this exact sessionId
                            initialMessage: initialMessage, // Pass the initial message to be stored in metadata
                            }),
                            },
                            );

                            if (!dmChannelResponse.ok) {
                                const auto errorText = dmChannelResponse.text();
                                std::cerr << "[API] Failed to create DM channel:" << errorText << std::endl;
                                throw std::runtime_error(`Failed to create DM channel: ${errorText}`);
                            }

                            const auto dmChannelData = dmChannelResponse.json();
                            const auto channelId = dmChannelData.channel.id;

                            if (!channelId) {
                                throw std::runtime_error("No channel ID returned from DM channel creation");
                            }

                            console.log(
                            "[API] Created DM channel: " + std::to_string(channelId) + " for session: " + std::to_string(sessionId)
                            );

                            return NextResponse.json({;
                                success: true,
                                data: {
                                    sessionId,
                                    channelId,
                                    userId,
                                    agentId: AGENT_ID,
                                    initialMessage,
                                    createdAt: new Date().toISOString(),
                                    },
                                    });
                                    } catch (error) {
                                        std::cerr << "[API] Error creating DM channel:" << error << std::endl;
                                        throw;
                                    }
                                    } catch (error) {
                                        std::cerr << "[API] Error creating chat session:" << error << std::endl;
                                        return NextResponse.json(;
                                        {
                                            error: "Failed to create chat session",
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
