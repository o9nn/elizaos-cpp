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
import type { QuoteMemory } from "@/lib/plugin-otc-desk/types";
;
;
;
;
;
;

// Type-safe wrapper for readContract with dynamic ABIs
struct ReadContractParams {
    Address address;
    Abi abi;
    std::string functionName;
    std::optional<std::vector<readonly unknown>> args;
};


// Use type assertion to bypass viem's strict authorizationList requirement
async ,
  params: ReadContractParams,
): Promise<T> {
  const result = await client.readContract(params);
  return result as T;
}

async  = body;

  if (!quoteId) {
    return NextResponse.json(
      { error: "Quote ID is required" },
      { status: 400 },
    );
  }

  if (action === "complete") {
    if (consignmentId && tokenId) {
      const { PriceProtectionService } = await import(
        "@/services/priceProtection"
      );
      const { TokenDB } = await import("@/services/database");
      const { ConsignmentService } = await import(
        "@/services/consignmentService"
      );

      const priceProtection = new PriceProtectionService();
      const consignmentService = new ConsignmentService();
      const token = await TokenDB.getToken(tokenId);

      const priceAtQuote = body.priceAtQuote || 1.0;
      const validationResult = await priceProtection.validateQuotePrice(
        tokenId,
        token.contractAddress,
        token.chain,
        priceAtQuote,
        body.maxPriceDeviationBps || 1000,
      );

      if (!validationResult.isValid) {
        return NextResponse.json(
          {
            error: "Price volatility exceeded maximum allowed",
            details: validationResult,
          },
          { status: 400 },
        );
      }

      await consignmentService.reserveAmount(consignmentId, body.tokenAmount);
      await consignmentService.recordDeal({
        consignmentId,
        quoteId,
        tokenId,
        buyerAddress: body.beneficiary || "",
        amount: body.tokenAmount,
        discountBps: body.discountBps || 1000,
        lockupDays: body.lockupDays || 150,
        offerId: body.offerId,
      });
    }

    const tokenAmountStr = String(body.tokenAmount);
    console.log("[DealCompletion] Received request:", {
      tokenAmount: body.tokenAmount,
      tokenAmountStr,
      paymentCurrency: body.paymentCurrency,
      chain: body.chain,
      offerId: body.offerId,
    });

    // Map SOL to ETH internally since database schema uses ETH/USDC
    const paymentCurrency: PaymentCurrency =
      body.paymentCurrency === "ETH" || body.paymentCurrency === "SOL"
        ? "ETH"
        : "USDC";
    const offerId = String(body.offerId || "");
    const transactionHash = String(body.transactionHash || "");
    const blockNumber = Number(body.blockNumber || 0);
    const chainType = body.chain || "evm";
    const offerAddress = body.offerAddress;
    const beneficiaryOverride = body.beneficiary; // Solana wallet address

    const runtime = agentRuntime.runtime;
    if (!runtime) {
      return NextResponse.json(
        { error: "Runtime not initialized" },
        { status: 500 },
      );
    }

    const quoteService = runtime.getService<QuoteService>("QuoteService");
    if (!quoteService) {
      return NextResponse.json(
        { error: "QuoteService not available" },
        { status: 500 },
      );
    }

    const quote = await quoteService.getQuoteByQuoteId(quoteId);

    // Update beneficiary AND entityId if provided (for Solana wallets)
    if (beneficiaryOverride) {
      const normalizedBeneficiary = beneficiaryOverride.toLowerCase();
      if (quote.beneficiary !== normalizedBeneficiary) {
        console.log("[DealCompletion] Updating beneficiary and entityId:", {
          oldBeneficiary: quote.beneficiary,
          newBeneficiary: normalizedBeneficiary,
          oldEntityId: quote.entityId,
        });

        // Update both beneficiary and entityId to match
        await quoteService.setQuoteBeneficiary(quoteId, normalizedBeneficiary);

        // Re-fetch to get updated quote
        const updatedQuote = await quoteService.getQuoteByQuoteId(quoteId);
        Object.assign(quote, updatedQuote);

        console.log(
          "[DealCompletion] Updated to new entityId:",
          quote.entityId,
        );
      }
    }

    let totalUsd = 0;
    let discountUsd = 0;
    let discountedUsd = 0;
    let actualPaymentAmount = "0";

    // Handle Solana deals (calculate from quote data, not contract)
    if (chainType === "solana") {
      console.log("[DealCompletion] Processing Solana deal:", {
        quoteId,
        offerId,
        offerAddress,
        tokenAmount: tokenAmountStr,
        transactionHash,
      });

      // Verify transaction on-chain
      if (!transactionHash) {
        return NextResponse.json(
          { error: "Transaction hash required for Solana verification" },
          { status: 400 },
        );
      }

      try {
        const rpcUrl =
          process.env.NEXT_PUBLIC_SOLANA_RPC ||
          "https://api.mainnet-beta.solana.com";
        const connection = new Connection(rpcUrl, "confirmed");

        console.log(
          `[DealCompletion] Verifying Solana tx: ${transactionHash} on ${rpcUrl}`,
        );

        // Fetch transaction
        const tx = await connection.getTransaction(transactionHash, {
          commitment: "confirmed",
          maxSupportedTransactionVersion: 0,
        });

        if (!tx) {
          throw new Error("Transaction not found or not confirmed");
        }

        if (tx.meta?.err) {
          throw new Error(`Transaction failed: ${JSON.stringify(tx.meta.err)}`);
        }

        console.log("[DealCompletion] ✅ Solana transaction verified on-chain");
      } catch (error) {
        console.error("[DealCompletion] Solana verification failed:", error);
        // If localnet, maybe allow skip? No, always enforce.
        // Unless we are in a mock environment where RPC fails?
        // For now, strict enforcement.
        return NextResponse.json(
          {
            error: "Solana transaction verification failed",
            details: error instanceof Error ? error.message : String(error),
          },
          { status: 400 },
        );
      }

      const tokenAmount = BigInt(tokenAmountStr);
      const discountBps = quote.discountBps || 1000;

      // Fetch real prices from market data and price feeds
      let tokenPrice = 0;
      let solPrice = 0;

      // Try to get actual token price from market data
      if (quote.tokenId) {
        try {
          const { MarketDataDB } = await import("@/services/database");
          const marketData = await MarketDataDB.getMarketData(quote.tokenId);
          if (marketData?.priceUsd && marketData.priceUsd > 0) {
            tokenPrice = marketData.priceUsd;
            console.log(
              "[DealCompletion] Using market data token price:",
              tokenPrice,
            );
          }
        } catch (err) {
          console.warn(
            "[DealCompletion] Failed to fetch token market data:",
            err,
          );
        }
      }

      // Fallback to quote's stored price if market data unavailable
      if (
        tokenPrice === 0 &&
        quote.priceUsdPerToken &&
        quote.priceUsdPerToken > 0
      ) {
        tokenPrice = quote.priceUsdPerToken;
        console.log(
          "[DealCompletion] Using quote stored token price:",
          tokenPrice,
        );
      }

      // Get SOL price from price feed API
      try {
        const { getSolPriceUsd } = await import(
          "@/lib/plugin-otc-desk/services/priceFeed"
        );
        solPrice = await getSolPriceUsd();
        console.log("[DealCompletion] Using SOL price from API:", solPrice);
      } catch (err) {
        console.warn(
          "[DealCompletion] Failed to fetch SOL price from API:",
          err,
        );
        // Try market data as fallback
        try {
          const { MarketDataDB } = await import("@/services/database");
          const solMarketData = await MarketDataDB.getMarketData(
            "token-solana-So11111111111111111111111111111111111111112",
          );
          if (solMarketData?.priceUsd && solMarketData.priceUsd > 0) {
            solPrice = solMarketData.priceUsd;
          }
        } catch {
          // Last resort: env variable
          const solPriceEnv = process.env.SOL_USD_PRICE;
          if (solPriceEnv) {
            solPrice = parseFloat(solPriceEnv);
          }
        }
        console.log("[DealCompletion] Using fallback SOL price:", solPrice);
      }

      // Validate we have real prices
      if (tokenPrice === 0) {
        console.error(
          "[DealCompletion] CRITICAL: Token price is $0 - deal value cannot be calculated",
        );
        return NextResponse.json(
          {
            error:
              "Token price unavailable - please ensure token has market data",
          },
          { status: 400 },
        );
      }
      if (body.paymentCurrency === "SOL" && solPrice === 0) {
        console.error(
          "[DealCompletion] CRITICAL: SOL price is $0 - cannot calculate SOL payment",
        );
        return NextResponse.json(
          { error: "SOL price unavailable - please try again later" },
          { status: 400 },
        );
      }

      // Calculate values
      totalUsd = Number(tokenAmount) * tokenPrice;
      discountUsd = totalUsd * (discountBps / 10000);
      discountedUsd = totalUsd - discountUsd;

      // Payment amount based on currency
      if (body.paymentCurrency === "SOL") {
        actualPaymentAmount = (discountedUsd / solPrice).toFixed(6);
      } else {
        actualPaymentAmount = discountedUsd.toFixed(2);
      }

      console.log("[DealCompletion] Calculated Solana deal values:", {
        tokenPrice,
        solPrice,
        totalUsd,
        discountUsd,
        discountedUsd,
        actualPaymentAmount,
      });
    } else if (offerId && offerId !== "0") {
      // Fetch on-chain data for EVM deals
      // Use chain-specific contract address based on NETWORK env var
      const OTC_ADDRESS = getContractAddress();

      const chain = getChain();
      const RPC_URL = getRpcUrl();

      console.log("[DealCompletion] Fetching on-chain data:", {
        offerId,
        OTC_ADDRESS,
        RPC_URL,
        network: process.env.NETWORK || "localnet",
      });

      const publicClient = createPublicClient({
        chain,
        transport: http(RPC_URL),
      });
      const abi = otcArtifact.abi as Abi;

      using OfferData = [
        bigint, // consignmentId
        string, // tokenId (bytes32)
        Address, // beneficiary
        bigint, // tokenAmount
        bigint, // discountBps
        bigint, // createdAt
        bigint, // unlockTime
        bigint, // priceUsdPerToken
        bigint, // maxPriceDeviation
        bigint, // ethUsdPrice
        number, // currency
        boolean, // approved
        boolean, // paid
        boolean, // fulfilled
        boolean, // cancelled
        Address, // payer
        bigint, // amountPaid
      ];
      const offerData = await readContractSafe<OfferData>(publicClient, {
        address: OTC_ADDRESS,
        abi,
        functionName: "offers",
        args: [BigInt(offerId)],
      });

      // Contract returns array matching struct order
      const [
        ,
        ,
        ,
        // consignmentId
        // tokenId
        // beneficiary
        tokenAmount,
        discountBps, // createdAt
        ,
        ,
        // unlockTime
        priceUsdPerToken, // maxPriceDeviation
        ,
        ,
        // ethUsdPrice
        currency, // approved
        ,
        paid, // fulfilled
        ,
        ,
        ,
        // cancelled
        // payer
        amountPaid,
      ] = offerData;

      console.log("[DealCompletion] Offer data from contract:", {
        tokenAmount: tokenAmount?.toString(),
        priceUsdPerToken: priceUsdPerToken?.toString(),
        discountBps: discountBps?.toString(),
        amountPaid: amountPaid?.toString(),
        currency,
        paid,
      });

      // Calculate real USD values from on-chain data
      // tokenAmount is 18 decimals, priceUsdPerToken is 8 decimals
      const tokenAmountWei = BigInt(tokenAmount);
      const priceUsd8 = BigInt(priceUsdPerToken);
      const discountBpsNum = Number(discountBps);
      const amountPaidBig = BigInt(amountPaid);

      // totalUsd = (tokenAmount * priceUsdPerToken) / 1e18 (result in 8 decimals)
      const totalUsd8 = (tokenAmountWei * priceUsd8) / BigInt(1e18);
      totalUsd = Number(totalUsd8) / 1e8;

      // discountUsd = totalUsd * discountBps / 10000
      const discountUsd8 = (totalUsd8 * BigInt(discountBpsNum)) / 10000n;
      discountUsd = Number(discountUsd8) / 1e8;

      // discountedUsd = totalUsd - discountUsd
      const discountedUsd8 = totalUsd8 - discountUsd8;
      discountedUsd = Number(discountedUsd8) / 1e8;

      // Format payment amount based on currency
      if (currency === 0) {
        // ETH (18 decimals)
        actualPaymentAmount = (Number(amountPaidBig) / 1e18).toFixed(6);
      } else {
        // USDC (6 decimals)
        actualPaymentAmount = (Number(amountPaidBig) / 1e6).toFixed(2);
      }

      console.log("[DealCompletion] Calculated from on-chain data:", {
        totalUsd,
        discountUsd,
        discountedUsd,
        actualPaymentAmount,
      });
    } else {
      // No offerId, use quote values (fallback)
      const discountBps = quote.discountBps;
      totalUsd = quote.totalUsd || 0;
      discountUsd = quote.discountUsd || totalUsd * (discountBps / 10000);
      discountedUsd = quote.discountedUsd || totalUsd - discountUsd;
      actualPaymentAmount = quote.paymentAmount || "0";
    }

    // VALIDATE before saving
    if (!tokenAmountStr || tokenAmountStr === "0") {
      console.warn(
        `[DealCompletion] tokenAmount is ${tokenAmountStr} - quote: ${quoteId}`,
      );
      // For old quotes, skip validation and just return current state
      if (quote.status === "executed") {
        console.log(
          "[DealCompletion] Quote already executed, returning current state",
        );
        return NextResponse.json({ success: true, quote });
      }
      throw new Error(
        `CRITICAL: tokenAmount is ${tokenAmountStr} - must be > 0`,
      );
    }
    if (totalUsd === 0 && chainType === "solana") {
      console.warn(
        `[DealCompletion] Solana deal has $0 value - quote: ${quoteId}`,
      );
      if (quote.status === "executed") {
        console.log(
          "[DealCompletion] Quote already executed, returning current state",
        );
        return NextResponse.json({ success: true, quote });
      }
      throw new Error("CRITICAL: Solana deal has $0 value");
    }

    // Calculate priceUsdPerToken from totalUsd / tokenAmount
    const tokenAmountNum = parseFloat(tokenAmountStr);
    const priceUsdPerToken = tokenAmountNum > 0 ? totalUsd / tokenAmountNum : 0;

    console.log("[DealCompletion] Calling updateQuoteExecution with:", {
      quoteId,
      tokenAmount: tokenAmountStr,
      totalUsd,
      discountUsd,
      discountedUsd,
      paymentCurrency,
      paymentAmount: actualPaymentAmount,
      offerId,
      priceUsdPerToken,
      lockupDays: body.lockupDays,
    });

    const updated = await quoteService.updateQuoteExecution(quoteId, {
      tokenAmount: tokenAmountStr,
      totalUsd,
      discountUsd,
      discountedUsd,
      paymentCurrency,
      paymentAmount: actualPaymentAmount,
      offerId,
      transactionHash,
      blockNumber,
      priceUsdPerToken,
      lockupDays: body.lockupDays,
    });

    // VERIFY status changed
    if (updated.status !== "executed") {
      throw new Error(
        `CRITICAL: Status is ${updated.status}, expected executed`,
      );
    }

    // Store chain type for proper currency display
    const updatedWithChain = {
      ...updated,
      chain: chainType === "solana" ? "solana" : "evm",
    };
    await runtime.setCache(`quote:${quoteId}`, updatedWithChain);

    // VERIFY quote is in entity's list, and fix index if missing
    const entityQuotes =
      (await runtime.getCache<string[]>(`entity_quotes:${updated.entityId}`)) ||
      [];
    if (!entityQuotes.includes(quoteId)) {
      console.warn(
        `[Deal Completion] Quote ${quoteId} not in entity ${updated.entityId} list - fixing index`,
      );
      entityQuotes.push(quoteId);
      await runtime.setCache(`entity_quotes:${updated.entityId}`, entityQuotes);

      // Also ensure it's in the all_quotes index
      const allQuotes = (await runtime.getCache<string[]>("all_quotes")) || [];
      if (!allQuotes.includes(quoteId)) {
        allQuotes.push(quoteId);
        await runtime.setCache("all_quotes", allQuotes);
      }
      console.log(`[Deal Completion] ✅ Fixed indexes for quote ${quoteId}`);
    }

    console.log("[Deal Completion] ✅ VERIFIED and completed:", {
      entityId: quote.entityId,
      quoteId,
      tokenAmount: updated.tokenAmount,
      status: updated.status,
      inEntityList: true,
      discountBps: quote.discountBps,
      finalPrice: discountedUsd,
    });

    return NextResponse.json({ success: true, quote: updated });
  }

  if (action === "share") {
    const shareRuntime = agentRuntime.runtime;
    if (!shareRuntime) {
      return NextResponse.json(
        { error: "Runtime not initialized" },
        { status: 500 },
      );
    }
    const shareQuoteService =
      shareRuntime.getService<QuoteService>("QuoteService");
    if (!shareQuoteService) {
      return NextResponse.json(
        { error: "QuoteService not available" },
        { status: 500 },
      );
    }
    const quote = await shareQuoteService.getQuoteByQuoteId(quoteId);
    if (!quote) {
      return NextResponse.json({ error: "Quote not found" }, { status: 404 });
    }
    const shareData = await DealCompletionService.generateShareData(quoteId);

    console.log("[Deal Completion] Deal shared", {
      quoteId,
      platform: body.platform || "general",
    });

    return NextResponse.json({ success: true, shareData });
  }

  return NextResponse.json({ error: "Invalid action" }, { status: 400 });
}

