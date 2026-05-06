"use client";

import { useState, useEffect, useRef, useCallback } from "react";
import { useRouter } from "next/navigation";
import {
  Check,
  AlertCircle,
  Loader2,
  ArrowLeft,
  ExternalLink,
} from "lucide-react";
import { Button } from "../button";

interface StepState {
  id: string;
  label: string;
  status: "pending" | "processing" | "complete" | "error";
  statusMessage?: string;
  errorMessage?: string;
  txHash?: string;
  canRetry?: boolean;
}

interface SubmissionStepProps {
  formData: {
    tokenId: string;
    amount: string;
    isNegotiable: boolean;
    fixedDiscountBps: number;
    fixedLockupDays: number;
    minDiscountBps: number;
    maxDiscountBps: number;
    minLockupDays: number;
    maxLockupDays: number;
    minDealAmount: string;
    maxDealAmount: string;
    isFractionalized: boolean;
    isPrivate: boolean;
    maxPriceVolatilityBps: number;
    maxTimeToExecuteSeconds: number;
  };
  consignerAddress: string;
  chain: string;
  activeFamily: "evm" | "solana" | null;
  selectedTokenDecimals: number;
  selectedTokenSymbol: string;
  selectedTokenName?: string;
  selectedTokenAddress?: string;
  selectedTokenLogoUrl?: string | null;
  onApproveToken: () => Promise<string>;
  onCreateConsignment: (
    onTxSubmitted?: (txHash: string) => void,
  ) => Promise<{ txHash: string; consignmentId: string }>;
  getBlockExplorerUrl: (txHash: string) => string;
  onBack: () => void;
}

