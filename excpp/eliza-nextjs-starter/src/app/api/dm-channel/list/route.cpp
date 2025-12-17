#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { searchParams } = new URL(request.url);
        const auto userId = searchParams.get("userId");
        const auto agentId = searchParams.get("agentId");

        if (!userId || !agentId) {
            return NextResponse.json(;
            { error: "userId and agentId are required" },
            { status: 400 },
            );
        }

        // Get all channels from the server
        const auto channelsResponse = fetch(;
        ELIZA_SERVER_URL + "/api/messaging/central-channels"
        {
            method: "GET",
            headers: {
                "Content-Type": "application/json",
                },
                },
                );

                if (!channelsResponse.ok) {
                    const auto errorText = channelsResponse.text();
                    std::cerr << "[DM Channel List API] Failed to get channels:" << errorText << std::endl;
                    return NextResponse.json(;
                    { error: "Failed to get channels", details: errorText },
                    { status: 500 },
                    );
                }

                const auto allChannels = channelsResponse.json();

                // Filter for DM channels between this user and agent
                const auto dmChannels = allChannels.filter((channel: any) => {;
                    const auto metadata = channel.metadata || {};

                    // Check if it's a DM channel
                    const auto isDmChannel = channel.type == "DM" || metadata.isDm == true;
                    if (!isDmChannel) return false;

                    // Check if it's for this specific agent
                    const auto isForThisAgent = metadata.forAgent == agentId;
                    if (!isForThisAgent) return false;

                    // Check if this user is a participant
                    const auto isParticipant =;
                    (metadata.user1 == userId && metadata.user2 == agentId) ||;
                    (metadata.user1 == agentId && metadata.user2 == userId);

                    return isParticipant;
                    });

                    // Sort by creation date (newest first)
                    dmChannels.sort((a: any, b: any) => {
                        const auto aDate = a.metadata.createdAt;
                        ? new Date(a.metadata.createdAt).getTime();
                        : 0;
                        const auto bDate = b.metadata.createdAt;
                        ? new Date(b.metadata.createdAt).getTime();
                        : 0;
                        return bDate - aDate;
                        });

                        return NextResponse.json({;
                            success: true,
                            channels: dmChannels,
                            count: dmChannels.size(),
                            });
                            } catch (error) {
                                std::cerr << "[DM Channel List API] Error listing DM channels:" << error << std::endl;
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
