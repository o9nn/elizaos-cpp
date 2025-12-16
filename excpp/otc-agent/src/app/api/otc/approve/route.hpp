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
import type {
  Transaction,
  VersionedTransaction,
  PublicKey as SolanaPublicKey,
} from "@solana/web3.js";
;
;
;
;
;
;
;
import type { QuoteService } from "@/lib/plugin-otc-desk/services/quoteService";
import type { QuoteMemory } from "@/types";

// Helper to safely read from contract bypassing viem's strict authorizationList requirement
using ReadContractFn = (params: unknown) => Promise<unknown>;
const safeReadContract = <T>(
  client: { readContract: ReadContractFn },
  params: unknown,
): Promise<T> => {
  return client.readContract(params) as Promise<T>;
};

async  catch {
      // Fallback to devnet file for local development
      const deployed = path.join(
        process.cwd(),
        "contracts/ignition/deployments/chain-31337/deployed_addresses.json",
      );
      try {
        const raw = await fs.readFile(deployed, "utf8");
        const json = JSON.parse(raw);
        const addr =
          (json["OTCModule#OTC"] as Address) ||
          (json["OTCDeskModule#OTC"] as Address) ||
          (json["ElizaOTCModule#ElizaOTC"] as Address) ||
          (json["OTCModule#desk"] as Address);
        if (addr) {
          console.log(`[Approve API] Using devnet address: ${addr}`);
          return addr;
        }
      } catch {
        // File doesn't exist or can't be read
      }
      throw new Error(
        "No OTC address configured. Set NETWORK and chain-specific NEXT_PUBLIC_*_OTC_ADDRESS env var.",
      );
    }
  };

  const OTC_ADDRESS = await resolveOtcAddress();
  const RAW_PK = process.env.EVM_PRIVATE_KEY as string | undefined;
  const EVM_PRIVATE_KEY =
    RAW_PK && /^0x[0-9a-fA-F]{64}$/.test(RAW_PK)
      ? (RAW_PK as `0x${string}`)
      : undefined;
  if (RAW_PK && !EVM_PRIVATE_KEY) {
    console.warn(
      "[Approve API] Ignoring invalid EVM_PRIVATE_KEY format. Falling back to impersonation.",
    );
  }

  // Parse body
  const contentType = request.headers.get("content-type") || "";
  let offerId: string | number | bigint;
  let chainType: string | undefined;
  let offerAddress: string | undefined;

  if (contentType.includes("application/json")) {
    const body = await request.json();
    offerId = body.offerId;
    chainType = body.chain;
    offerAddress = body.offerAddress;
  } else if (contentType.includes("application/x-www-form-urlencoded")) {
    // Use type assertion for FormData as Next.js returns a compatible type
    const form = (await request.formData()) as unknown as {
      get: (name: string) => FormDataEntryValue | null;
    };
    const v = form.get("offerId");
    if (!v) throw new Error("offerId required in form data");
    offerId = String(v);
  } else {
    const { searchParams } = new URL(request.url);
    const v = searchParams.get("offerId");
    if (!v) throw new Error("offerId required in query params");
    offerId = v;
  }

  console.log("[Approve API] Approving offer:", offerId, "chain:", chainType);

  // Handle Solana approval
  if (chainType === "solana") {
    if (!offerAddress) throw new Error("offerAddress required for Solana");
    console.log(
      "[Approve API] Processing Solana approval for offer:",
      offerAddress,
    );

    // Import Anchor and Solana libs dynamically
    const anchor = await import("@coral-xyz/anchor");
    const { Connection, PublicKey, Keypair } = await import("@solana/web3.js");

    const SOLANA_RPC =
      process.env.NEXT_PUBLIC_SOLANA_RPC || "http://127.0.0.1:8899";
    const SOLANA_DESK = process.env.NEXT_PUBLIC_SOLANA_DESK;

    if (!SOLANA_DESK) throw new Error("SOLANA_DESK not configured");

    const connection = new Connection(SOLANA_RPC, "confirmed");

    // Load owner/approver keypair from id.json
    const idlPath = path.join(
      process.cwd(),
      "solana/otc-program/target/idl/otc.json",
    );
    const keypairPath = path.join(process.cwd(), "solana/otc-program/id.json");
    const idl = JSON.parse(await fs.readFile(idlPath, "utf8"));
    const keypairData = JSON.parse(await fs.readFile(keypairPath, "utf8"));
    const approverKeypair = Keypair.fromSecretKey(Uint8Array.from(keypairData));

    // Create provider with the approver keypair
    // Wallet interface matches @coral-xyz/anchor's Wallet type
    struct AnchorWallet {
    SolanaPublicKey publicKey;
    T tx;
    std::vector<T> txs;
};


    const wallet: AnchorWallet = {
      publicKey: approverKeypair.publicKey,
      signTransaction: async <T extends Transaction | VersionedTransaction>(
        tx: T,
      ) => {
        (tx as Transaction).partialSign(approverKeypair);
        return tx;
      },
      signAllTransactions: async <T extends Transaction | VersionedTransaction>(
        txs: T[],
      ) => {
        txs.forEach((tx) => (tx as Transaction).partialSign(approverKeypair));
        return txs;
      },
    };

    const provider = new anchor.AnchorProvider(connection, wallet, {
      commitment: "confirmed",
    });
    anchor.setProvider(provider);

    const program = new anchor.Program(idl, provider);

    // Approve the offer
    const desk = new PublicKey(SOLANA_DESK);
    const offer = new PublicKey(offerAddress);

    const approveTx = await program.methods
      .approveOffer(new anchor.BN(offerId))
      .accounts({
        desk,
        offer,
        approver: approverKeypair.publicKey,
      })
      .signers([approverKeypair])
      .rpc();

    console.log("[Approve API] ✅ Solana offer approved:", approveTx);

    // Fetch offer to get payment details and token mint
    // In token-agnostic architecture, each offer stores its own token_mint
    using ProgramAccountsFetch = {
      offer: {
        fetch: (address: SolanaPublicKey) => Promise<{
          currency: number;
          id: import("@coral-xyz/anchor").BN;
          tokenMint: SolanaPublicKey;
        }>;
      };
      desk: {
        fetch: (
          address: SolanaPublicKey,
        ) => Promise<{ usdcMint: SolanaPublicKey }>;
      };
    };
    const programAccounts = program.account as unknown as ProgramAccountsFetch;
    const offerData = await programAccounts.offer.fetch(offer);

    // Auto-fulfill (backend pays)
    console.log("[Approve API] Auto-fulfilling Solana offer...");

    const { getAssociatedTokenAddress } = await import("@solana/spl-token");
    const deskData = await programAccounts.desk.fetch(desk);
    // Token mint comes from the offer itself (multi-token support)
    const tokenMint = new PublicKey(offerData.tokenMint);
    const deskTokenTreasury = await getAssociatedTokenAddress(
      tokenMint,
      desk,
      true,
    );

    let fulfillTx: string;

    if (offerData.currency === 0) {
      // Pay with SOL
      fulfillTx = await program.methods
        .fulfillOfferSol(new anchor.BN(offerId))
        .accounts({
          desk,
          offer,
          deskTokenTreasury,
          payer: approverKeypair.publicKey,
          systemProgram: new PublicKey("11111111111111111111111111111111"),
        })
        .signers([approverKeypair])
        .rpc();
      console.log("[Approve API] ✅ Paid with SOL:", fulfillTx);
    } else {
      // Pay with USDC
      const usdcMint = new PublicKey(deskData.usdcMint);
      const deskUsdcTreasury = await getAssociatedTokenAddress(
        usdcMint,
        desk,
        true,
      );
      const payerUsdcAta = await getAssociatedTokenAddress(
        usdcMint,
        approverKeypair.publicKey,
        false,
      );

      fulfillTx = await program.methods
        .fulfillOfferUsdc(new anchor.BN(offerId))
        .accounts({
          desk,
          offer,
          deskTokenTreasury,
          deskUsdcTreasury,
          payerUsdcAta,
          payer: approverKeypair.publicKey,
          tokenProgram: new PublicKey(
            "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA",
          ),
          systemProgram: new PublicKey("11111111111111111111111111111111"),
        })
        .signers([approverKeypair])
        .rpc();
      console.log("[Approve API] ✅ Paid with USDC:", fulfillTx);
    }

    return NextResponse.json({
      success: true,
      approved: true,
      autoFulfilled: true,
      fulfillTx,
      chain: "solana",
      offerAddress,
      approvalTx: approveTx,
    });
  }

  const chain = getChain();
  const rpcUrl = getRpcUrl();
  const publicClient = createPublicClient({ chain, transport: http(rpcUrl) });
  const abi = otcArtifact.abi as Abi;

  // Resolve approver account: prefer PK; else use testWalletPrivateKey from deployment; else impersonate
  let account: PrivateKeyAccount | Address;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  let walletClient: any; // Using any to avoid viem deep type instantiation issues
  let approverAddr: Address;

  if (EVM_PRIVATE_KEY) {
    account = privateKeyToAccount(EVM_PRIVATE_KEY);
    walletClient = createWalletClient({
      account,
      chain,
      transport: http(rpcUrl),
    });
    approverAddr = account.address;
  } else {
    const deploymentInfoPath = path.join(
      process.cwd(),
      "contracts/deployments/eliza-otc-deployment.json",
    );
    const raw = await fs.readFile(deploymentInfoPath, "utf8");
    const json = JSON.parse(raw);
    const testPk = json.testWalletPrivateKey as `0x${string}` | undefined;

    if (testPk && /^0x[0-9a-fA-F]{64}$/.test(testPk)) {
      account = privateKeyToAccount(testPk);
      walletClient = createWalletClient({
        account,
        chain,
        transport: http(rpcUrl),
      });
      approverAddr = account.address;
      console.log(
        "[Approve API] Using testWalletPrivateKey from deployment for approvals",
        { address: approverAddr },
      );
    } else {
      approverAddr = json.accounts.approver as Address;
      if (!approverAddr) throw new Error("approver address not found");

      // Impersonate approver on Anvil
      await fetch("http://127.0.0.1:8545", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          jsonrpc: "2.0",
          method: "anvil_impersonateAccount",
          params: [approverAddr],
          id: 1,
        }),
      });
      account = approverAddr;
      walletClient = createWalletClient({ chain, transport: http(rpcUrl) });
      console.log("[Approve API] Impersonating approver account on Anvil", {
        address: approverAddr,
      });
    }
  }

  // Ensure single approver mode (dev convenience)
  const currentRequired = await safeReadContract<bigint>(publicClient, {
    address: OTC_ADDRESS,
    abi,
    functionName: "requiredApprovals",
    args: [],
  });

  console.log(
    "[Approve API] Current required approvals:",
    Number(currentRequired),
  );

  if (Number(currentRequired) !== 1) {
    console.log("[Approve API] Setting requiredApprovals to 1...");
    const deploymentInfoPath = path.join(
      process.cwd(),
      "contracts/deployments/eliza-otc-deployment.json",
    );
    const raw = await fs.readFile(deploymentInfoPath, "utf8");
    const json = JSON.parse(raw);
    const ownerAddr = json.accounts.owner as Address;

    await fetch("http://127.0.0.1:8545", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        jsonrpc: "2.0",
        method: "anvil_impersonateAccount",
        params: [ownerAddr],
        id: 1,
      }),
    });

    const { request: setReq } = await publicClient.simulateContract({
      address: OTC_ADDRESS,
      abi,
      functionName: "setRequiredApprovals",
      args: [1n],
      account: ownerAddr,
    });
    await createWalletClient({ chain, transport: http(rpcUrl) }).writeContract({
      ...setReq,
      account: ownerAddr,
    });
    console.log("[Approve API] ✅ Set requiredApprovals to 1");
  } else {
    console.log("[Approve API] ✅ Already in single-approver mode");
  }

  // Poll for offer to exist (tx might still be pending)
  // This handles the case where frontend calls us immediately after tx submission
  let offer;
  const maxPollAttempts = 10; // 10 attempts * 2 seconds = 20 seconds max wait

  for (let attempt = 1; attempt <= maxPollAttempts; attempt++) {
    const offerRaw = await safeReadContract<RawOfferData>(publicClient, {
      address: OTC_ADDRESS,
      abi,
      functionName: "offers",
      args: [BigInt(offerId)],
    });

    offer = parseOfferStruct(offerRaw);

    // Check if offer exists (beneficiary is set when offer is created)
    if (
      offer.beneficiary &&
      offer.beneficiary !== "0x0000000000000000000000000000000000000000"
    ) {
      console.log(`[Approve API] Offer found on attempt ${attempt}`);
      break;
    }

    if (attempt < maxPollAttempts) {
      console.log(
        `[Approve API] Offer ${offerId} not found yet, waiting... (${attempt}/${maxPollAttempts})`,
      );
      await new Promise((resolve) => setTimeout(resolve, 2000));
    }
  }

  if (
    !offer?.beneficiary ||
    offer.beneficiary === "0x0000000000000000000000000000000000000000"
  ) {
    return NextResponse.json(
      {
        error: `Offer ${offerId} not found after ${maxPollAttempts} attempts. Transaction may still be pending.`,
      },
      { status: 404 },
    );
  }

  console.log("[Approve API] Offer state:", {
    approved: offer.approved,
    cancelled: offer.cancelled,
    beneficiary: offer.beneficiary,
  });

  if (offer.approved) {
    console.log("[Approve API] Offer already approved");
    return NextResponse.json({
      success: true,
      txHash: "already-approved",
      alreadyApproved: true,
    });
  }

  // ============ PRICE VALIDATION ============
  // Validate that the offer price hasn't diverged too much from market price
  // This prevents abuse from stale quotes or manipulated pool prices
  const MAX_PRICE_DIVERGENCE_BPS = 1000; // 10% maximum divergence

  // Skip price validation for local development (Anvil has mock prices)
  const isLocalNetwork =
    process.env.NEXT_PUBLIC_NETWORK === "localhost" ||
    process.env.NEXT_PUBLIC_NETWORK === "anvil" ||
    process.env.NETWORK === "localhost" ||
    process.env.NETWORK === "anvil";

  if (isLocalNetwork) {
    console.log("[Approve API] Skipping price validation on local network");
  }

  try {
    const { checkPriceDivergence } = await import("@/utils/price-validator");
    const { TokenDB, QuoteDB } = await import("@/services/database");

    // Get token info from the offer
    // offer.priceUsdPerToken is in 8 decimals (Chainlink format)
    const offerPriceUsd = Number(offer.priceUsdPerToken) / 1e8;

    // Find the specific token associated with this offer
    // Primary method: Use the on-chain tokenId (keccak256 hash of symbol) to look up token
    let tokenAddress: string | null = null;
    let tokenChain: "base" | "solana" = "base";

    // The offer.tokenId is a bytes32 (keccak256 of token symbol)
    if (offer.tokenId) {
      const token = await TokenDB.getTokenByOnChainId(offer.tokenId);
      if (token) {
        tokenAddress = token.contractAddress;
        tokenChain = token.chain as "base" | "solana";
        console.log("[Approve API] Found token via on-chain tokenId:", {
          symbol: token.symbol,
          address: tokenAddress,
          chain: tokenChain,
        });
      }
    }

    // Fallback: Try to find via quote (if we have a matching quote by beneficiary)
    if (!tokenAddress) {
      const activeQuotes = await QuoteDB.getActiveQuotes();
      const matchingQuote = activeQuotes.find(
        (q: { beneficiary: string }) =>
          q.beneficiary.toLowerCase() === offer.beneficiary.toLowerCase(),
      );

      if (matchingQuote && "tokenId" in matchingQuote) {
        const token = await TokenDB.getToken(matchingQuote.tokenId as string);
        if (token) {
          tokenAddress = token.contractAddress;
          tokenChain = token.chain as "base" | "solana";
          console.log("[Approve API] Found token via quote:", {
            symbol: token.symbol,
            address: tokenAddress,
            chain: tokenChain,
          });
        }
      }
    }

    // Last resort fallback: Use first active Base token (for backwards compatibility)
    if (!tokenAddress) {
      console.warn(
        "[Approve API] Could not find token via on-chain tokenId or quote, using fallback",
      );
      const tokens = await TokenDB.getAllTokens({ isActive: true });
      const baseToken = tokens.find((t) => t.chain === "base");
      if (baseToken) {
        tokenAddress = baseToken.contractAddress;
        tokenChain = "base";
      }
    }

    if (tokenAddress && offerPriceUsd > 0) {
      console.log("[Approve API] Validating price against market...", {
        offerPriceUsd,
        tokenAddress,
        tokenChain,
      });

      const priceCheck = await checkPriceDivergence(
        tokenAddress,
        tokenChain,
        offerPriceUsd,
      );

      if (!priceCheck.valid && priceCheck.divergencePercent !== undefined) {
        console.log("[Approve API] Price divergence detected:", {
          offerPrice: offerPriceUsd,
          marketPrice: priceCheck.aggregatedPrice,
          divergence: priceCheck.divergencePercent,
          warning: priceCheck.warning,
        });

        // Reject if divergence exceeds threshold (skip on local network)
        if (
          priceCheck.divergencePercent > MAX_PRICE_DIVERGENCE_BPS / 100 &&
          !isLocalNetwork
        ) {
          return NextResponse.json(
            {
              success: false,
              error: "Price divergence too high",
              details: {
                offerPrice: offerPriceUsd,
                marketPrice: priceCheck.aggregatedPrice,
                divergencePercent: priceCheck.divergencePercent,
                maxAllowedPercent: MAX_PRICE_DIVERGENCE_BPS / 100,
                reason: priceCheck.warning,
              },
            },
            { status: 400 },
          );
        } else if (
          isLocalNetwork &&
          priceCheck.divergencePercent > MAX_PRICE_DIVERGENCE_BPS / 100
        ) {
          console.log(
            "[Approve API] Skipping price rejection on local network (divergence:",
            priceCheck.divergencePercent,
            "%)",
          );
        }
      } else {
        console.log("[Approve API] Price validation passed:", {
          divergence: priceCheck.divergencePercent,
          valid: priceCheck.valid,
        });
      }
    }
  } catch (priceError) {
    // Log but don't block - fail open for price validation errors
    console.warn(
      "[Approve API] Price validation failed (continuing):",
      priceError,
    );
  }
  // ============ END PRICE VALIDATION ============

  // Approve immediately
  const accountAddr = (
    typeof account === "string" ? account : account.address
  ) as Address;

  console.log("[Approve API] Simulating approval...", {
    offerId,
    account: accountAddr,
    otcAddress: OTC_ADDRESS,
  });

  const { request: approveRequest } = await publicClient.simulateContract({
    address: OTC_ADDRESS,
    abi,
    functionName: "approveOffer",
    args: [BigInt(offerId)],
    account: accountAddr,
  });

  console.log("[Approve API] Sending approval tx...");
  const txHash: `0x${string}` =
    await walletClient.writeContract(approveRequest);

  console.log("[Approve API] Waiting for confirmation...", txHash);
  const approvalReceipt = await publicClient.waitForTransactionReceipt({
    hash: txHash,
  });

  console.log("[Approve API] Approval receipt:", {
    status: approvalReceipt.status,
    blockNumber: approvalReceipt.blockNumber,
    gasUsed: approvalReceipt.gasUsed?.toString(),
  });

  console.log("[Approve API] ✅ Offer approved:", offerId, "tx:", txHash);

  // Update quote status and financial data if we can find it
  const runtime = await agentRuntime.getRuntime();
  const quoteService = runtime.getService<QuoteService>("QuoteService");

  if (quoteService && offer.beneficiary) {
    const activeQuotes = await quoteService.getActiveQuotes();
    const matchingQuote = activeQuotes.find(
      (q: QuoteMemory) =>
        q.beneficiary.toLowerCase() === offer.beneficiary.toLowerCase(),
    );

    if (matchingQuote) {
      // Calculate financial values from on-chain offer data
      const tokenAmountWei = BigInt(offer.tokenAmount);
      const priceUsd8 = BigInt(offer.priceUsdPerToken);
      const discountBpsNum = Number(offer.discountBps);

      // totalUsd = (tokenAmount * priceUsdPerToken) / 1e18 (result in 8 decimals)
      const totalUsd8 = (tokenAmountWei * priceUsd8) / BigInt(1e18);
      const totalUsd = Number(totalUsd8) / 1e8;

      // discountUsd = totalUsd * discountBps / 10000
      const discountUsd8 = (totalUsd8 * BigInt(discountBpsNum)) / 10000n;
      const discountUsd = Number(discountUsd8) / 1e8;

      // discountedUsd = totalUsd - discountUsd
      const discountedUsd8 = totalUsd8 - discountUsd8;
      const discountedUsd = Number(discountedUsd8) / 1e8;

      // Determine payment currency and amount based on offer currency
      const paymentCurrency: "ETH" | "USDC" =
        offer.currency === 0 ? "ETH" : "USDC";
      let paymentAmount = "0";

      if (offer.currency === 0) {
        // Calculate required ETH
        const ethPrice = Number(offer.ethUsdPrice) / 1e8;
        paymentAmount = (discountedUsd / ethPrice).toFixed(6);
      } else {
        // USDC
        paymentAmount = discountedUsd.toFixed(2);
      }

      console.log("[Approve API] Calculated financial data:", {
        tokenAmount: offer.tokenAmount.toString(),
        totalUsd,
        discountUsd,
        discountedUsd,
        paymentAmount,
        paymentCurrency,
      });

      // Update quote status
      await quoteService.updateQuoteStatus(matchingQuote.quoteId, "approved", {
        offerId: String(offerId),
        transactionHash: txHash,
        blockNumber: Number(approvalReceipt.blockNumber),
        rejectionReason: "",
        approvalNote: "Approved via API",
      });

      // Update quote with financial data from contract
      const updatedQuote = await quoteService.getQuoteByQuoteId(
        matchingQuote.quoteId,
      );
      updatedQuote.tokenAmount = offer.tokenAmount.toString();
      updatedQuote.totalUsd = totalUsd;
      updatedQuote.discountUsd = discountUsd;
      updatedQuote.discountedUsd = discountedUsd;
      updatedQuote.paymentAmount = paymentAmount;
      updatedQuote.paymentCurrency = paymentCurrency;
      updatedQuote.discountBps = discountBpsNum;

      await runtime.setCache(`quote:${matchingQuote.quoteId}`, updatedQuote);

      console.log(
        "[Approve API] Updated quote with financial data:",
        matchingQuote.quoteId,
      );
    }
  }

  // If still not approved (multi-approver deployments), escalate approvals
  let approvedOfferRaw = await safeReadContract<RawOfferData>(publicClient, {
    address: OTC_ADDRESS,
    abi,
    functionName: "offers",
    args: [BigInt(offerId)],
  });

  let approvedOffer = parseOfferStruct(approvedOfferRaw);

  if (!approvedOffer.approved) {
    // Load known approver and agent from deployment file
    const deploymentInfoPath = path.join(
      process.cwd(),
      "contracts/deployments/eliza-otc-deployment.json",
    );
    const raw = await fs.readFile(deploymentInfoPath, "utf8");
    const json = JSON.parse(raw);
    const approver = json.accounts.approver as Address;
    const agentAddr = json.accounts.agent as Address;
    const candidates = [approver, agentAddr];

    for (const addr of candidates) {
      console.log("[Approve API] Attempting secondary approval by", addr);

      await fetch("http://127.0.0.1:8545", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          jsonrpc: "2.0",
          method: "anvil_impersonateAccount",
          params: [addr],
          id: 1,
        }),
      });

      const { request: req2 } = await publicClient.simulateContract({
        address: OTC_ADDRESS,
        abi,
        functionName: "approveOffer",
        args: [BigInt(offerId)],
        account: addr,
      });
      await createWalletClient({
        chain,
        transport: http(rpcUrl),
      }).writeContract({ ...req2, account: addr });

      // Re-read state after each attempt
      approvedOfferRaw = await safeReadContract<RawOfferData>(publicClient, {
        address: OTC_ADDRESS,
        abi,
        functionName: "offers",
        args: [BigInt(offerId)],
      });
      approvedOffer = parseOfferStruct(approvedOfferRaw);
      if (approvedOffer.approved) break;
    }
  }

  // Final verification that offer was approved
  console.log("[Approve API] Verifying final approval state...");

  approvedOfferRaw = await safeReadContract<RawOfferData>(publicClient, {
    address: OTC_ADDRESS,
    abi,
    functionName: "offers",
    args: [BigInt(offerId)],
  });

  approvedOffer = parseOfferStruct(approvedOfferRaw);

  console.log("[Approve API] Final offer state:", {
    offerId,
    approved: approvedOffer.approved,
    cancelled: approvedOffer.cancelled,
    paid: approvedOffer.paid,
    fulfilled: approvedOffer.fulfilled,
  });

  if (approvedOffer.cancelled) {
    return NextResponse.json({ error: "Offer is cancelled" }, { status: 400 });
  }

  if (!approvedOffer.approved) {
    throw new Error("Offer still not approved after all attempts");
  }

  // Check if approver should also fulfill
  const requireApproverToFulfill = await safeReadContract<boolean>(
    publicClient,
    {
      address: OTC_ADDRESS,
      abi,
      functionName: "requireApproverToFulfill",
      args: [],
    },
  );

  console.log(
    "[Approve API] requireApproverToFulfill:",
    requireApproverToFulfill,
  );

  let fulfillTxHash: `0x${string}` | undefined;

  // If approver-only fulfill is enabled, backend pays immediately after approval
  if (requireApproverToFulfill && !approvedOffer.paid) {
    console.log("[Approve API] Auto-fulfilling offer (approver-only mode)...");

    try {
      const accountAddr = (
        typeof account === "string" ? account : account.address
      ) as Address;

      // Calculate required payment
      const currency = approvedOffer.currency;
      let valueWei: bigint | undefined;

      if (currency === 0) {
        // ETH payment required
        const requiredEth = await safeReadContract<bigint>(publicClient, {
          address: OTC_ADDRESS,
          abi,
          functionName: "requiredEthWei",
          args: [BigInt(offerId)],
        });

        valueWei = requiredEth;
        console.log("[Approve API] Required ETH:", requiredEth.toString());
      } else {
        // USDC payment - need to approve first
        const usdcAddress = await safeReadContract<Address>(publicClient, {
          address: OTC_ADDRESS,
          abi,
          functionName: "usdc",
          args: [],
        });

        const requiredUsdc = await safeReadContract<bigint>(publicClient, {
          address: OTC_ADDRESS,
          abi,
          functionName: "requiredUsdcAmount",
          args: [BigInt(offerId)],
        });

        console.log("[Approve API] Required USDC:", requiredUsdc.toString());

        // Approve USDC
        const erc20Abi = [
          {
            type: "function",
            name: "approve",
            stateMutability: "nonpayable",
            inputs: [
              { name: "spender", type: "address" },
              { name: "amount", type: "uint256" },
            ],
            outputs: [{ name: "", type: "bool" }],
          },
        ] as Abi;

        const { request: approveUsdcReq } = await publicClient.simulateContract(
          {
            address: usdcAddress,
            abi: erc20Abi,
            functionName: "approve",
            args: [OTC_ADDRESS, requiredUsdc],
            account: accountAddr,
          },
        );

        await walletClient.writeContract(approveUsdcReq);
        console.log("[Approve API] USDC approved");
      }

      // Fulfill offer
      const { request: fulfillReq } = await publicClient.simulateContract({
        address: OTC_ADDRESS,
        abi,
        functionName: "fulfillOffer",
        args: [BigInt(offerId)],
        account: accountAddr,
        value: valueWei,
      });

      fulfillTxHash = await walletClient.writeContract(fulfillReq);
      console.log("[Approve API] Fulfill tx sent:", fulfillTxHash);

      if (fulfillTxHash) {
        await publicClient.waitForTransactionReceipt({ hash: fulfillTxHash });
      }
      console.log("[Approve API] ✅ Offer fulfilled automatically");
    } catch (fulfillError) {
      console.error("[Approve API] ❌ Auto-fulfill failed:", fulfillError);

      // Check if offer got paid by another transaction during our attempt
      const recheckOffer = await safeReadContract<RawOfferData>(publicClient, {
        address: OTC_ADDRESS,
        abi,
        functionName: "offers",
        args: [BigInt(offerId)],
      });

      const recheckParsed = parseOfferStruct(recheckOffer);

      if (recheckParsed.paid) {
        console.log(
          "[Approve API] ✅ Offer was paid by another transaction, continuing...",
        );
        fulfillTxHash = undefined; // No fulfillTx from us, but offer is paid
      } else {
        // Offer is approved but not paid - this is a real error
        throw new Error(
          `Auto-fulfill failed: ${fulfillError instanceof Error ? fulfillError.message : String(fulfillError)}. Offer is approved but not paid.`,
        );
      }
    }
  } else if (approvedOffer.paid) {
    console.log("[Approve API] ✅ Offer already paid, skipping auto-fulfill");
  } else if (!requireApproverToFulfill) {
    console.log(
      "[Approve API] ⚠️  requireApproverToFulfill is disabled. User must fulfill (pay) manually.",
    );
  }

  // Return success
  return NextResponse.json({
    success: true,
    approved: true,
    approvalTx: txHash,
    fulfillTx: fulfillTxHash,
    offerId: String(offerId),
    autoFulfilled: Boolean(fulfillTxHash),
    message: fulfillTxHash
      ? "Offer approved and fulfilled automatically"
      : "Offer approved. Please complete payment to fulfill the offer.",
  });
}

} // namespace elizaos
