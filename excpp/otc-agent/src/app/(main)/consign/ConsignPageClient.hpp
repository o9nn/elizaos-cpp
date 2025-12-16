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

"use client";

import React, { useCallback, useEffect, useMemo, useState } from "react";
;
;
;
;
;
import type { TokenWithBalance } from "@/components/consignment-form/token-selection-step";

// Solana imports
import type { Idl, Wallet } from "@coral-xyz/anchor";
;
;
;
;

// Solana config
const SOLANA_RPC = SUPPORTED_CHAINS.solana.rpcUrl;
const SOLANA_DESK = SUPPORTED_CHAINS.solana.contracts.otc;

// Default gas deposit fallback (0.001 ETH) - used when RPC fetch fails
const DEFAULT_GAS_DEPOSIT = BigInt(1000000000000000);

const TokenSelectionStep = dynamic(
  () =>
    import("@/components/consignment-form/token-selection-step").then(
      (m) => m.TokenSelectionStep,
    ),
  { ssr: false },
);
const FormStep = dynamic(
  () =>
    import("@/components/consignment-form/form-step").then((m) => m.FormStep),
  { ssr: false },
);
const ReviewStep = dynamic(
  () =>
    import("@/components/consignment-form/review-step").then(
      (m) => m.ReviewStep,
    ),
  { ssr: false },
);
const SubmissionStepComponent = dynamic(
  () =>
    import("@/components/consignment-form/submission-step").then(
      (m) => m.SubmissionStepComponent,
    ),
  { ssr: false },
);



std::future<Idl> fetchSolanaIdl();

const STEP_LABELS = ["Select", "Configure", "Review", "Submit"];

const INITIAL_FORM_DATA = {
  tokenId: "",
  amount: "",
  isNegotiable: true,
  fixedDiscountBps: 1000,
  fixedLockupDays: 180,
  minDiscountBps: 500,
  maxDiscountBps: 2000,
  minLockupDays: 7,
  maxLockupDays: 365,
  minDealAmount: "1000",
  maxDealAmount: "100000",
  isFractionalized: true,
  isPrivate: false,
  maxPriceVolatilityBps: 1000,
  maxTimeToExecuteSeconds: 1800,
};

// Extract chain and address from tokenId (format: token-{chain}-{address})
;
}

