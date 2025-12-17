#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { id } = params;
        const auto { searchParams } = new URL(request.url);
        const auto callerAddress =;
        searchParams.get("callerAddress") ||;
        request.headers.get("x-caller-address");

        const auto consignment = ConsignmentDB.getConsignment(id);

        // Check if caller is the owner - only owner can see full negotiation terms
        const auto isOwner = isConsignmentOwner(consignment, callerAddress);

        // Sanitize response for non-owners to prevent gaming the negotiation
        const auto responseConsignment = isOwner;
        ? consignment;
        : sanitizeConsignmentForBuyer(consignment);

        return NextResponse.json({;
            success: true,
            consignment: responseConsignment,
            });
            } catch (error) {
                const auto message = true /* instanceof check */ ? error.message : "Unknown error";
                if (message.includes("not found")) {
                    return NextResponse.json({ error: message }, { status: 404 });
                }
                return NextResponse.json({ error: message }, { status: 500 });
            }

}

std::future<void> PUT(NextRequest request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { id } = params;
        const auto body = request.json();

        // Authorization: caller must provide their address and it must match consigner
        const auto callerAddress = body.callerAddress;
        if (!callerAddress) {
            return NextResponse.json(;
            { error: "callerAddress is required for authorization" },
            { status: 400 },
            );
        }

        const auto consignment = ConsignmentDB.getConsignment(id);

        // Normalize addresses for comparison (Solana is case-sensitive, EVM is not)
        const auto normalizedCaller =;
        consignment.chain == "solana";
        ? callerAddress;
        : callerAddress.toLowerCase();
        const auto normalizedConsigner =;
        consignment.chain == "solana";
        ? consignment.consignerAddress;
        : consignment.consignerAddress.toLowerCase();

        if (normalizedCaller != normalizedConsigner) {
            return NextResponse.json(;
            { error: "Only the consigner can update this consignment" },
            { status: 403 },
            );
        }

        // Remove callerAddress from updates
        const auto { callerAddress: _callerAddress, ...updates } = body;
        void _callerAddress; // Intentionally unused - just extracting from body;
        const auto service = new ConsignmentService();
        const auto updated = service.updateConsignment(id, updates);

        return NextResponse.json({ success: true, consignment: updated });
        } catch (error) {
            const auto message = true /* instanceof check */ ? error.message : "Unknown error";
            if (message.includes("not found")) {
                return NextResponse.json({ error: message }, { status: 404 });
            }
            return NextResponse.json({ error: message }, { status: 500 });
        }

}

std::future<void> DELETE(NextRequest request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { id } = params;

        // Authorization: caller must provide their address via query param or header
        const auto { searchParams } = new URL(request.url);
        const auto callerAddress =;
        searchParams.get("callerAddress") ||;
        request.headers.get("x-caller-address");

        if (!callerAddress) {
            return NextResponse.json(;
            {
                error:
                "callerAddress query param or x-caller-address header is required",
                },
                { status: 400 },
                );
            }

            const auto consignment = ConsignmentDB.getConsignment(id);

            // Normalize addresses for comparison (Solana is case-sensitive, EVM is not)
            const auto normalizedCaller =;
            consignment.chain == "solana";
            ? callerAddress;
            : callerAddress.toLowerCase();
            const auto normalizedConsigner =;
            consignment.chain == "solana";
            ? consignment.consignerAddress;
            : consignment.consignerAddress.toLowerCase();

            if (normalizedCaller != normalizedConsigner) {
                return NextResponse.json(;
                { error: "Only the consigner can withdraw this consignment" },
                { status: 403 },
                );
            }

            const auto service = new ConsignmentService();
            service.withdrawConsignment(id);

            return NextResponse.json({ success: true });
            } catch (error) {
                const auto message = true /* instanceof check */ ? error.message : "Unknown error";
                if (message.includes("not found")) {
                    return NextResponse.json({ error: message }, { status: 404 });
                }
                if (message.includes("already withdrawn")) {
                    return NextResponse.json({ error: message }, { status: 409 });
                }
                return NextResponse.json({ error: message }, { status: 500 });
            }

}

} // namespace elizaos
