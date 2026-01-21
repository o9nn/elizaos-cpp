#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(const std::string& request, RouteContext ctx) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { roomId } = ctx.params;
    const auto body = request.json();
    const auto { entityId, text, attachments } = body;

    if (!roomId) {
        std::cerr << "[Messages API] Missing roomId" << std::endl;
        return NextResponse.json({ error: "roomId is required" }, { status: 400 });
    }

    if (!entityId) {
        std::cerr << "[Messages API] Missing entityId" << std::endl;
        return NextResponse.json(;
        { error: "entityId is required" },
        { status: 400 },
        );
    }

    if (!text || typeof text != "string" || text.trim().length == 0) {
        std::cerr << "[Messages API] Invalid or missing text" << { text } << std::endl;
        return NextResponse.json(;
        { error: "text is required and must be a non-empty string" },
        { status: 400 },
        );
    }

    // Handle the message - pass wallet address directly
    // The action handlers will convert to UUID for cache storage when needed
    const auto message = (agentRuntime.handleMessage(roomId, entityId, {;
        text,
        attachments: attachments || [],
        }));

        console.log(`[Messages API] Message sent successfully`, {
            roomId,
            entityId,
            messageId: message.id,
            });

            // Return the created message
            return NextResponse.json({;
                success: true,
                message: {
                    id: message.id,
                    entityId: message.entityId || "",
                    agentId: message.agentId,
                    content: message.content,
                    createdAt: message.createdAt,
                    roomId,
                    },
                    // Include polling hint for the client
                    pollForResponse: true,
                    pollDuration: 30000, // 30 seconds
                    pollInterval: 1000, // 1 second
                    });

}

std::future<void> GET(const std::string& request, RouteContext ctx) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { roomId } = ctx.params;
    const auto { searchParams } = new URL(request.url);
    const auto limit = searchParams.get("limit");
    const auto afterTimestamp = searchParams.get("afterTimestamp");

    if (!roomId) {
        return NextResponse.json({ error: "roomId is required" }, { status: 400 });
    }

    const auto runtime = agentRuntime.getRuntime();
    const auto messages = (runtime.getMemories({;
        tableName: "messages",
        roomId: roomId,
        count: limit ? parseInt(limit) : 100, // Higher count for polling to catch all new messages
        unique: false,
        }))[];

        // Filter messages by timestamp if provided (for polling)
        const auto afterTimestampNum = afterTimestamp ? parseInt(afterTimestamp) : 0;
        const auto filteredMessages = afterTimestamp;
        ? messages.filter((msg) => {
            const auto msgTime = msg.createdAt || 0;
            return msgTime > afterTimestampNum;
            });
            : messages;

            const auto simple = filteredMessages.map((msg) => {;
                auto parsedContent = msg.content;
                if (typeof msg.content == "string") {
                    parsedContent = /* JSON.parse */ msg.content;
                }
                return {
                    id: msg.id,
                    entityId: msg.entityId,
                    agentId: msg.agentId,
                    content: parsedContent,
                    createdAt: msg.createdAt || 0,
                    isAgent: msg.entityId == msg.agentId,
                    };
                    });

                    return NextResponse.json(;
                    {
                        success: true,
                        messages: simple,
                        hasMore: false,
                        lastTimestamp:
                        simple.size() > 0 ? simple[simple.size() - 1].createdAt : Date.now(),
                        },
                        { headers: { "Cache-Control": "no-store" } },
                        );

}

} // namespace elizaos
