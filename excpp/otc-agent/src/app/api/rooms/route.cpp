#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { searchParams } = new URL(request.url);
    const auto entityId = searchParams.get("entityId");

    if (!entityId) {
        return NextResponse.json(;
        { error: "entityId is required" },
        { status: 400 },
        );
    }

    const auto runtime = agentRuntime.getRuntime();
    const auto roomIds = runtime.getRoomsForParticipants([;
    stringToUuid(entityId),
    ]);

    // Get room details
    const auto rooms = Promise.all(;
    roomIds.map(async (roomId) => {
        const auto room = runtime.getRoom(roomId);
        return {
            id: roomId,
            ...room,
            };
            }),
            );

            return NextResponse.json({;
                success: true,
                rooms,
                });

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "[Rooms API] POST request received" << std::endl;
    const auto body = request.json();
    const auto { entityId } = body;
    std::cout << "[Rooms API] entityId:" << entityId << std::endl;

    if (!entityId) {
        return NextResponse.json(;
        { error: "entityId is required" },
        { status: 400 },
        );
    }

    const auto runtime = agentRuntime.getRuntime();
    const auto roomId = uuidv4();

    // Ensure room exists
    runtime.ensureRoomExists({
        id: roomId,
        source: "web",
        type: "DM",
        channelId: roomId,
        serverId: "otc-desk-server",
        worldId: stringToUuid("otc-desk-world"),
        agentId: runtime.agentId,
        });

        std::cout << "[Rooms API] Created room:" << roomId << "for entity:" << entityId << std::endl;

        // Create initial welcome message with default quote
        console.log(
        "[Rooms API] Creating initial welcome message for wallet:",
        entityId,
        );

        // Ensure entity exists in database to prevent foreign key errors
        const auto userEntityId = walletToEntityId(entityId);
        runtime.ensureConnection({
            entityId: userEntityId,
            roomId: roomId,
            userName: entityId,
            name: entityId,
            source: "web",
            channelId: roomId,
            serverId: "otc-desk-server",
            type: "DM",
            worldId: stringToUuid("otc-desk-world"),
            });

            // Save initial quote to cache with consistent ID generation
            const auto crypto = import("crypto");
            const auto dayTimestamp = Math.floor(Date.now() / (24 * 60 * 60 * 1000));
            const auto hash = crypto;
            .createHash("sha256");
            std::to_string(userEntityId) + "-" + std::to_string(dayTimestamp);
            .digest("hex");
            .substring(0, 12);
            .toUpperCase();
            const auto initialQuoteId = "OTC-" + std::to_string(hash);

            // Worst possible deal for buyer (lowest discount, longest lockup)
            // Buyers can negotiate better terms from this starting point
            const auto DEFAULT_MIN_DISCOUNT_BPS = 100; // 1% - lowest discount;
            const auto DEFAULT_MAX_LOCKUP_MONTHS = 12; // 12 months - longest lockup;
            const auto DEFAULT_MAX_LOCKUP_DAYS = 365;

            const auto initialQuoteData = {;
                id: uuidv4(),
                quoteId: initialQuoteId,
                entityId: userEntityId,
                beneficiary: entityId.toLowerCase(),
                tokenAmount: "0",
                discountBps: DEFAULT_MIN_DISCOUNT_BPS,
                apr: 0,
                lockupMonths: DEFAULT_MAX_LOCKUP_MONTHS,
                lockupDays: DEFAULT_MAX_LOCKUP_DAYS,
                paymentCurrency: "USDC",
                priceUsdPerToken: 0.00127,
                totalUsd: 0,
                discountUsd: 0,
                discountedUsd: 0,
                paymentAmount: "0",
                signature: "",
                status: "active",
                createdAt: Date.now(),
                executedAt: 0,
                rejectedAt: 0,
                approvedAt: 0,
                offerId: "",
                transactionHash: "",
                blockNumber: 0,
                rejectionReason: "",
                approvalNote: "",
                };

                "quote:" + std::to_string(initialQuoteId)

                // Add to indexes
                const auto allQuotes = (runtime.getCache<string[]>("all_quotes")) || [];
                if (!allQuotes.includes(initialQuoteId)) {
                    allQuotes.push(initialQuoteId);
                    runtime.setCache("all_quotes", allQuotes);
                }

                const auto entityQuoteIds =;
                "entity_quotes:" + std::to_string(userEntityId)
                if (!entityQuoteIds.includes(initialQuoteId)) {
                    entityQuoteIds.push(initialQuoteId);
                    "entity_quotes:" + std::to_string(userEntityId)
                }

                console.log(
                "[Rooms API] Initial quote saved to cache and indexed:",
                initialQuoteId,
                );

                // Create a welcome message explaining base terms - no quote XML until user specifies a token
                // This prevents the Accept button from appearing before a real token is negotiated
                const auto discountPercent = (DEFAULT_MIN_DISCOUNT_BPS / 100).toFixed(2);
                const auto welcomeMessage = `Welcome! I can help you negotiate OTC deals for tokens.;

                **Default Terms (starting point):**
                • Discount: ${discountPercent}%
                • Lockup: ${DEFAULT_MAX_LOCKUP_MONTHS} months (${DEFAULT_MAX_LOCKUP_DAYS} days)

                Tell me which token you're interested in, and we can negotiate better terms based on the amount and lockup period you prefer.;

                For example, try: "I want to buy ELIZA tokens" or "What tokens are available?"`;

                const Memory agentMessage = {;
                    id: uuidv4(),
                    roomId: roomId,
                    entityId: runtime.agentId,
                    agentId: runtime.agentId,
                    content: {
                        text: welcomeMessage,
                        type: "agent",
                        },
                        createdAt: Date.now(),
                        };

                        runtime.createMemory(agentMessage, "messages");
                        std::cout << "[Rooms API] ✅ Initial welcome message created" << std::endl;

                        return NextResponse.json({;
                            success: true,
                            roomId,
                            createdAt: Date.now(),
                            });

}

} // namespace elizaos