async  = new URL(request.url);
  const wallet = searchParams.get("wallet");

  if (!wallet) {
    return NextResponse.json({ error: "wallet required" }, { status: 400 });
  }

  const entityId = walletToEntityId(wallet);
  const normalizedWallet = wallet.toLowerCase();
  console.log("[Deal Completion GET] Querying deals:", {
    wallet,
    normalizedWallet,
    entityId,
  });

  const getRuntime = agentRuntime.runtime;
  if (!getRuntime) {
    console.warn("[Deal Completion GET] Runtime not ready");
    return NextResponse.json({
      success: true,
      deals: [],
    });
  }

  const quoteService = getRuntime.getService<QuoteService>("QuoteService");
  if (!quoteService) {
    console.warn("[Deal Completion GET] QuoteService not ready");
    return NextResponse.json({
      success: true,
      deals: [],
    });
  }

  // Get quotes by entityId
  const quotes = await quoteService.getUserQuoteHistory(entityId, 100);
  console.log("[Deal Completion GET] Got quotes by entityId:", quotes.length);

  // ALSO search by beneficiary address (for quotes indexed under wrong entityId)
  // Use beneficiary index if available, otherwise do a limited parallel search
  const beneficiaryQuoteIds = await getRuntime.getCache<string[]>(
    `beneficiary_quotes:${normalizedWallet}`,
  );
  const quotesSet = new Set(quotes.map((q) => q.quoteId));

  if (beneficiaryQuoteIds) {
    // Fast path: use beneficiary index
    const additionalQuotes = await Promise.all(
      beneficiaryQuoteIds
        .filter((id) => !quotesSet.has(id))
        .map((id) => getRuntime.getCache<QuoteMemory>(`quote:${id}`)),
    );
    for (const quote of additionalQuotes) {
      if (quote) quotes.push(quote);
    }
  } else {
    // Slow path fallback: parallel search (limited to 50 for performance)
    const allQuoteIds =
      (await getRuntime.getCache<string[]>("all_quotes")) ?? [];
    const idsToCheck = allQuoteIds
      .filter((id) => !quotesSet.has(id))
      .slice(0, 50);

    if (idsToCheck.length > 0) {
      const additionalQuotes = await Promise.all(
        idsToCheck.map((id) => getRuntime.getCache<QuoteMemory>(`quote:${id}`)),
      );
      for (const quote of additionalQuotes) {
        if (quote && quote.beneficiary === normalizedWallet) {
          quotes.push(quote);
        }
      }
    }
  }

  console.log("[Deal Completion GET] Total quotes found:", quotes.length);

  // Show active, approved, and executed deals
  // active = quote created, approved = offer created/approved on-chain, executed = paid/fulfilled
  const deals = quotes.filter(
    (quote) =>
      quote.status === "executed" ||
      quote.status === "active" ||
      quote.status === "approved",
  );
  console.log(
    "[Deal Completion GET] Filtered deals (active + approved + executed):",
    deals.length,
  );

  // Enrich deals with token metadata - batch fetch to avoid N+1 queries
  const { TokenDB, ConsignmentDB } = await import("@/services/database");

  // Collect unique IDs that need fetching
  const consignmentIdsToFetch = new Set<string>();
  const tokenIdsToFetch = new Set<string>();

  for (const deal of deals) {
    const quoteData = deal as QuoteMemory;
    if (!quoteData.tokenId && quoteData.consignmentId) {
      consignmentIdsToFetch.add(quoteData.consignmentId);
    }
    if (quoteData.tokenId && (!quoteData.tokenSymbol || !quoteData.tokenName)) {
      tokenIdsToFetch.add(quoteData.tokenId);
    }
  }

  // Batch fetch consignments and tokens in parallel
  const [consignmentResults, tokenResults] = await Promise.all([
    Promise.all(
      [...consignmentIdsToFetch].map(async (id) => {
        try {
          const consignment = await ConsignmentDB.getConsignment(id);
          return { id, data: consignment };
        } catch {
          return { id, data: null };
        }
      }),
    ),
    Promise.all(
      [...tokenIdsToFetch].map(async (id) => {
        try {
          const token = await TokenDB.getToken(id);
          return { id, data: token };
        } catch {
          return { id, data: null };
        }
      }),
    ),
  ]);

  // Build lookup maps
  const consignmentMap = new Map(consignmentResults.map((r) => [r.id, r.data]));
  const tokenMap = new Map(tokenResults.map((r) => [r.id, r.data]));

  // Also add tokens found via consignments
  for (const result of consignmentResults) {
    if (result.data?.tokenId && !tokenMap.has(result.data.tokenId)) {
      tokenIdsToFetch.add(result.data.tokenId);
    }
  }

  // Fetch any additional tokens found via consignments
  if (tokenIdsToFetch.size > tokenMap.size) {
    const additionalTokenIds = [...tokenIdsToFetch].filter(
      (id) => !tokenMap.has(id),
    );
    const additionalTokens = await Promise.all(
      additionalTokenIds.map(async (id) => {
        try {
          const token = await TokenDB.getToken(id);
          return { id, data: token };
        } catch {
          return { id, data: null };
        }
      }),
    );
    for (const { id, data } of additionalTokens) {
      tokenMap.set(id, data);
    }
  }

  // Enrich deals using pre-fetched data
  const enrichedDeals = deals.map((deal) => {
    const quoteData = deal as QuoteMemory;
    let tokenSymbol = quoteData.tokenSymbol;
    let tokenName = quoteData.tokenName;
    let tokenLogoUrl = quoteData.tokenLogoUrl;
    let tokenId = quoteData.tokenId;
    let chain: string | undefined = deal.chain;
    const consignmentId = quoteData.consignmentId;

    // If quote doesn't have token metadata, try consignment lookup
    if (!tokenId && consignmentId) {
      const consignment = consignmentMap.get(consignmentId);
      if (consignment) {
        tokenId = consignment.tokenId;
        chain = consignment.chain;
      }
    }

    // Look up token by tokenId if we still need metadata
    if (tokenId && (!tokenSymbol || !tokenName)) {
      const token = tokenMap.get(tokenId);
      if (token) {
        tokenSymbol = tokenSymbol || token.symbol;
        tokenName = tokenName || token.name;
        tokenLogoUrl = tokenLogoUrl || token.logoUrl;
      }
    }

    return {
      ...deal,
      tokenSymbol,
      tokenName,
      tokenLogoUrl,
      tokenId,
      chain,
      consignmentId,
    };
  });

  // Cache for 30 seconds - deals change infrequently but should be reasonably fresh
  return NextResponse.json(
    { success: true, deals: enrichedDeals },
    {
      headers: {
        "Cache-Control": "private, s-maxage=30, stale-while-revalidate=60",
      },
    },
  );
}

} // namespace elizaos