export function SubmissionStepComponent({
  formData,
  consignerAddress,
  chain,
  activeFamily,
  selectedTokenDecimals,
  selectedTokenSymbol,
  selectedTokenName,
  selectedTokenAddress,
  selectedTokenLogoUrl,
  onApproveToken,
  onCreateConsignment,
  getBlockExplorerUrl,
  onBack,
}: SubmissionStepProps) {
  const router = useRouter();

  // Use refs to avoid stale closure issues in async callbacks
  const contractConsignmentIdRef = useRef<string | null>(null);
  const isProcessingRef = useRef(false);
  const hasStartedRef = useRef(false);

  const [isComplete, setIsComplete] = useState(false);
  const [, forceUpdate] = useState(0);

  // Define steps - use ref so it's stable and doesn't cause stale closure issues
  const stepsRef = useRef<StepState[]>([
    // Solana: Token transfer is part of the createConsignment instruction (no separate approval)
    // EVM: Needs separate approval step
    ...(activeFamily !== "solana"
      ? [
          {
            id: "approve",
            label: "Approve Token Transfer",
            status: "pending" as const,
            canRetry: true,
          },
        ]
      : []),
    {
      id: "create-onchain",
      label: "Create Consignment On-Chain",
      status: "pending" as const,
      canRetry: true,
    },
    {
      id: "save-db",
      label: "Save to Database",
      status: "pending" as const,
      canRetry: true,
    },
  ]);

  const updateStepStatus = useCallback(
    (stepId: string, updates: Partial<StepState>) => {
      const step = stepsRef.current.find((s) => s.id === stepId);
      if (step) {
        Object.assign(step, updates);
        forceUpdate((n) => n + 1); // Trigger re-render
      }
    },
    [],
  );

  // Retry transient errors with exponential backoff
  const executeWithRetry = useCallback(
    async <T,>(action: () => Promise<T>, stepId: string): Promise<T> => {
      const MAX_RETRIES = 3;
      const NON_RETRYABLE = ["rejected", "denied", "cancelled", "user"];
      const RETRYABLE = ["network", "timeout", "fetch", "connection", "rate"];

      let lastError: Error | null = null;
      for (let attempt = 0; attempt < MAX_RETRIES; attempt++) {
        try {
          return await action();
        } catch (error) {
          lastError = error instanceof Error ? error : new Error(String(error));
          const msg = lastError.message.toLowerCase();

          // Never retry user rejections
          if (NON_RETRYABLE.some((p) => msg.includes(p))) throw lastError;

          // Only retry transient errors, and only if we have retries left
          const canRetry =
            RETRYABLE.some((p) => msg.includes(p)) && attempt < MAX_RETRIES - 1;
          if (!canRetry) throw lastError;

          const delayMs = Math.pow(2, attempt) * 1000;
          console.log(
            `[SubmissionStep] ${stepId} failed, retrying in ${delayMs}ms:`,
            msg,
          );
          updateStepStatus(stepId, {
            statusMessage: `Retrying... (${attempt + 2}/${MAX_RETRIES})`,
          });
          await new Promise((resolve) => setTimeout(resolve, delayMs));
        }
      }
      throw lastError;
    },
    [updateStepStatus],
  );

  const saveToDatabase = useCallback(async () => {
    // Convert human-readable amounts to raw amounts with decimals
    const toRawAmount = (humanAmount: string): string => {
      const parsed = parseFloat(humanAmount) || 0;
      const raw = BigInt(
        Math.floor(parsed * Math.pow(10, selectedTokenDecimals)),
      );
      return raw.toString();
    };

    const response = await fetch("/api/consignments", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        ...formData,
        amount: toRawAmount(formData.amount),
        minDealAmount: toRawAmount(formData.minDealAmount),
        maxDealAmount: toRawAmount(formData.maxDealAmount),
        consignerAddress,
        chain,
        contractConsignmentId: contractConsignmentIdRef.current,
        // Token metadata - saved to DB so we don't need to fetch from chain later
        tokenSymbol: selectedTokenSymbol,
        tokenName: selectedTokenName || selectedTokenSymbol,
        tokenDecimals: selectedTokenDecimals,
        tokenAddress: selectedTokenAddress,
        tokenLogoUrl: selectedTokenLogoUrl,
      }),
    });

    if (!response.ok) {
      const data = await response
        .json()
        .catch(() => ({ error: "Unknown error" }));
      if (contractConsignmentIdRef.current) {
        throw new Error(
          `Your consignment is on-chain (ID: ${contractConsignmentIdRef.current}) but failed to save to our database. Click retry to try saving again.`,
        );
      }
      throw new Error(data.error || "Failed to save to database");
    }

    const data = await response.json();
    if (!data.success) {
      throw new Error(data.error || "Failed to save to database");
    }
  }, [
    formData,
    consignerAddress,
    chain,
    selectedTokenDecimals,
    selectedTokenSymbol,
    selectedTokenName,
    selectedTokenAddress,
    selectedTokenLogoUrl,
  ]);

  const processStep = useCallback(
    async (stepIndex: number): Promise<void> => {
      const steps = stepsRef.current;
      const currentStep = steps[stepIndex];

      if (!currentStep) {
        console.log(
          "[SubmissionStep] No step at index:",
          stepIndex,
          "- completing",
        );
        isProcessingRef.current = false;
        setIsComplete(true);
        return;
      }

      console.log(
        "[SubmissionStep] Processing step:",
        currentStep.id,
        "index:",
        stepIndex,
      );
      updateStepStatus(currentStep.id, {
        status: "processing",
        statusMessage: "Please confirm in your wallet...",
      });

      try {
        // Execute the step
        if (currentStep.id === "approve") {
          const txHash = await executeWithRetry(
            () => onApproveToken(),
            "approve",
          );
          updateStepStatus("approve", { txHash, statusMessage: "Confirmed" });
        } else if (currentStep.id === "create-onchain") {
          const result = await executeWithRetry(
            () =>
              onCreateConsignment((txHash) => {
                updateStepStatus("create-onchain", {
                  txHash,
                  statusMessage: "Confirming...",
                });
              }),
            "create-onchain",
          );
          contractConsignmentIdRef.current = result.consignmentId;
          updateStepStatus("create-onchain", {
            txHash: result.txHash,
            statusMessage: "Confirmed",
          });
        } else if (currentStep.id === "save-db") {
          updateStepStatus("save-db", { statusMessage: "Saving..." });
          await executeWithRetry(() => saveToDatabase(), "save-db");
        }

        // Mark complete
        updateStepStatus(currentStep.id, {
          status: "complete",
          statusMessage: undefined,
        });

        // Process next step
        const nextIndex = stepIndex + 1;
        if (nextIndex < steps.length) {
          console.log("[SubmissionStep] Moving to next step:", nextIndex);
          // Small delay for UI feedback, then process next
          await new Promise((resolve) => setTimeout(resolve, 300));
          await processStep(nextIndex);
        } else {
          console.log("[SubmissionStep] All steps complete");
          isProcessingRef.current = false;
          setIsComplete(true);
        }
      } catch (error) {
        const errorMessage =
          error instanceof Error ? error.message : "Unknown error";
        console.error(
          "[SubmissionStep] Step failed:",
          currentStep.id,
          errorMessage,
        );
        isProcessingRef.current = false;

        let displayError = errorMessage;
        if (errorMessage.includes("rejected")) {
          if (currentStep.id === "approve") {
            displayError =
              "Token approval was rejected. Click retry to try again.";
          } else if (currentStep.id === "create-onchain") {
            displayError =
              "Consignment creation was rejected. Your token approval is still active. Click retry to try again.";
          }
        }

        updateStepStatus(currentStep.id, {
          status: "error",
          statusMessage: undefined,
          errorMessage: displayError,
        });
      }
    },
    [
      onApproveToken,
      onCreateConsignment,
      updateStepStatus,
      executeWithRetry,
      saveToDatabase,
    ],
  );

  // Start processing on mount
  useEffect(() => {
    if (!hasStartedRef.current) {
      hasStartedRef.current = true;
      isProcessingRef.current = true;
      console.log("[SubmissionStep] Starting submission process...");
      processStep(0);
    }
  }, [processStep]);

  const retryStep = useCallback(
    (stepId: string) => {
      if (isProcessingRef.current) {
        console.log("[SubmissionStep] Already processing, ignoring retry");
        return;
      }

      const stepIndex = stepsRef.current.findIndex((s) => s.id === stepId);
      if (stepIndex === -1) return;

      console.log("[SubmissionStep] Retrying step:", stepId);
      isProcessingRef.current = true;
      updateStepStatus(stepId, {
        status: "pending",
        errorMessage: undefined,
        statusMessage: undefined,
      });
      processStep(stepIndex);
    },
    [processStep, updateStepStatus],
  );

  const handleGoToDeals = () => {
    router.push("/my-deals");
  };

  const formatAmount = (amount: string) => {
    const num = parseFloat(amount) || 0;
    return num.toLocaleString();
  };

  const steps = stepsRef.current;
  const hasError = steps.some((s) => s.status === "error");
  const isProcessing = isProcessingRef.current && !isComplete && !hasError;

  return (
    <div className="flex flex-col h-full min-h-0 overflow-y-auto space-y-6">
      {/* Header */}
      <div className="text-center pb-4 border-b border-zinc-200 dark:border-zinc-700">
        <h3 className="text-lg font-semibold text-zinc-900 dark:text-zinc-100">
          {isComplete ? "Listing Created" : "Creating Your Listing"}
        </h3>
        <p className="text-sm text-zinc-500">
          {isComplete
            ? "Your tokens are now listed on the OTC desk"
            : `Listing ${formatAmount(formData.amount)} ${selectedTokenSymbol}`}
        </p>
      </div>

      {/* Steps */}
      <div className="space-y-4">
        {steps.map((step, index) => (
          <div
            key={step.id}
            className="border border-zinc-200 dark:border-zinc-800 rounded-lg p-4"
          >
            <div className="flex items-start gap-3">
              {/* Status Icon */}
              <div className="flex-shrink-0 mt-0.5">
                {step.status === "complete" ? (
                  <div className="w-6 h-6 rounded-full bg-green-100 dark:bg-green-900/30 flex items-center justify-center">
                    <Check className="w-4 h-4 text-green-600 dark:text-green-400" />
                  </div>
                ) : step.status === "processing" ? (
                  <div className="w-6 h-6 flex items-center justify-center">
                    <Loader2 className="w-5 h-5 text-brand-500 animate-spin" />
                  </div>
                ) : step.status === "error" ? (
                  <div className="w-6 h-6 rounded-full bg-red-100 dark:bg-red-900/30 flex items-center justify-center">
                    <AlertCircle className="w-4 h-4 text-red-600 dark:text-red-400" />
                  </div>
                ) : (
                  <div className="w-6 h-6 rounded-full bg-zinc-100 dark:bg-zinc-800 flex items-center justify-center">
                    <span className="text-xs text-zinc-500">{index + 1}</span>
                  </div>
                )}
              </div>

              {/* Step Content */}
              <div className="flex-1 min-w-0">
                <div className="flex items-center justify-between gap-2">
                  <h3 className="font-medium text-sm">{step.label}</h3>
                  {step.status === "error" && step.canRetry && (
                    <Button
                      onClick={() => retryStep(step.id)}
                      color="brand"
                      className="!py-1 !px-3 !text-xs"
                    >
                      Retry
                    </Button>
                  )}
                </div>

                {/* Transaction Hash - show immediately when available */}
                {step.txHash && (
                  <a
                    href={getBlockExplorerUrl(step.txHash)}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="inline-flex items-center gap-1 text-xs text-blue-600 dark:text-blue-400 hover:underline mt-1"
                  >
                    View transaction
                    <ExternalLink className="w-3 h-3" />
                  </a>
                )}

                {/* Error Message */}
                {step.errorMessage && (
                  <p className="text-xs text-red-600 dark:text-red-400 mt-2">
                    {step.errorMessage}
                  </p>
                )}

                {/* Status Message - dynamic based on phase */}
                {step.status === "processing" && step.statusMessage && (
                  <p className="text-xs text-zinc-500 dark:text-zinc-400 mt-1">
                    {step.statusMessage}
                  </p>
                )}
              </div>
            </div>
          </div>
        ))}

        {/* Success Message */}
        {isComplete && (
          <div className="bg-green-50 dark:bg-green-900/20 border border-green-200 dark:border-green-800 rounded-lg p-4">
            <div className="flex items-center gap-2">
              <Check className="w-5 h-5 text-green-600 dark:text-green-400" />
              <div>
                <p className="font-medium text-green-900 dark:text-green-100">
                  Success
                </p>
                <p className="text-sm text-green-700 dark:text-green-300">
                  Your consignment has been created and is now live.
                </p>
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Navigation */}
      <div className="flex gap-3 pt-4 border-t border-zinc-200 dark:border-zinc-800">
        {!isComplete && !isProcessing && hasError && (
          <Button
            onClick={onBack}
            className="flex items-center gap-2 px-6 py-3 bg-zinc-100 hover:bg-zinc-200 dark:bg-zinc-800 dark:hover:bg-zinc-700 text-zinc-700 dark:text-zinc-300 rounded-xl transition-colors"
          >
            <ArrowLeft className="w-4 h-4" />
            Back to Review
          </Button>
        )}
        {isComplete && (
          <Button
            onClick={handleGoToDeals}
            color="brand"
            className="flex-1 py-3"
          >
            View My Listings
          </Button>
        )}
      </div>
    </div>
  );
}
