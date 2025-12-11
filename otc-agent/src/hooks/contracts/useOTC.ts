"use client";

import { useCallback, useMemo, useRef } from "react";
import {
  useAccount,
  useReadContract,
  useReadContracts,
  useWriteContract,
  useChainId,
  useBalance,
  usePublicClient,
} from "wagmi";
import { keccak256, stringToBytes, decodeEventLog } from "viem";
import type { Abi, Address } from "viem";
import type {
  Offer,
  ConsignmentParams,
  ConsignmentCreationResult,
} from "@/types";
import otcArtifact from "@/contracts/artifacts/contracts/OTC.sol/OTC.json";
import { getContracts, getCurrentNetwork } from "@/config/contracts";

// Helper to get OTC address from deployments or env - cached at module level
let cachedOtcAddress: Address | undefined = undefined;
let addressLogged = false;

function getOtcAddress(): Address | undefined {
  if (cachedOtcAddress !== undefined) {
    return cachedOtcAddress;
  }

  const network = getCurrentNetwork();
  const deployments = getContracts(network);

  // Get address from deployment config (includes env override handling)
  const configAddress = deployments.evm?.contracts?.otc;
  if (configAddress) {
    if (process.env.NODE_ENV === "development" && !addressLogged) {
      console.log("[useOTC] OTC address:", configAddress, "network:", network);
      addressLogged = true;
    }
    cachedOtcAddress = configAddress as Address;
    return cachedOtcAddress;
  }

  if (!addressLogged) {
    console.warn("[useOTC] No OTC address found in config");
    addressLogged = true;
  }
  return undefined;
}

// Configuration for contract reading with dynamic ABIs
interface ReadContractConfig {
  address: Address;
  abi: Abi;
  functionName: string;
  args?: readonly unknown[];
}

// Log type for transaction receipts with topics
interface TransactionLog {
  address: Address;
  data: `0x${string}`;
  topics: readonly `0x${string}`[];
  blockHash: `0x${string}`;
  blockNumber: bigint;
  logIndex: number;
  transactionHash: `0x${string}`;
  transactionIndex: number;
  removed: boolean;
}

// Type-safe wrapper for readContract that handles wagmi client and dynamic ABIs
// The client type is inferred, we only need to specify the return type
// Uses type assertion to bypass viem's strict authorizationList requirement
async function readContractFromClient<T>(
  client: { readContract: (params: unknown) => Promise<unknown> },
  params: ReadContractConfig,
): Promise<T> {
  const result = await client.readContract(params);
  return result as T;
}

const erc20Abi = [
  {
    type: "function",
    name: "decimals",
    stateMutability: "view",
    inputs: [],
    outputs: [{ name: "", type: "uint8" }],
  },
  {
    type: "function",
    name: "balanceOf",
    stateMutability: "view",
    inputs: [{ name: "account", type: "address" }],
    outputs: [{ name: "", type: "uint256" }],
  },
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
] as unknown as Abi;

