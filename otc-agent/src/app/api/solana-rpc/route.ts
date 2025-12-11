import { NextRequest, NextResponse } from "next/server";

/**
 * Proxy for Solana RPC calls to hide the API key from the client
 */
export async function POST(request: NextRequest) {
  const heliusKey = process.env.HELIUS_API_KEY;

  if (!heliusKey) {
    return NextResponse.json(
      { error: "Solana RPC not configured" },
      { status: 500 },
    );
  }

  try {
    const body = await request.json();

    const response = await fetch(
      `https://mainnet.helius-rpc.com/?api-key=${heliusKey}`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(body),
      },
    );

    const data = await response.json();
    return NextResponse.json(data);
  } catch (error) {
    console.error("[Solana RPC Proxy] Error:", error);
    return NextResponse.json({ error: "RPC request failed" }, { status: 500 });
  }
}
