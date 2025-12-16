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

;
;
;
;

async : { params: Promise<{ id: string }> },
) {
  try {
    const { id } = await params;
    const { searchParams } = new URL(request.url);
    const callerAddress =
      searchParams.get("callerAddress") ||
      request.headers.get("x-caller-address");

    const consignment = await ConsignmentDB.getConsignment(id);

    // Check if caller is the owner - only owner can see full negotiation terms
    const isOwner = isConsignmentOwner(consignment, callerAddress);

    // Sanitize response for non-owners to prevent gaming the negotiation
    const responseConsignment = isOwner
      ? consignment
      : sanitizeConsignmentForBuyer(consignment);

    return NextResponse.json({
      success: true,
      consignment: responseConsignment,
    });
  } catch (error) {
    const message = error instanceof Error ? error.message : "Unknown error";
    if (message.includes("not found")) {
      return NextResponse.json({ error: message }, { status: 404 });
    }
    return NextResponse.json({ error: message }, { status: 500 });
  }
}

async : { params: Promise<{ id: string }> },
) {
  try {
    const { id } = await params;
    const body = await request.json();

    // Authorization: caller must provide their address and it must match consigner
    const callerAddress = body.callerAddress;
    if (!callerAddress) {
      return NextResponse.json(
        { error: "callerAddress is required for authorization" },
        { status: 400 },
      );
    }

    const consignment = await ConsignmentDB.getConsignment(id);

    // Normalize addresses for comparison (Solana is case-sensitive, EVM is not)
    const normalizedCaller =
      consignment.chain === "solana"
        ? callerAddress
        : callerAddress.toLowerCase();
    const normalizedConsigner =
      consignment.chain === "solana"
        ? consignment.consignerAddress
        : consignment.consignerAddress.toLowerCase();

    if (normalizedCaller !== normalizedConsigner) {
      return NextResponse.json(
        { error: "Only the consigner can update this consignment" },
        { status: 403 },
      );
    }

    // Remove callerAddress from updates
    const { callerAddress: _callerAddress, ...updates } = body;
    void _callerAddress; // Intentionally unused - just extracting from body
    const service = new ConsignmentService();
    const updated = await service.updateConsignment(id, updates);

    return NextResponse.json({ success: true, consignment: updated });
  } catch (error) {
    const message = error instanceof Error ? error.message : "Unknown error";
    if (message.includes("not found")) {
      return NextResponse.json({ error: message }, { status: 404 });
    }
    return NextResponse.json({ error: message }, { status: 500 });
  }
}

async : { params: Promise<{ id: string }> },
) {
  try {
    const { id } = await params;

    // Authorization: caller must provide their address via query param or header
    const { searchParams } = new URL(request.url);
    const callerAddress =
      searchParams.get("callerAddress") ||
      request.headers.get("x-caller-address");

    if (!callerAddress) {
      return NextResponse.json(
        {
          error:
            "callerAddress query param or x-caller-address header is required",
        },
        { status: 400 },
      );
    }

    const consignment = await ConsignmentDB.getConsignment(id);

    // Normalize addresses for comparison (Solana is case-sensitive, EVM is not)
    const normalizedCaller =
      consignment.chain === "solana"
        ? callerAddress
        : callerAddress.toLowerCase();
    const normalizedConsigner =
      consignment.chain === "solana"
        ? consignment.consignerAddress
        : consignment.consignerAddress.toLowerCase();

    if (normalizedCaller !== normalizedConsigner) {
      return NextResponse.json(
        { error: "Only the consigner can withdraw this consignment" },
        { status: 403 },
      );
    }

    const service = new ConsignmentService();
    await service.withdrawConsignment(id);

    return NextResponse.json({ success: true });
  } catch (error) {
    const message = error instanceof Error ? error.message : "Unknown error";
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