export function useOTC(): {
  otcAddress: Address | undefined;
  availableTokens: bigint;
  myOfferIds: bigint[];
  myOffers: (Offer & { id: bigint })[];
  openOfferIds: bigint[];
  openOffers: Offer[];
  agent: Address | undefined;
  isAgent: boolean;
  isApprover: boolean;
  usdcAddress: Address | undefined;
  ethBalanceWei?: bigint;
  usdcBalance?: bigint;
  minUsdAmount?: bigint;
  maxTokenPerOrder?: bigint;
  quoteExpirySeconds?: bigint;
  defaultUnlockDelaySeconds?: bigint;
  emergencyRefundsEnabled?: boolean;
  isLoading: boolean;
  error: unknown;
  claim: (offerId: bigint) => Promise<unknown>;
  isClaiming: boolean;
  createOffer: (params: {
    tokenAmountWei: bigint;
    discountBps: number;
    paymentCurrency: 0 | 1;
    lockupSeconds?: bigint;
  }) => Promise<unknown>;
  approveOffer: (offerId: bigint) => Promise<unknown>;
  cancelOffer: (offerId: bigint) => Promise<unknown>;
  fulfillOffer: (offerId: bigint, valueWei?: bigint) => Promise<unknown>;
  approveUsdc: (amount: bigint) => Promise<unknown>;
  emergencyRefund: (offerId: bigint) => Promise<unknown>;
  withdrawConsignment: (consignmentId: bigint) => Promise<unknown>;
  createConsignmentOnChain: (
    params: ConsignmentParams,
    onTxSubmitted?: (txHash: string) => void,
  ) => Promise<ConsignmentCreationResult>;
  approveToken: (tokenAddress: Address, amount: bigint) => Promise<unknown>;
  getTokenAddress: (tokenId: string) => Promise<Address>;
  getRequiredGasDeposit: () => Promise<bigint>;
  getRequiredPayment: (
    offerId: bigint,
    currency: "ETH" | "USDC",
  ) => Promise<bigint>;
} {
  const { address: account } = useAccount();
  const chainId = useChainId();
  // OTC address from env vars or deployment config
  const otcAddress = getOtcAddress();
  const abi = otcArtifact.abi as Abi;

  // Use wagmi's public client which automatically handles all configured chains
  const publicClient = usePublicClient();

  const enabled = Boolean(otcAddress);

  const availableTokensRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "availableTokenInventory",
    chainId,
    query: { enabled },
  });
  const minUsdRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "minUsdAmount",
    chainId,
    query: { enabled },
  });
  const maxTokenRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "maxTokenPerOrder",
    chainId,
    query: { enabled },
  });
  const expiryRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "quoteExpirySeconds",
    chainId,
    query: { enabled },
  });
  const unlockDelayRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "defaultUnlockDelaySeconds",
    chainId,
    query: { enabled },
  });

  const agentRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "agent",
    chainId,
    query: { enabled },
  });
  const approverMappingRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "isApprover",
    args: [account as Address],
    chainId,
    query: { enabled: enabled && Boolean(account) },
  });

  const myOfferIdsRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "getOffersForBeneficiary",
    args: [account as Address],
    chainId,
    query: {
      enabled: enabled && Boolean(account),
      refetchInterval: 30000, // Poll every 30s for offer updates (15s was too aggressive)
      staleTime: 20000, // Consider data fresh for 20s
      refetchOnWindowFocus: true, // Refresh when tab becomes active
    },
  });
  // Track previous data to only log when actually changed
  const prevMyOfferIdsRef = useRef<string | null>(null);

  const myOfferIds = useMemo(() => {
    const ids = (myOfferIdsRes.data as bigint[] | undefined) ?? [];

    // Only log if data actually changed
    if (process.env.NODE_ENV === "development") {
      const idsKey = ids.map((id) => id.toString()).join(",");
      if (prevMyOfferIdsRef.current !== idsKey) {
        prevMyOfferIdsRef.current = idsKey;
        console.log(
          "[useOTC] My offer IDs from contract:",
          ids.map((id) => id.toString()),
        );
      }
    }

    return ids;
  }, [myOfferIdsRes.data]);

  // Using type assertion to avoid deep type instantiation issue
  const myOffersContracts = myOfferIds.map((id) => ({
    address: otcAddress!,
    abi,
    functionName: "offers" as const,
    args: [id] as const,
    chainId,
  }));

  // Type assertion to avoid deep type instantiation with wagmi's complex generics
  // wagmi's useReadContracts has deeply nested generics that cause TS2589
  const myOffersConfig = {
    contracts: myOffersContracts,
    query: {
      enabled: enabled && myOfferIds.length > 0,
      refetchInterval: 30000, // Poll every 30s for offer status changes
      staleTime: 20000, // Consider data fresh for 20s
      refetchOnWindowFocus: true, // Refresh when tab becomes active
    },
  };
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  const myOffersRes = useReadContracts(myOffersConfig as any);

  const openOfferIdsRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "getOpenOfferIds",
    chainId,
    query: { enabled },
  });
  // Using type assertion to avoid deep type instantiation issue
  const openOfferIds = (openOfferIdsRes.data as bigint[] | undefined) ?? [];
  const openOffersContracts = openOfferIds.map((id) => ({
    address: otcAddress!,
    abi,
    functionName: "offers" as const,
    args: [id] as const,
    chainId,
  }));

  // Type assertion to avoid deep type instantiation with wagmi's complex generics
  const openOffersConfig = {
    contracts: openOffersContracts,
    query: {
      enabled:
        enabled &&
        Array.isArray(openOfferIdsRes.data) &&
        openOfferIds.length > 0,
    },
  };
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  const openOffersRes = useReadContracts(openOffersConfig as any);

  const { writeContractAsync: writeContractAsyncBase, isPending } =
    useWriteContract();

  // Wrapper to handle writeContractAsync with dynamic ABIs
  // wagmi's types require chain/account which are inferred from context
  function writeContractAsync(
    config: ReadContractConfig & { value?: bigint },
  ): Promise<`0x${string}`> {
    return writeContractAsyncBase(
      config as unknown as Parameters<typeof writeContractAsyncBase>[0],
    );
  }

  const claim = useCallback(
    async (offerId: bigint) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "claim",
        args: [offerId],
      });
    },
    [otcAddress, account, abi],
  );

  const createOffer = useCallback(
    async (params: {
      tokenAmountWei: bigint;
      discountBps: number;
      paymentCurrency: 0 | 1;
      lockupSeconds?: bigint;
    }) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "createOffer",
        args: [
          params.tokenAmountWei,
          BigInt(params.discountBps),
          params.paymentCurrency,
          params.lockupSeconds ?? 0n,
        ],
      });
    },
    [otcAddress, account, abi],
  );

  const approveOffer = useCallback(
    async (offerId: bigint) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "approveOffer",
        args: [offerId],
      });
    },
    [otcAddress, account, abi],
  );

  const cancelOffer = useCallback(
    async (offerId: bigint) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "cancelOffer",
        args: [offerId],
      });
    },
    [otcAddress, account, abi],
  );

  const fulfillOffer = useCallback(
    async (offerId: bigint, valueWei?: bigint) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "fulfillOffer",
        args: [offerId],
        value: valueWei,
      });
    },
    [otcAddress, account, abi],
  );

  const usdcAddressRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "usdc",
    chainId,
    query: { enabled },
  });
  const usdcAddress = (usdcAddressRes.data as Address | undefined) ?? undefined;
  const usdcBalanceRes = useReadContract({
    address: usdcAddress,
    abi: erc20Abi,
    functionName: "balanceOf",
    args: [otcAddress as Address],
    chainId,
    query: { enabled: enabled && Boolean(usdcAddress) && Boolean(otcAddress) },
  });
  const ethBalRes = useBalance({
    address: otcAddress as Address,
    chainId,
    query: { enabled: enabled && Boolean(otcAddress) },
  });

  const approveUsdc = useCallback(
    async (amount: bigint) => {
      if (!otcAddress || !usdcAddress) throw new Error("Missing addresses");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: usdcAddress,
        abi: erc20Abi,
        functionName: "approve",
        args: [otcAddress, amount],
      });
    },
    [otcAddress, usdcAddress, account],
  );

  const emergencyRefund = useCallback(
    async (offerId: bigint) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "emergencyRefund",
        args: [offerId],
      });
    },
    [otcAddress, account, abi],
  );

  const withdrawConsignment = useCallback(
    async (consignmentId: bigint) => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");
      return writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "withdrawConsignment",
        args: [consignmentId],
      });
    },
    [otcAddress, account, abi],
  );

  const createConsignmentOnChain = useCallback(
    async (
      params: ConsignmentParams,
      onTxSubmitted?: (txHash: string) => void,
    ): Promise<ConsignmentCreationResult> => {
      if (!otcAddress) throw new Error("No OTC address");
      if (!account) throw new Error("No wallet connected");

      // Compute the contract tokenId (keccak256 of the symbol)
      // The symbol is passed directly from the selected token, no DB lookup needed
      const tokenIdBytes32 = keccak256(stringToBytes(params.tokenSymbol));

      const txHash = await writeContractAsync({
        address: otcAddress,
        abi,
        functionName: "createConsignment",
        args: [
          tokenIdBytes32,
          params.amount,
          params.isNegotiable,
          params.fixedDiscountBps,
          params.fixedLockupDays,
          params.minDiscountBps,
          params.maxDiscountBps,
          params.minLockupDays,
          params.maxLockupDays,
          params.minDealAmount,
          params.maxDealAmount,
          params.isFractionalized,
          params.isPrivate,
          params.maxPriceVolatilityBps,
          params.maxTimeToExecute,
        ],
        value: params.gasDeposit,
      });

      // Notify caller that tx was submitted (before waiting for receipt)
      // This allows UI to update immediately with tx hash
      console.log("[useOTC] Transaction submitted:", txHash);
      if (onTxSubmitted) {
        onTxSubmitted(txHash);
      }

      // Wait for transaction receipt and parse the consignmentId from the event
      // Use manual polling to avoid RPC timeout/rate limit issues
      console.log("[useOTC] Waiting for transaction receipt:", txHash);
      if (!publicClient) {
        throw new Error("Public client not available");
      }

      // Quick poll for receipt - if not found fast, use fallback ID
      // Backend can resolve actual consignmentId later via tx hash
      let receipt = null;
      const maxAttempts = 5; // 5 attempts * 2 seconds = 10 seconds max
      const pollInterval = 2000;

      for (let attempt = 1; attempt <= maxAttempts; attempt++) {
        try {
          receipt = await publicClient.getTransactionReceipt({
            hash: txHash as `0x${string}`,
          });
          if (receipt) {
            console.log(`[useOTC] Receipt found on attempt ${attempt}`);
            break;
          }
        } catch {
          // Receipt not found yet
        }

        if (attempt < maxAttempts) {
          await new Promise((resolve) => setTimeout(resolve, pollInterval));
        }
      }

      if (!receipt) {
        console.warn(
          `[useOTC] No receipt after ${maxAttempts} attempts, using fallback`,
        );
      }

      // If we got a receipt, parse the consignment ID from the event
      if (receipt) {
        console.log(
          "[useOTC] Receipt received, parsing ConsignmentCreated event",
        );
        console.log("[useOTC] Receipt logs count:", receipt.logs.length);
        console.log("[useOTC] OTC contract address:", otcAddress);

        // Find ConsignmentCreated event from the OTC contract
        const logs = receipt.logs as TransactionLog[];
        const consignmentCreatedEvent = logs.find((log) => {
          if (log.address.toLowerCase() !== otcAddress.toLowerCase()) {
            return false;
          }

          try {
            const decoded = decodeEventLog({
              abi,
              data: log.data,
              topics: log.topics as [`0x${string}`, ...`0x${string}`[]],
            });
            console.log("[useOTC] Decoded event:", decoded.eventName);
            return decoded.eventName === "ConsignmentCreated";
          } catch (e) {
            console.log("[useOTC] Failed to decode log:", e);
            return false;
          }
        });

        if (consignmentCreatedEvent) {
          const decoded = decodeEventLog({
            abi,
            data: consignmentCreatedEvent.data,
            topics: consignmentCreatedEvent.topics as [
              `0x${string}`,
              ...`0x${string}`[],
            ],
          });

          const args = decoded.args as unknown as Record<string, unknown>;
          const consignmentId = args.consignmentId as bigint;
          console.log(
            "[useOTC] Consignment created with ID:",
            consignmentId.toString(),
          );

          return { txHash: txHash as `0x${string}`, consignmentId };
        }

        console.warn(
          "[useOTC] No ConsignmentCreated event found in receipt, using tx hash as ID",
        );
      } else {
        console.warn(
          "[useOTC] Could not get receipt (timeout/rate limit), using tx hash as ID",
        );
      }

      // Fallback: use a hash of the tx hash as the consignment ID
      // This allows the flow to continue even if RPC is rate limited
      // The actual consignment ID can be looked up later from the tx
      const fallbackId = BigInt(txHash.slice(0, 18));
      console.log(
        "[useOTC] Using fallback consignment ID:",
        fallbackId.toString(),
      );
      return { txHash: txHash as `0x${string}`, consignmentId: fallbackId };
    },
    [otcAddress, account, abi, publicClient],
  );

  const approveToken = useCallback(
    async (tokenAddress: Address, amount: bigint) => {
      if (!account) throw new Error("No wallet connected");
      if (!otcAddress) throw new Error("OTC contract address not configured");

      const network = getCurrentNetwork();
      console.log("[useOTC] approveToken - network config:", network);
      console.log("[useOTC] approveToken - token:", tokenAddress);
      console.log("[useOTC] approveToken - spender (OTC):", otcAddress);
      console.log("[useOTC] approveToken - wallet chainId:", chainId);
      console.log("[useOTC] approveToken - amount:", amount.toString());

      // Warn if there might be a network mismatch
      const isMainnetConfig = network === "mainnet";
      const isMainnetWallet = chainId === 8453; // Base mainnet
      const isTestnetWallet = chainId === 84532; // Base Sepolia

      if (isMainnetConfig && isTestnetWallet) {
        console.error(
          "[useOTC] NETWORK MISMATCH: App is configured for mainnet but wallet is on Base Sepolia",
        );
        throw new Error(
          "Network mismatch: Please switch your wallet to Base mainnet",
        );
      }
      if (!isMainnetConfig && isMainnetWallet) {
        console.error(
          "[useOTC] NETWORK MISMATCH: App is configured for testnet but wallet is on Base mainnet",
        );
        throw new Error(
          "Network mismatch: Please switch your wallet to Base Sepolia (testnet) or set NEXT_PUBLIC_NETWORK=mainnet",
        );
      }

      return writeContractAsync({
        address: tokenAddress,
        abi: erc20Abi,
        functionName: "approve",
        args: [otcAddress, amount],
      });
    },
    [account, otcAddress, chainId],
  );

  // Helper to extract contract address from tokenId format: "token-{chain}-{address}"
  const extractContractAddress = useCallback((tokenId: string): Address => {
    const parts = tokenId.split("-");
    if (parts.length >= 3) {
      // Format is: token-chain-address, so join everything after the second dash
      return parts.slice(2).join("-") as Address;
    }
    // Fallback: assume it's already an address
    return tokenId as Address;
  }, []);

  const getTokenAddress = useCallback(
    async (tokenId: string): Promise<Address> => {
      // Simply extract the contract address from the tokenId
      // The tokenId format is "token-{chain}-{contractAddress}"
      return extractContractAddress(tokenId);
    },
    [extractContractAddress],
  );

  const getRequiredGasDeposit = useCallback(async (): Promise<bigint> => {
    if (!otcAddress) throw new Error("No OTC address");
    if (!publicClient) throw new Error("Public client not available");
    return readContractFromClient<bigint>(publicClient, {
      address: otcAddress,
      abi,
      functionName: "requiredGasDepositPerConsignment",
    });
  }, [otcAddress, publicClient, abi]);

  // Helper to get exact required payment amount
  const getRequiredPayment = useCallback(
    async (offerId: bigint, currency: "ETH" | "USDC"): Promise<bigint> => {
      if (!otcAddress) {
        throw new Error("OTC address not configured");
      }
      if (!publicClient) {
        throw new Error("Public client not available");
      }
      const functionName =
        currency === "ETH" ? "requiredEthWei" : "requiredUsdcAmount";
      return readContractFromClient<bigint>(publicClient, {
        address: otcAddress,
        abi,
        functionName,
        args: [offerId],
      });
    },
    [otcAddress, publicClient, abi],
  );

  const agentAddr = (agentRes.data as Address | undefined) ?? undefined;
  const isAgent =
    !!account &&
    !!agentAddr &&
    (account as string).toLowerCase() === (agentAddr as string).toLowerCase();
  const isWhitelisted = Boolean(approverMappingRes.data as boolean | undefined);
  const isApprover = isAgent || isWhitelisted;

  // Check if emergency refunds are enabled
  const emergencyRefundsRes = useReadContract({
    address: otcAddress,
    abi,
    functionName: "emergencyRefundsEnabled",
    chainId,
    query: { enabled },
  });

  // Track previous offers data to only log on actual changes
  const prevMyOffersDataRef = useRef<string | null>(null);

  const myOffers: (Offer & { id: bigint })[] = useMemo(() => {
    const base = myOffersRes.data ?? [];

    const offers = myOfferIds.map((id, idx) => {
      const rawResult = base[idx]?.result;

      // Contract returns array: [consignmentId, tokenId, beneficiary, tokenAmount, discountBps, createdAt, unlockTime,
      //   priceUsdPerToken, maxPriceDeviation, ethUsdPrice, currency, approved, paid, fulfilled, cancelled, payer, amountPaid]
      if (Array.isArray(rawResult)) {
        const [
          consignmentId,
          tokenId,
          beneficiary,
          tokenAmount,
          discountBps,
          createdAt,
          unlockTime,
          priceUsdPerToken,
          maxPriceDeviation,
          ethUsdPrice,
          currency,
          approved,
          paid,
          fulfilled,
          cancelled,
          payer,
          amountPaid,
        ] = rawResult;

        return {
          id,
          consignmentId,
          tokenId,
          beneficiary,
          tokenAmount,
          discountBps,
          createdAt,
          unlockTime,
          priceUsdPerToken,
          maxPriceDeviation,
          ethUsdPrice,
          currency,
          approved,
          paid,
          fulfilled,
          cancelled,
          payer,
          amountPaid,
        } as Offer & { id: bigint };
      }

      return {
        id,
        paid: false,
        fulfilled: false,
        cancelled: false,
      } as Offer & { id: bigint };
    });

    // Only log when offers data actually changes
    if (process.env.NODE_ENV === "development" && offers.length > 0) {
      const offersKey = offers
        .map((o) => `${o.id}:${o.paid}:${o.fulfilled}`)
        .join(",");
      if (prevMyOffersDataRef.current !== offersKey) {
        prevMyOffersDataRef.current = offersKey;
        const paidOffers = offers.filter((o) => o.paid);
        console.log("[useOTC] Offers updated:", {
          total: offers.length,
          paid: paidOffers.length,
          ids: paidOffers.map((o) => o.id.toString()),
        });
      }
    }

    return offers;
  }, [myOfferIds, myOffersRes.data]);

  return {
    otcAddress,
    availableTokens: (availableTokensRes.data as bigint | undefined) ?? 0n,
    myOfferIds,
    myOffers,
    openOfferIds: ((openOfferIdsRes.data as bigint[] | undefined) ??
      []) as bigint[],
    openOffers: (openOffersRes.data ?? [])
      .map((x) => x?.result as Offer | undefined)
      .filter((x): x is Offer => x !== undefined),
    agent: agentAddr,
    isAgent,
    isApprover,
    usdcAddress,
    ethBalanceWei: (ethBalRes.data?.value as bigint | undefined) ?? undefined,
    usdcBalance: (usdcBalanceRes.data as bigint | undefined) ?? undefined,
    minUsdAmount: (minUsdRes.data as bigint | undefined) ?? undefined,
    maxTokenPerOrder: (maxTokenRes.data as bigint | undefined) ?? undefined,
    quoteExpirySeconds: (expiryRes.data as bigint | undefined) ?? undefined,
    defaultUnlockDelaySeconds:
      (unlockDelayRes.data as bigint | undefined) ?? undefined,
    emergencyRefundsEnabled:
      (emergencyRefundsRes.data as boolean | undefined) ?? false,
    isLoading:
      availableTokensRes.isLoading ||
      myOfferIdsRes.isLoading ||
      myOffersRes.isLoading ||
      usdcBalanceRes.isLoading ||
      ethBalRes.isLoading,
    error:
      availableTokensRes.error ||
      myOfferIdsRes.error ||
      myOffersRes.error ||
      usdcBalanceRes.error ||
      ethBalRes.error,
    claim,
    isClaiming: isPending,
    createOffer,
    approveOffer,
    cancelOffer,
    fulfillOffer,
    approveUsdc,
    emergencyRefund,
    withdrawConsignment,
    createConsignmentOnChain,
    approveToken,
    getTokenAddress,
    getRequiredGasDeposit,
    getRequiredPayment,
  } as const;
}