default  = useMultiWallet();
  const { login, ready: privyReady } = usePrivy();
  const { createConsignmentOnChain, approveToken, getRequiredGasDeposit } =
    useOTC();

  const [step, setStep] = useState(1);
  const [selectedToken, setSelectedToken] = useState<TokenWithBalance | null>(
    null,
  );
  const [formData, setFormData] = useState(INITIAL_FORM_DATA);
  const [gasDeposit, setGasDeposit] = useState<bigint | null>(null);

  const currentAddress =
    activeFamily === "solana" ? solanaPublicKey : evmAddress;
  const displayAddress = currentAddress
    ? `${currentAddress.slice(0, 6)}...${currentAddress.slice(-4)}`
    : null;

  const requiredChain = useMemo(
    () => getRequiredChain(formData.tokenId),
    [formData.tokenId],
  );
  const { chain: tokenChain, address: rawTokenAddress } = useMemo(
    () => getTokenInfo(formData.tokenId),
    [formData.tokenId],
  );

  const isConnectedToRequiredChain = useMemo(() => {
    if (!requiredChain) return hasWallet;
    return requiredChain === "solana"
      ? activeFamily === "solana" && hasWallet
      : activeFamily === "evm" && hasWallet;
  }, [requiredChain, activeFamily, hasWallet]);

  // Reset form when chain changes (prevents stale token selection)
  useEffect(() => {
    if (step > 1 && selectedToken) {
      const tokenChainType = selectedToken.chain;
      const isTokenOnCurrentChain =
        (tokenChainType === "solana" && activeFamily === "solana") ||
        (tokenChainType !== "solana" && activeFamily === "evm");

      if (!isTokenOnCurrentChain) {
        // Token is on different chain, reset to step 1
        setStep(1);
        setSelectedToken(null);
        setFormData(INITIAL_FORM_DATA);
      }
    }
  }, [activeFamily, step, selectedToken]);

  // Pre-fetch gas deposit when we have a token selected and reach review step
  useEffect(() => {
    if (step === 3 && activeFamily !== "solana" && rawTokenAddress) {
      getRequiredGasDeposit()
        .then((deposit) => {
          console.log(
            "[ConsignPage] Gas deposit fetched:",
            deposit?.toString(),
          );
          setGasDeposit(deposit);
        })
        .catch((err) => {
          console.error("[ConsignPage] Failed to fetch gas deposit:", err);
          setGasDeposit(DEFAULT_GAS_DEPOSIT);
        });
    }
  }, [step, activeFamily, rawTokenAddress, getRequiredGasDeposit]);

  const updateFormData = useCallback((updates: Partial<typeof formData>) => {
    setFormData((prev) => ({ ...prev, ...updates }));
  }, []);

  const handleNext = useCallback(() => setStep((s) => Math.min(s + 1, 4)), []);
  const handleBack = useCallback(() => setStep((s) => Math.max(s - 1, 1)), []);

  const handleConnect = useCallback(
    (chain?: "evm" | "solana") => {
      if (chain) setActiveFamily(chain);
      if (privyAuthenticated) {
        connectWallet();
      } else {
        login();
      }
    },
    [setActiveFamily, privyAuthenticated, connectWallet, login],
  );

  const handleTokenSelect = useCallback((token: TokenWithBalance) => {
    setSelectedToken(token);
    // Auto-set deal amounts based on token balance
    const humanBalance =
      Number(BigInt(token.balance)) / Math.pow(10, token.decimals);
    const minDeal = Math.max(1, Math.floor(humanBalance * 0.01));
    const maxDeal = Math.floor(humanBalance);
    setFormData((prev) => ({
      ...prev,
      minDealAmount: minDeal.toString(),
      maxDealAmount: maxDeal.toString(),
    }));
  }, []);

  const getBlockExplorerUrl = useCallback(
    (txHash: string) => {
      if (tokenChain === "solana") {
        return `https://solscan.io/tx/${txHash}`;
      }
      if (tokenChain === "bsc") {
        return `https://bscscan.com/tx/${txHash}`;
      }
      return `https://basescan.org/tx/${txHash}`;
    },
    [tokenChain],
  );

  const handleApproveToken = useCallback(async (): Promise<string> => {
    // Solana path - approve is done as part of the transaction
    if (activeFamily === "solana") {
      return "solana-approval-pending";
    }

    // EVM path
    if (!rawTokenAddress) throw new Error("Token address not found");
    const decimals = selectedToken?.decimals ?? 18;
    const rawAmount = BigInt(
      Math.floor(parseFloat(formData.amount) * Math.pow(10, decimals)),
    );
    const txHash = await approveToken(
      rawTokenAddress as `0x${string}`,
      rawAmount,
    );
    return txHash as string;
  }, [
    activeFamily,
    rawTokenAddress,
    selectedToken,
    formData.amount,
    approveToken,
  ]);

  const handleCreateConsignment = useCallback(
    async (
      onTxSubmitted?: (txHash: string) => void,
    ): Promise<{
      txHash: string;
      consignmentId: string;
    }> => {
      const decimals = selectedToken?.decimals ?? 18;

      // Solana path
      if (activeFamily === "solana") {
        if (!SOLANA_DESK) {
          throw new Error("SOLANA_DESK address not configured in environment.");
        }
        if (!solanaWallet || !solanaWallet.publicKey) {
          throw new Error("Connect a Solana wallet to continue.");
        }
        if (!rawTokenAddress) {
          throw new Error("Token address not found");
        }

        const connection = new Connection(SOLANA_RPC, "confirmed");

        // Adapt our wallet adapter to Anchor's Wallet interface
        const anchorWallet = {
          publicKey: new SolPubkey(solanaWallet.publicKey.toBase58()),
          signTransaction: solanaWallet.signTransaction,
          signAllTransactions: solanaWallet.signAllTransactions,
        } as Wallet;

        const provider = new anchor.AnchorProvider(connection, anchorWallet, {
          commitment: "confirmed",
        });

        console.log("[ConsignPage] Fetching Solana IDL...");
        const idl = await fetchSolanaIdl();
        console.log("[ConsignPage] IDL loaded, creating program...");
        const program = new anchor.Program(idl, provider);

        const desk = new SolPubkey(SOLANA_DESK);
        const tokenMintPk = new SolPubkey(rawTokenAddress);
        const consignerPk = new SolPubkey(solanaWallet.publicKey.toBase58());

        // Get consigner's token ATA
        const consignerTokenAta = await getAssociatedTokenAddress(
          tokenMintPk,
          consignerPk,
          false,
        );

        // Get desk's token treasury
        const deskTokenTreasury = await getAssociatedTokenAddress(
          tokenMintPk,
          desk,
          true,
        );

        // Generate consignment keypair (required as signer in the program)
        const consignmentKeypair = Keypair.generate();

        // Convert amounts to raw values
        const rawAmount = new anchor.BN(
          Math.floor(
            parseFloat(formData.amount) * Math.pow(10, decimals),
          ).toString(),
        );
        const rawMinDeal = new anchor.BN(
          Math.floor(
            parseFloat(formData.minDealAmount) * Math.pow(10, decimals),
          ).toString(),
        );
        const rawMaxDeal = new anchor.BN(
          Math.floor(
            parseFloat(formData.maxDealAmount) * Math.pow(10, decimals),
          ).toString(),
        );

        // Call createConsignment instruction
        const txSignature = await program.methods
          .createConsignment(
            rawAmount,
            formData.isNegotiable,
            formData.fixedDiscountBps ?? 0,
            formData.fixedLockupDays ?? 0,
            formData.minDiscountBps,
            formData.maxDiscountBps,
            formData.minLockupDays,
            formData.maxLockupDays,
            rawMinDeal,
            rawMaxDeal,
            formData.isFractionalized,
            formData.isPrivate,
            formData.maxPriceVolatilityBps,
            new anchor.BN(formData.maxTimeToExecuteSeconds),
          )
          .accounts({
            desk: desk,
            consigner: consignerPk,
            tokenMint: tokenMintPk,
            consignerTokenAta: consignerTokenAta,
            deskTokenTreasury: deskTokenTreasury,
            consignment: consignmentKeypair.publicKey,
            tokenProgram: TOKEN_PROGRAM_ID,
            systemProgram: SolSystemProgram.programId,
          })
          .signers([consignmentKeypair])
          .rpc();

        // Notify that tx was submitted (Solana confirms fast so this is immediate)
        if (onTxSubmitted) {
          onTxSubmitted(txSignature);
        }

        return {
          txHash: txSignature,
          consignmentId: consignmentKeypair.publicKey.toString(),
        };
      }

      // EVM path - use cached gas deposit or fetch with retry
      let currentGasDeposit = gasDeposit;
      if (!currentGasDeposit) {
        console.log("[ConsignPage] Gas deposit not cached, fetching...");
        for (let attempt = 0; attempt < 3; attempt++) {
          try {
            currentGasDeposit = await getRequiredGasDeposit();
            if (currentGasDeposit) {
              console.log(
                "[ConsignPage] Gas deposit fetched:",
                currentGasDeposit.toString(),
              );
              break;
            }
          } catch (err) {
            console.error("[ConsignPage] Gas deposit fetch failed:", err);
          }
          if (attempt < 2) {
            await new Promise((resolve) =>
              setTimeout(resolve, Math.pow(2, attempt) * 1000),
            );
          }
        }
        currentGasDeposit ??= DEFAULT_GAS_DEPOSIT;
      }

      // Convert human-readable amounts to raw amounts with decimals
      const rawAmount = BigInt(
        Math.floor(parseFloat(formData.amount) * Math.pow(10, decimals)),
      );
      const rawMinDeal = BigInt(
        Math.floor(parseFloat(formData.minDealAmount) * Math.pow(10, decimals)),
      );
      const rawMaxDeal = BigInt(
        Math.floor(parseFloat(formData.maxDealAmount) * Math.pow(10, decimals)),
      );

      const result = await createConsignmentOnChain(
        {
          tokenId: formData.tokenId,
          tokenSymbol: selectedToken?.symbol ?? "TOKEN",
          amount: rawAmount,
          isNegotiable: formData.isNegotiable,
          fixedDiscountBps: formData.fixedDiscountBps ?? 0,
          fixedLockupDays: formData.fixedLockupDays ?? 0,
          minDiscountBps: formData.minDiscountBps,
          maxDiscountBps: formData.maxDiscountBps,
          minLockupDays: formData.minLockupDays,
          maxLockupDays: formData.maxLockupDays,
          minDealAmount: rawMinDeal,
          maxDealAmount: rawMaxDeal,
          isFractionalized: formData.isFractionalized,
          isPrivate: formData.isPrivate,
          maxPriceVolatilityBps: formData.maxPriceVolatilityBps,
          maxTimeToExecute: formData.maxTimeToExecuteSeconds,
          gasDeposit: currentGasDeposit,
        },
        onTxSubmitted,
      );

      return {
        txHash: result.txHash,
        consignmentId: result.consignmentId.toString(),
      };
    },
    [
      activeFamily,
      solanaWallet,
      rawTokenAddress,
      selectedToken,
      formData,
      gasDeposit,
      createConsignmentOnChain,
      getRequiredGasDeposit,
    ],
  );

  return (
    <main className="flex-1 px-3 sm:px-4 md:px-6 py-4 sm:py-6 md:py-8">
      <div className="max-w-3xl mx-auto">
        <h1 className="text-2xl sm:text-3xl font-bold mb-2">
          List Your Tokens
        </h1>
        <p className="text-sm sm:text-base text-zinc-600 dark:text-zinc-400 mb-4">
          Sell your tokens at a discount with a lockup period
        </p>

        {hasWallet && (
          <div className="mb-6 p-4 rounded-lg border border-zinc-200 dark:border-zinc-800 bg-zinc-50 dark:bg-zinc-900/50">
            <div className="flex items-center justify-between gap-4 flex-wrap">
              <div className="flex items-center gap-3">
                <div className="w-8 h-8 rounded-full bg-gradient-to-br from-brand-500 to-brand-400 flex items-center justify-center">
                  <span className="text-white text-xs font-bold">
                    {activeFamily === "solana" ? "S" : "E"}
                  </span>
                </div>
                <div>
                  <p className="text-sm font-medium text-zinc-900 dark:text-zinc-100">
                    {displayAddress || "Not connected"}
                  </p>
                  <p className="text-xs text-zinc-500 dark:text-zinc-400">
                    {networkLabel}
                  </p>
                </div>
              </div>
              {!isFarcasterContext && (
                <button
                  onClick={disconnect}
                  className="px-3 py-1.5 text-xs font-medium rounded-lg border border-zinc-300 dark:border-zinc-600 text-zinc-600 dark:text-zinc-400 hover:bg-zinc-100 dark:hover:bg-zinc-800 transition-colors"
                >
                  Disconnect
                </button>
              )}
            </div>
          </div>
        )}

        {/* Progress indicator */}
        <div className="mb-6 sm:mb-8">
          <div className="flex justify-between items-center mb-2">
            {[1, 2, 3, 4].map((s) => (
              <div
                key={s}
                className={`flex-1 h-2 ${
                  s <= step ? "bg-brand-500" : "bg-zinc-200 dark:bg-zinc-800"
                } ${s < 4 ? "mr-2" : ""} rounded-full transition-colors`}
              />
            ))}
          </div>
          <div className="flex justify-between text-xs text-zinc-600 dark:text-zinc-400">
            {STEP_LABELS.map((label, idx) => (
              <span
                key={label}
                className={step === idx + 1 ? "text-brand-500 font-medium" : ""}
              >
                {label}
              </span>
            ))}
          </div>
        </div>

        {/* Form steps */}
        <div className="bg-white dark:bg-zinc-900 rounded-xl border border-zinc-200 dark:border-zinc-800 p-4 sm:p-6 max-h-[calc(100dvh-220px)] sm:max-h-[calc(100dvh-280px)] flex flex-col overflow-hidden">
          {step === 1 && (
            <TokenSelectionStep
              formData={formData}
              updateFormData={updateFormData}
              onNext={handleNext}
              onTokenSelect={handleTokenSelect}
            />
          )}
          {step === 2 && (
            <FormStep
              formData={formData}
              updateFormData={updateFormData}
              onNext={handleNext}
              onBack={handleBack}
              selectedTokenBalance={selectedToken?.balance}
              selectedTokenDecimals={selectedToken?.decimals}
              selectedTokenSymbol={selectedToken?.symbol}
            />
          )}
          {step === 3 && (
            <ReviewStep
              formData={formData}
              onBack={handleBack}
              onNext={handleNext}
              requiredChain={requiredChain}
              isConnectedToRequiredChain={isConnectedToRequiredChain}
              onConnect={() => handleConnect(requiredChain || undefined)}
              privyReady={privyReady}
              selectedTokenSymbol={selectedToken?.symbol}
              selectedTokenDecimals={selectedToken?.decimals}
            />
          )}
          {step === 4 && (
            <SubmissionStepComponent
              formData={formData}
              consignerAddress={
                activeFamily === "solana"
                  ? solanaPublicKey || ""
                  : evmAddress || ""
              }
              chain={
                tokenChain || (activeFamily === "solana" ? "solana" : "base")
              }
              activeFamily={activeFamily}
              selectedTokenDecimals={selectedToken?.decimals ?? 18}
              selectedTokenSymbol={selectedToken?.symbol ?? "TOKEN"}
              selectedTokenName={selectedToken?.name}
              selectedTokenAddress={selectedToken?.contractAddress}
              selectedTokenLogoUrl={selectedToken?.logoUrl}
              onApproveToken={handleApproveToken}
              onCreateConsignment={handleCreateConsignment}
              getBlockExplorerUrl={getBlockExplorerUrl}
              onBack={handleBack}
            />
          )}
        </div>
      </div>
    </main>
  );
}

} // namespace elizaos
