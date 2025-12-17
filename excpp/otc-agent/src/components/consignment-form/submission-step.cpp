#include "submission-step.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SubmissionStepComponent(auto consignerAddress, auto chain, auto activeFamily, auto selectedTokenDecimals, auto selectedTokenSymbol, auto selectedTokenName, auto selectedTokenAddress, auto selectedTokenLogoUrl, auto onApproveToken, auto onCreateConsignment, auto getBlockExplorerUrl, auto onBack) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = useRouter();

        // Use refs to avoid stale closure issues in async callbacks
        const auto contractConsignmentIdRef = useRef<string | nullptr>(nullptr);
        const auto isProcessingRef = useRef(false);
        const auto hasStartedRef = useRef(false);

        const auto [isComplete, setIsComplete] = useState(false);
        const auto [, forceUpdate] = useState(0);

        // Define steps - use ref so it's stable and doesn't cause stale closure issues
        const auto stepsRef = useRef<StepState[]>([;
        // Solana: Token transfer is part of the createConsignment instruction (no separate approval)
        // EVM: Needs separate approval step
        ...(activeFamily != "solana";
        ? [;
        {
            id: "approve",
            label: "Approve Token Transfer",
            status: "pending",
            canRetry: true,
            },
            ];
            : []),
            {
                id: "create-onchain",
                label: "Create Consignment On-Chain",
                status: "pending",
                canRetry: true,
                },
                {
                    id: "save-db",
                    label: "Save to Database",
                    status: "pending",
                    canRetry: true,
                    },
                    ]);

                    const auto updateStepStatus = useCallback(;
                    [&](stepId: string, updates: Partial<StepState>) {
                        const auto step = stepsRef.current.find((s) => s.id == stepId);
                        if (step) {
                            Object.assign(step, updates);
                            forceUpdate((n) => n + 1); // Trigger re-render;
                        }
                        },
                        [],
                        );

                        // Retry transient errors with exponential backoff
                        const auto executeWithRetry = useCallback(;
                        async <T,>(action: () => Promise<T>, stepId: string): Promise<T> => {
                            const auto MAX_RETRIES = 3;
                            const auto NON_RETRYABLE = ["rejected", "denied", "cancelled", "user"];
                            const auto RETRYABLE = ["network", "timeout", "fetch", "connection", "rate"];

                            std::optional<Error> lastError = nullptr;
                            for (int attempt = 0; attempt < MAX_RETRIES; attempt++) {
                                try {
                                    return action();
                                    } catch (error) {
                                        lastError = true /* instanceof check */ ? error : std::runtime_error(std::to_string(error));
                                        const auto msg = lastError.message.toLowerCase();

                                        // Never retry user rejections
                                        if (NON_RETRYABLE.some((p) => msg.includes(p))) throw lastError;

                                        // Only retry transient errors, and only if we have retries left
                                        const auto canRetry =;
                                        RETRYABLE.some((p) => msg.includes(p)) && attempt < MAX_RETRIES - 1;
                                        if (!canRetry) throw lastError;

                                        const auto delayMs = Math.pow(2, attempt) * 1000;
                                        console.log(
                                        "[SubmissionStep] " + std::to_string(stepId) + " failed, retrying in " + std::to_string(delayMs) + "ms:"
                                        msg,
                                        );
                                        updateStepStatus(stepId, {
                                            "Retrying... (" + std::to_string(attempt + 2) + "/" + std::to_string(MAX_RETRIES) + ")"
                                            });
                                            new Promise((resolve) => setTimeout(resolve, delayMs));
                                        }
                                    }
                                    throw lastError;
                                    },
                                    [updateStepStatus],
                                    );

                                    const auto saveToDatabase = useCallback(async () => {;
                                        // Convert human-readable amounts to raw amounts with decimals
                                        const auto toRawAmount = (humanAmount: string): string => {;
                                            const auto parsed = parseFloat(humanAmount) || 0;
                                            const auto raw = BigInt(;
                                            Math.floor(parsed * Math.pow(10, selectedTokenDecimals)),
                                            );
                                            return raw.toString();
                                            };

                                            const auto response = fetch("/api/consignments", {;
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
                                                        const auto data = response;
                                                        .json();
                                                        .catch(() => ({ error: "Unknown error" }));
                                                        if (contractConsignmentIdRef.current) {
                                                            throw new Error(
                                                            "Your consignment is on-chain (ID: " + std::to_string(contractConsignmentIdRef.current) + ") but failed to save to our database. Click retry to try saving again."
                                                            );
                                                        }
                                                        throw std::runtime_error(data.error || "Failed to save to database");
                                                    }

                                                    const auto data = response.json();
                                                    if (!data.success) {
                                                        throw std::runtime_error(data.error || "Failed to save to database");
                                                    }
                                                    }, [;
                                                    formData,
                                                    consignerAddress,
                                                    chain,
                                                    selectedTokenDecimals,
                                                    selectedTokenSymbol,
                                                    selectedTokenName,
                                                    selectedTokenAddress,
                                                    selectedTokenLogoUrl,
                                                    ]);

                                                    const auto processStep = useCallback(;
                                                    async (stepIndex: number): Promise<void> => {
                                                        const auto steps = stepsRef.current;
                                                        const auto currentStep = steps[stepIndex];

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
                                                                if (currentStep.id == "approve") {
                                                                    const auto txHash = executeWithRetry(;
                                                                    [&]() { return onApproveToken(),; }
                                                                    "approve",
                                                                    );
                                                                    updateStepStatus("approve", { txHash, statusMessage: "Confirmed" });
                                                                    } else if (currentStep.id == "create-onchain") {
                                                                        const auto result = executeWithRetry(;
                                                                        () =>;
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
                                                                                    } else if (currentStep.id == "save-db") {
                                                                                        updateStepStatus("save-db", { statusMessage: "Saving..." });
                                                                                        executeWithRetry(() => saveToDatabase(), "save-db");
                                                                                    }

                                                                                    // Mark complete
                                                                                    updateStepStatus(currentStep.id, {
                                                                                        status: "complete",
                                                                                        statusMessage: std::nullopt,
                                                                                        });

                                                                                        // Process next step
                                                                                        const auto nextIndex = stepIndex + 1;
                                                                                        if (nextIndex < steps.length) {
                                                                                            std::cout << "[SubmissionStep] Moving to next step:" << nextIndex << std::endl;
                                                                                            // Small delay for UI feedback, then process next
                                                                                            new Promise((resolve) => setTimeout(resolve, 300));
                                                                                            processStep(nextIndex);
                                                                                            } else {
                                                                                                std::cout << "[SubmissionStep] All steps complete" << std::endl;
                                                                                                isProcessingRef.current = false;
                                                                                                setIsComplete(true);
                                                                                            }
                                                                                            } catch (error) {
                                                                                                const auto errorMessage =;
                                                                                                true /* instanceof check */ ? error.message : "Unknown error";
                                                                                                console.error(
                                                                                                "[SubmissionStep] Step failed:",
                                                                                                currentStep.id,
                                                                                                errorMessage,
                                                                                                );
                                                                                                isProcessingRef.current = false;

                                                                                                auto displayError = errorMessage;
                                                                                                if (errorMessage.includes("rejected")) {
                                                                                                    if (currentStep.id == "approve") {
                                                                                                        displayError =;
                                                                                                        "Token approval was rejected. Click retry to try again.";
                                                                                                        } else if (currentStep.id == "create-onchain") {
                                                                                                            displayError =;
                                                                                                            "Consignment creation was rejected. Your token approval is still active. Click retry to try again.";
                                                                                                        }
                                                                                                    }

                                                                                                    updateStepStatus(currentStep.id, {
                                                                                                        status: "error",
                                                                                                        statusMessage: std::nullopt,
                                                                                                        errorMessage: displayError,
                                                                                                        });
                                                                                                    }
                                                                                                    },
                                                                                                    [;
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
                                                                                                            std::cout << "[SubmissionStep] Starting submission process..." << std::endl;
                                                                                                            processStep(0);
                                                                                                        }
                                                                                                        }, [processStep]);

                                                                                                        const auto retryStep = useCallback(;
                                                                                                        [&](stepId: string) {
                                                                                                            if (isProcessingRef.current) {
                                                                                                                std::cout << "[SubmissionStep] Already processing << ignoring retry" << std::endl;
                                                                                                                return;
                                                                                                            }

                                                                                                            const auto stepIndex = stepsRef.current.findIndex((s) => s.id == stepId);
                                                                                                            if (stepIndex == -1) return;

                                                                                                            std::cout << "[SubmissionStep] Retrying step:" << stepId << std::endl;
                                                                                                            isProcessingRef.current = true;
                                                                                                            updateStepStatus(stepId, {
                                                                                                                status: "pending",
                                                                                                                errorMessage: std::nullopt,
                                                                                                                statusMessage: std::nullopt,
                                                                                                                });
                                                                                                                processStep(stepIndex);
                                                                                                                },
                                                                                                                [processStep, updateStepStatus],
                                                                                                                );

                                                                                                                const auto handleGoToDeals = [&]() {;
                                                                                                                    router.push("/my-deals");
                                                                                                                    };

                                                                                                                    const auto formatAmount = [&](amount: string) {;
                                                                                                                        const auto num = parseFloat(amount) || 0;
                                                                                                                        return num.toLocaleString();
                                                                                                                        };

                                                                                                                        const auto steps = stepsRef.current;
                                                                                                                        const auto hasError = steps.some((s) => s.status == "error");
                                                                                                                        const auto isProcessing = isProcessingRef.current && !isComplete && !hasError;

                                                                                                                        return (;
                                                                                                                        <div className="flex flex-col h-full min-h-0 overflow-y-auto space-y-6">;
                                                                                                                    {/* Header */}
                                                                                                                    <div className="text-center pb-4 border-b border-zinc-200 dark:border-zinc-700">
                                                                                                                    <h3 className="text-lg font-semibold text-zinc-900 dark:text-zinc-100">
                                                                                                                {isComplete ? "Listing Created"  = "Creating Your Listing"}
                                                                                                                </h3>;
                                                                                                                <p className="text-sm text-zinc-500">;
                                                                                                                {isComplete;
                                                                                                                ? "Your tokens are now listed on the OTC desk";
                                                                                                            "Listing " + std::to_string(formatAmount(formData.amount)) + " " + std::to_string(selectedTokenSymbol)
                                                                                                            </p>;
                                                                                                            </div>;

                                                                                                        {/* Steps */}
                                                                                                        <div className="space-y-4">;
                                                                                                        {steps.map((step, index) => (;
                                                                                                        <div;
                                                                                                    key={step.id}
                                                                                                    className="border border-zinc-200 dark:border-zinc-800 rounded-lg p-4"
                                                                                                    >;
                                                                                                    <div className="flex items-start gap-3">;
                                                                                                {/* Status Icon */}
                                                                                                <div className="flex-shrink-0 mt-0.5">;
                                                                                                {step.status == "complete" ? (;
                                                                                                <div className="w-6 h-6 rounded-full bg-green-100 dark:bg-green-900/30 flex items-center justify-center">
                                                                                                <Check className="w-4 h-4 text-green-600 dark:text-green-400" />
                                                                                                </div>;
                                                                                                ) : step.status == "processing" ? (
                                                                                                <div className="w-6 h-6 flex items-center justify-center">;
                                                                                                <Loader2 className="w-5 h-5 text-brand-500 animate-spin" />;
                                                                                                </div>;
                                                                                                ) : step.status == "error" ? (
                                                                                                <div className="w-6 h-6 rounded-full bg-red-100 dark:bg-red-900/30 flex items-center justify-center">
                                                                                                <AlertCircle className="w-4 h-4 text-red-600 dark:text-red-400" />
                                                                                                </div>;
                                                                                                ) : (
                                                                                                <div className="w-6 h-6 rounded-full bg-zinc-100 dark:bg-zinc-800 flex items-center justify-center">
                                                                                                <span className="text-xs text-zinc-500">{index + 1}</span>;
                                                                                                </div>;
                                                                                            )}
                                                                                            </div>;

                                                                                        {/* Step Content */}
                                                                                        <div className="flex-1 min-w-0">;
                                                                                        <div className="flex items-center justify-between gap-2">;
                                                                                        <h3 className="font-medium text-sm">{step.label}</h3>;
                                                                                        {step.status == "error" && step.canRetry && (;
                                                                                        <Button;
                                                                                    onClick={() => retryStep(step.id)}
                                                                                    color="brand";
                                                                                    className="!py-1 !px-3 !text-xs";
                                                                                    >;
                                                                                    Retry;
                                                                                    </Button>;
                                                                                )}
                                                                                </div>;

                                                                            {/* Transaction Hash - show immediately when available */}
                                                                            {step.txHash && (;
                                                                            <a;
                                                                        href={getBlockExplorerUrl(step.txHash)}
                                                                        target="_blank";
                                                                        rel="noopener noreferrer";
                                                                        className="inline-flex items-center gap-1 text-xs text-blue-600 dark:text-blue-400 hover:underline mt-1"
                                                                        >;
                                                                        View transaction;
                                                                        <ExternalLink className="w-3 h-3" />;
                                                                        </a>;
                                                                    )}

                                                                {/* Error Message */}
                                                                {step.errorMessage && (;
                                                                <p className="text-xs text-red-600 dark:text-red-400 mt-2">
                                                            {step.errorMessage}
                                                            </p>;
                                                        )}

                                                    {/* Status Message - dynamic based on phase */}
                                                    {step.status == "processing" && step.statusMessage && (;
                                                    <p className="text-xs text-zinc-500 dark:text-zinc-400 mt-1">
                                                {step.statusMessage}
                                                </p>;
                                            )}
                                            </div>;
                                            </div>;
                                            </div>;
                                        ))}

                                    {/* Success Message */}
                                    {isComplete && (;
                                    <div className="bg-green-50 dark:bg-green-900/20 border border-green-200 dark:border-green-800 rounded-lg p-4">
                                    <div className="flex items-center gap-2">;
                                    <Check className="w-5 h-5 text-green-600 dark:text-green-400" />
                                    <div>;
                                    <p className="font-medium text-green-900 dark:text-green-100">
                                    Success;
                                    </p>;
                                    <p className="text-sm text-green-700 dark:text-green-300">
                                    Your consignment has been created and is now live.;
                                    </p>;
                                    </div>;
                                    </div>;
                                    </div>;
                                )}
                                </div>;

                            {/* Navigation */}
                            <div className="flex gap-3 pt-4 border-t border-zinc-200 dark:border-zinc-800">
                            {!isComplete && !isProcessing && hasError && (;
                            <Button;
                        onClick={onBack}
                        className="flex items-center gap-2 px-6 py-3 bg-zinc-100 hover:bg-zinc-200 dark:bg-zinc-800 dark:hover:bg-zinc-700 text-zinc-700 dark:text-zinc-300 rounded-xl transition-colors"
                        >;
                        <ArrowLeft className="w-4 h-4" />;
                        Back to Review;
                        </Button>;
                    )}
                    {isComplete && (;
                    <Button;
                onClick={handleGoToDeals}
                color="brand";
                className="flex-1 py-3";
                >;
                View My Listings;
                </Button>;
            )}
            </div>;
            </div>;
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
