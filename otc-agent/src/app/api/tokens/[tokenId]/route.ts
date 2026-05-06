import { NextRequest, NextResponse } from "next/server";
import { TokenDB, MarketDataDB, ConsignmentDB } from "@/services/database";
import { sanitizeConsignmentForBuyer } from "@/utils/consignment-sanitizer";

export async function GET(
  request: NextRequest,
  { params }: { params: Promise<{ tokenId: string }> },
) {
  const { tokenId } = await params;

  try {
    const token = await TokenDB.getToken(tokenId);
    const marketData = await MarketDataDB.getMarketData(tokenId);
    let consignments = await ConsignmentDB.getConsignmentsByToken(tokenId);

    // Filter out listings with < 1 token remaining (dust amounts)
    const oneToken = BigInt(10) ** BigInt(token.decimals);
    consignments = consignments.filter(
      (c) => BigInt(c.remainingAmount) >= oneToken,
    );

    // Sanitize consignments to hide negotiation terms from buyers
    // This prevents gaming the negotiation by querying the API
    const sanitizedConsignments = consignments.map(sanitizeConsignmentForBuyer);

    return NextResponse.json({
      success: true,
      token,
      marketData,
      consignments: sanitizedConsignments,
    });
  } catch (error) {
    if (error instanceof Error && error.message.includes("not found")) {
      return NextResponse.json(
        { success: false, error: "Token not found" },
        { status: 404 },
      );
    }
    throw error;
  }
}
