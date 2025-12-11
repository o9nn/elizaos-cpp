import { NextRequest, NextResponse } from "next/server";
import {
  createPublicClient,
  createWalletClient,
  http,
  type Abi,
  type Address,
} from "viem";
import { privateKeyToAccount } from "viem/accounts";
import otcArtifact from "@/contracts/artifacts/contracts/OTC.sol/OTC.json";
import { getChain, getRpcUrl } from "@/lib/getChain";
import { getContractAddress } from "@/lib/getContractAddress";
import type { RawOfferData } from "@/lib/otc-helpers";

// This should be called daily via a cron job (e.g., Vercel Cron or external scheduler)
// It checks for matured OTC and claims them on behalf of users

const EVM_PRIVATE_KEY = process.env.EVM_PRIVATE_KEY as
  | `0x${string}`
  | undefined;
const CRON_SECRET = process.env.CRON_SECRET;

export async function GET(request: NextRequest) {
  // Verify cron secret if using external scheduler
  const authHeader = request.headers.get("authorization");
  const cronSecret = CRON_SECRET;

  // Always require authentication in production
  if (!cronSecret && process.env.NODE_ENV === "production") {
    console.error("[Cron API] No CRON_SECRET configured in production");
    return NextResponse.json(
      { error: "Server configuration error" },
      { status: 500 },
    );
  }

  if (cronSecret && authHeader !== `Bearer ${cronSecret}`) {
    console.warn("[Cron API] Unauthorized cron access attempt", {
      ip:
        request.headers.get("x-forwarded-for") ||
        request.headers.get("x-real-ip"),
      timestamp: new Date().toISOString(),
    });
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
  }

  // Get chain-specific contract address based on NETWORK env var
  let OTC_ADDRESS: Address;
  try {
    OTC_ADDRESS = getContractAddress();
    console.log(
      `[Check Matured OTC] Using contract address: ${OTC_ADDRESS} for network: ${process.env.NETWORK || "localnet"}`,
    );
  } catch (error) {
    console.error("[Check Matured OTC] Failed to get contract address:", error);
    return NextResponse.json(
      {
        error: "Missing contract address configuration",
        details: error instanceof Error ? error.message : "Unknown error",
      },
      { status: 500 },
    );
  }

  const chain = getChain();
  const rpcUrl = getRpcUrl();
  const publicClient = createPublicClient({ chain, transport: http(rpcUrl) });
  const abi = otcArtifact.abi as Abi;

  // Enumerate all offers via nextOfferId
  // Use type assertion to work around viem's strict generics with dynamic ABIs
  const nextOfferId = (await publicClient.readContract({
    address: OTC_ADDRESS,
    abi,
    functionName: "nextOfferId",
    args: [],
  } as unknown as Parameters<typeof publicClient.readContract>[0])) as bigint;

  const now = Math.floor(Date.now() / 1000);
  const maturedOffers: bigint[] = [];

  for (let i = BigInt(1); i < nextOfferId; i++) {
    const offerData = (await publicClient.readContract({
      address: OTC_ADDRESS,
      abi,
      functionName: "offers",
      args: [i],
    } as unknown as Parameters<
      typeof publicClient.readContract
    >[0])) as RawOfferData;

    // Offer struct indices (from OTC.sol):
    // 0: consignmentId, 1: tokenId, 2: beneficiary, 3: tokenAmount, 4: discountBps,
    // 5: createdAt, 6: unlockTime, 7: priceUsdPerToken, 8: maxPriceDeviation,
    // 9: ethUsdPrice, 10: currency, 11: approved, 12: paid, 13: fulfilled, 14: cancelled,
    // 15: payer, 16: amountPaid

    if (!Array.isArray(offerData)) continue;

    const [
      ,
      ,
      // 0: consignmentId
      // 1: tokenId
      beneficiary, // 2: beneficiary
      ,
      ,
      ,
      // 3: tokenAmount
      // 4: discountBps
      // 5: createdAt
      unlockTime, // 6: unlockTime
      ,
      ,
      ,
      ,
      ,
      // 7: priceUsdPerToken
      // 8: maxPriceDeviation
      // 9: ethUsdPrice
      // 10: currency
      // 11: approved
      paid, // 12: paid
      fulfilled, // 13: fulfilled
      cancelled, // 14: cancelled
    ] = offerData;

    // Matured = paid, not fulfilled, not cancelled, and unlockTime passed
    if (
      beneficiary &&
      paid &&
      !fulfilled &&
      !cancelled &&
      Number(unlockTime) > 0 &&
      Number(unlockTime) <= now
    ) {
      maturedOffers.push(i);
    }
  }

  const result: {
    maturedOffers: string[];
    claimedOffers: string[];
    failedOffers: { id: string; error: string }[];
    txHash?: string;
  } = {
    maturedOffers: maturedOffers.map(String),
    claimedOffers: [],
    failedOffers: [],
  };

  // Execute autoClaim as approver if configured and there are matured offers
  if (maturedOffers.length > 0) {
    if (!EVM_PRIVATE_KEY) {
      return NextResponse.json(
        {
          success: false,
          error: "Missing EVM_PRIVATE_KEY",
          maturedOffers: result.maturedOffers,
          message: "Found matured offers but cannot claim without approver key",
        },
        { status: 500 },
      );
    }

    const account = privateKeyToAccount(EVM_PRIVATE_KEY);
    const walletClient = createWalletClient({
      account,
      chain,
      transport: http(rpcUrl),
    });

    // Chunk to avoid gas issues (e.g., 50 per tx)
    const chunkSize = 50;
    const chunks: bigint[][] = [];
    for (let i = 0; i < maturedOffers.length; i += chunkSize) {
      chunks.push(maturedOffers.slice(i, i + chunkSize));
    }

    for (const chunk of chunks) {
      const hash = await walletClient.writeContract({
        address: OTC_ADDRESS,
        abi,
        functionName: "autoClaim",
        args: [chunk],
        account,
        chain,
      });
      // wait for 1 confirmation
      await publicClient.waitForTransactionReceipt({ hash });
      result.txHash = hash;
      result.claimedOffers.push(...chunk.map(String));
    }
  }

  return NextResponse.json({
    success: true,
    timestamp: new Date().toISOString(),
    ...result,
  });
}

// Also support POST for some cron services
export async function POST(request: NextRequest) {
  return GET(request);
}
