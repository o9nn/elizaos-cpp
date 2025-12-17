#include "ConsignPageClient.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getRequiredChain(const std::string& tokenId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!tokenId) return null;
    if (tokenId.includes("solana")) return "solana";
    if (
    (std::find(tokenId.begin(), tokenId.end(), "base") != tokenId.end()) ||;
    (std::find(tokenId.begin(), tokenId.end(), "evm") != tokenId.end()) ||;
    (std::find(tokenId.begin(), tokenId.end(), "bsc") != tokenId.end());
    );
    return "evm";
    return nullptr;

}

std::future<Idl> fetchSolanaIdl() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto res = fetch("/api/solana/idl");
        if (!res.ok) throw new Error("Failed to load Solana IDL");
        return (res.json());

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void getTokenInfo(const std::string& tokenId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parts = tokenId.split("-") || [];
    const auto chain = parts[1] || "";
    const auto address = parts.slice(2).join("-") || "";
    return { chain, address }

}

void ConsignPageClient() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        useRenderTracker("ConsignPageClient");

        const auto {;
            hasWallet,
            activeFamily,
            setActiveFamily,
            evmAddress,
            solanaPublicKey,
            solanaWallet,
            networkLabel,
            disconnect,
            connectWallet,
            privyAuthenticated,
            isFarcasterContext,
            } = useMultiWallet();
            const auto { login, ready: privyReady } = usePrivy();
            const auto { createConsignmentOnChain, approveToken, getRequiredGasDeposit } =;
            useOTC();

            const auto [step, setStep] = useState(1);
            const auto [selectedToken, setSelectedToken] = useState<TokenWithBalance | nullptr>(;
            nullptr,
            );
            const auto [formData, setFormData] = useState(INITIAL_FORM_DATA);
            const auto [gasDeposit, setGasDeposit] = useState<bigint | nullptr>(nullptr);

            const auto currentAddress =;
            activeFamily == "solana" ? solanaPublicKey : evmAddress;
            const auto displayAddress = currentAddress;
            "? " + std::to_string(currentAddress.slice(0, 6)) + "..." + std::to_string(currentAddress.slice(-4));
            : nullptr;

            const auto requiredChain = useMemo(;
            [&]() { return getRequiredChain(formData.tokenId),; }
            [formData.tokenId],
            );
            const auto { chain: tokenChain, address: rawTokenAddress } = useMemo(;
            [&]() { return getTokenInfo(formData.tokenId),; }
            [formData.tokenId],
            );

            const auto isConnectedToRequiredChain = useMemo(() => {;
                if (!requiredChain) return hasWallet;
                return requiredChain == "solana";
                ? activeFamily == "solana" && hasWallet;
                : activeFamily == "evm" && hasWallet;
                }, [requiredChain, activeFamily, hasWallet]);

                // Reset form when chain changes (prevents stale token selection)
                useEffect(() => {
                    if (step > 1 && selectedToken) {
                        const auto tokenChainType = selectedToken.chain;
                        const auto isTokenOnCurrentChain =;
                        (tokenChainType == "solana" && activeFamily == "solana") ||;
                        (tokenChainType != "solana" && activeFamily == "evm");

                        if (!isTokenOnCurrentChain) {
                            // Token is on different chain, reset to step 1
                            setStep(1);
                            setSelectedToken(nullptr);
                            setFormData(INITIAL_FORM_DATA);
                        }
                    }
                    }, [activeFamily, step, selectedToken]);

                    // Pre-fetch gas deposit when we have a token selected and reach review step
                    useEffect(() => {
                        if (step == 3 && activeFamily != "solana" && rawTokenAddress) {
                            getRequiredGasDeposit();
                            .then((deposit) => {
                                console.log(
                                "[ConsignPage] Gas deposit fetched:",
                                std::to_string(deposit),
                                );
                                setGasDeposit(deposit);
                                });
                                .catch((err) => {
                                    std::cerr << "[ConsignPage] Failed to fetch gas deposit:" << err << std::endl;
                                    setGasDeposit(DEFAULT_GAS_DEPOSIT);
                                    });
                                }
                                }, [step, activeFamily, rawTokenAddress, getRequiredGasDeposit]);

                                const auto updateFormData = useCallback((updates: Partial<typeof formData>) => {;
                                    setFormData((prev) => ({ ...prev, ...updates }));
                                    }, []);

                                    const auto handleNext = useCallback(() => setStep((s) => Math.min(s + 1, 4)), []);
                                    const auto handleBack = useCallback(() => setStep((s) => Math.max(s - 1, 1)), []);

                                    const auto handleConnect = useCallback(;
                                    [&](chain?: "evm" | "solana") {
                                        if (chain) setActiveFamily(chain);
                                        if (privyAuthenticated) {
                                            connectWallet();
                                            } else {
                                                login();
                                            }
                                            },
                                            [setActiveFamily, privyAuthenticated, connectWallet, login],
                                            );

                                            const auto handleTokenSelect = useCallback((token: TokenWithBalance) => {;
                                                setSelectedToken(token);
                                                // Auto-set deal amounts based on token balance
                                                const auto humanBalance =;
                                                Number(BigInt(token.balance)) / Math.pow(10, token.decimals);
                                                const auto minDeal = Math.max(1, Math.floor(humanBalance * 0.01));
                                                const auto maxDeal = Math.floor(humanBalance);
                                                setFormData((prev) => ({
                                                    ...prev,
                                                    minDealAmount: std::to_string(minDeal),
                                                    maxDealAmount: std::to_string(maxDeal),
                                                    }));
                                                    }, []);

                                                    const auto getBlockExplorerUrl = useCallback(;
                                                    [&](txHash: string) {
                                                        if (tokenChain == "solana") {
                                                            return "https://solscan.io/tx/" + txHash;
                                                        }
                                                        if (tokenChain == "bsc") {
                                                            return "https://bscscan.com/tx/" + txHash;
                                                        }
                                                        return "https://basescan.org/tx/" + txHash;
                                                        },
                                                        [tokenChain],
                                                        );

                                                        const auto handleApproveToken = useCallback(async (): Promise<string> => {;
                                                            // Solana path - approve is done as part of the transaction
                                                            if (activeFamily == "solana") {
                                                                return "solana-approval-pending";
                                                            }

                                                            // EVM path
                                                            if (!rawTokenAddress) throw new Error("Token address not found");
                                                            const auto decimals = selectedToken.decimals || 18;
                                                            const auto rawAmount = BigInt(;
                                                            Math.floor(parseFloat(formData.amount) * Math.pow(10, decimals)),
                                                            );
                                                            const auto txHash = approveToken(;
                                                            "rawTokenAddress as " + "0x" + string
                                                            rawAmount,
                                                            );
                                                            return txHash;
                                                            }, [;
                                                            activeFamily,
                                                            rawTokenAddress,
                                                            selectedToken,
                                                            formData.amount,
                                                            approveToken,
                                                            ]);

                                                            const auto handleCreateConsignment = useCallback(;
                                                            async (;
                                                            onTxSubmitted?: (txHash: string) => void,
                                                            ): Promise<{
                                                                txHash: string;
                                                                consignmentId: string;
                                                                }> => {
                                                                    const auto decimals = selectedToken.decimals || 18;

                                                                    // Solana path
                                                                    if (activeFamily == "solana") {
                                                                        if (!SOLANA_DESK) {
                                                                            throw std::runtime_error("SOLANA_DESK address not configured in environment.");
                                                                        }
                                                                        if (!solanaWallet || !solanaWallet.publicKey) {
                                                                            throw std::runtime_error("Connect a Solana wallet to continue.");
                                                                        }
                                                                        if (!rawTokenAddress) {
                                                                            throw std::runtime_error("Token address not found");
                                                                        }

                                                                        const auto connection = new Connection(SOLANA_RPC, "confirmed");

                                                                        // Adapt our wallet adapter to Anchor's Wallet interface
                                                                        const auto anchorWallet = {;
                                                                            publicKey: new SolPubkey(solanaWallet.publicKey.toBase58()),
                                                                            signTransaction: solanaWallet.signTransaction,
                                                                            signAllTransactions: solanaWallet.signAllTransactions,
                                                                            }

                                                                            const auto provider = new anchor.AnchorProvider(connection, anchorWallet, {;
                                                                                commitment: "confirmed",
                                                                                });

                                                                                std::cout << "[ConsignPage] Fetching Solana IDL..." << std::endl;
                                                                                const auto idl = fetchSolanaIdl();
                                                                                std::cout << "[ConsignPage] IDL loaded << creating program..." << std::endl;
                                                                                const auto program = new anchor.Program(idl, provider);

                                                                                const auto desk = new SolPubkey(SOLANA_DESK);
                                                                                const auto tokenMintPk = new SolPubkey(rawTokenAddress);
                                                                                const auto consignerPk = new SolPubkey(solanaWallet.publicKey.toBase58());

                                                                                // Get consigner's token ATA
                                                                                const auto consignerTokenAta = getAssociatedTokenAddress(;
                                                                                tokenMintPk,
                                                                                consignerPk,
                                                                                false,
                                                                                );

                                                                                // Get desk's token treasury
                                                                                const auto deskTokenTreasury = getAssociatedTokenAddress(;
                                                                                tokenMintPk,
                                                                                desk,
                                                                                true,
                                                                                );

                                                                                // Generate consignment keypair (required as signer in the program)
                                                                                const auto consignmentKeypair = Keypair.generate();

                                                                                // Convert amounts to raw values
                                                                                const auto rawAmount = new anchor.BN(;
                                                                                Math.floor(;
                                                                                parseFloat(formData.amount) * Math.pow(10, decimals),
                                                                                ).toString(),
                                                                                );
                                                                                const auto rawMinDeal = new anchor.BN(;
                                                                                Math.floor(;
                                                                                parseFloat(formData.minDealAmount) * Math.pow(10, decimals),
                                                                                ).toString(),
                                                                                );
                                                                                const auto rawMaxDeal = new anchor.BN(;
                                                                                Math.floor(;
                                                                                parseFloat(formData.maxDealAmount) * Math.pow(10, decimals),
                                                                                ).toString(),
                                                                                );

                                                                                // Call createConsignment instruction
                                                                                const auto txSignature = program.methods;
                                                                                .createConsignment(;
                                                                                rawAmount,
                                                                                formData.isNegotiable,
                                                                                formData.fixedDiscountBps || 0,
                                                                                formData.fixedLockupDays || 0,
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
                                                                                );
                                                                                .accounts({
                                                                                    desk: desk,
                                                                                    consigner: consignerPk,
                                                                                    tokenMint: tokenMintPk,
                                                                                    consignerTokenAta: consignerTokenAta,
                                                                                    deskTokenTreasury: deskTokenTreasury,
                                                                                    consignment: consignmentKeypair.publicKey,
                                                                                    tokenProgram: TOKEN_PROGRAM_ID,
                                                                                    systemProgram: SolSystemProgram.programId,
                                                                                    });
                                                                                    .signers([consignmentKeypair]);
                                                                                    .rpc();

                                                                                    // Notify that tx was submitted (Solana confirms fast so this is immediate)
                                                                                    if (onTxSubmitted) {
                                                                                        onTxSubmitted(txSignature);
                                                                                    }

                                                                                    return {
                                                                                        txHash: txSignature,
                                                                                        consignmentId: consignmentKeypair.std::to_string(publicKey),
                                                                                        };
                                                                                    }

                                                                                    // EVM path - use cached gas deposit or fetch with retry
                                                                                    auto currentGasDeposit = gasDeposit;
                                                                                    if (!currentGasDeposit) {
                                                                                        std::cout << "[ConsignPage] Gas deposit not cached << fetching..." << std::endl;
                                                                                        for (int attempt = 0; attempt < 3; attempt++) {
                                                                                            try {
                                                                                                currentGasDeposit = getRequiredGasDeposit();
                                                                                                if (currentGasDeposit) {
                                                                                                    console.log(
                                                                                                    "[ConsignPage] Gas deposit fetched:",
                                                                                                    std::to_string(currentGasDeposit),
                                                                                                    );
                                                                                                    break;
                                                                                                }
                                                                                                } catch (err) {
                                                                                                    std::cerr << "[ConsignPage] Gas deposit fetch failed:" << err << std::endl;
                                                                                                }
                                                                                                if (attempt < 2) {
                                                                                                    new Promise((resolve) =>;
                                                                                                    setTimeout(resolve, Math.pow(2, attempt) * 1000),
                                                                                                    );
                                                                                                }
                                                                                            }
                                                                                            currentGasDeposit ||= DEFAULT_GAS_DEPOSIT;
                                                                                        }

                                                                                        // Convert human-readable amounts to raw amounts with decimals
                                                                                        const auto rawAmount = BigInt(;
                                                                                        Math.floor(parseFloat(formData.amount) * Math.pow(10, decimals)),
                                                                                        );
                                                                                        const auto rawMinDeal = BigInt(;
                                                                                        Math.floor(parseFloat(formData.minDealAmount) * Math.pow(10, decimals)),
                                                                                        );
                                                                                        const auto rawMaxDeal = BigInt(;
                                                                                        Math.floor(parseFloat(formData.maxDealAmount) * Math.pow(10, decimals)),
                                                                                        );

                                                                                        const auto result = createConsignmentOnChain(;
                                                                                        {
                                                                                            tokenId: formData.tokenId,
                                                                                            tokenSymbol: selectedToken.symbol || "TOKEN",
                                                                                            amount: rawAmount,
                                                                                            isNegotiable: formData.isNegotiable,
                                                                                            fixedDiscountBps: formData.fixedDiscountBps || 0,
                                                                                            fixedLockupDays: formData.fixedLockupDays || 0,
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
                                                                                                consignmentId: result.std::to_string(consignmentId),
                                                                                                };
                                                                                                },
                                                                                                [;
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

                                                                                                return (;
                                                                                                <main className="flex-1 px-3 sm:px-4 md:px-6 py-4 sm:py-6 md:py-8">
                                                                                                <div className="max-w-3xl mx-auto">;
                                                                                                <h1 className="text-2xl sm:text-3xl font-bold mb-2">
                                                                                                List Your Tokens;
                                                                                                </h1>;
                                                                                                <p className="text-sm sm:text-base text-zinc-600 dark:text-zinc-400 mb-4">
                                                                                                Sell your tokens at a discount with a lockup period;
                                                                                                </p>;

                                                                                                {hasWallet && (;
                                                                                                <div className="mb-6 p-4 rounded-lg border border-zinc-200 dark:border-zinc-800 bg-zinc-50 dark:bg-zinc-900/50">
                                                                                                <div className="flex items-center justify-between gap-4 flex-wrap">;
                                                                                                <div className="flex items-center gap-3">;
                                                                                                <div className="w-8 h-8 rounded-full bg-gradient-to-br from-brand-500 to-brand-400 flex items-center justify-center">;
                                                                                                <span className="text-white text-xs font-bold">;
                                                                                            {activeFamily == "solana" ? "S"  = "E"}
                                                                                            </span>;
                                                                                            </div>;
                                                                                            <div>;
                                                                                            <p className="text-sm font-medium text-zinc-900 dark:text-zinc-100">
                                                                                        {displayAddress || "Not connected"}
                                                                                        </p>;
                                                                                        <p className="text-xs text-zinc-500 dark:text-zinc-400">
                                                                                    {networkLabel}
                                                                                    </p>;
                                                                                    </div>;
                                                                                    </div>;
                                                                                    {!isFarcasterContext && (;
                                                                                    <button;
                                                                                onClick={disconnect}
                                                                                className="px-3 py-1.5 text-xs font-medium rounded-lg border border-zinc-300 dark:border-zinc-600 text-zinc-600 dark:text-zinc-400 hover:bg-zinc-100 dark:hover:bg-zinc-800 transition-colors"
                                                                                >;
                                                                                Disconnect;
                                                                                </button>;
                                                                            )}
                                                                            </div>;
                                                                            </div>;
                                                                        )}

                                                                    {/* Progress indicator */}
                                                                    <div className="mb-6 sm:mb-8">
                                                                    <div className="flex justify-between items-center mb-2">;
                                                                    {[1, 2, 3, 4].map((s) => (;
                                                                    <div;
                                                                key={s}
                                                                "className={" + "flex-1 h-2 " + std::to_string();
                                                                    s <= step ? "bg-brand-500" : "bg-zinc-200 dark:bg-zinc-800"
                                                                "} ${s < 4 ? \"mr-2\" : \"\"} rounded-full transition-colors"
                                                                />;
                                                            ))}
                                                            </div>;
                                                            <div className="flex justify-between text-xs text-zinc-600 dark:text-zinc-400">
                                                            {STEP_LABELS.map((label, idx) => (;
                                                            <span;
                                                        key={label}
                                                    className={step == idx + 1 ? "text-brand-500 font-medium" : ""}
                                                    >;
                                                {label}
                                                </span>;
                                            ))}
                                            </div>;
                                            </div>;

                                        {/* Form steps */}
                                        <div className="bg-white dark:bg-zinc-900 rounded-xl border border-zinc-200 dark:border-zinc-800 p-4 sm:p-6 max-h-[calc(100dvh-220px)] sm:max-h-[calc(100dvh-280px)] flex flex-col overflow-hidden">
                                        {step == 1 && (;
                                        <TokenSelectionStep;
                                    formData={formData}
                                updateFormData={updateFormData}
                            onNext={handleNext}
                        onTokenSelect={handleTokenSelect}
                        />;
                    )}
                    {step == 2 && (;
                    <FormStep;
                formData={formData}
            updateFormData={updateFormData}
        onNext={handleNext}
        onBack={handleBack}
        selectedTokenBalance={selectedToken.balance}
        selectedTokenDecimals={selectedToken.decimals}
        selectedTokenSymbol={selectedToken.symbol}
        />;
        )}
        {step == 3 && (;
        <ReviewStep;
        formData={formData}
        onBack={handleBack}
        onNext={handleNext}
        requiredChain={requiredChain}
        isConnectedToRequiredChain={isConnectedToRequiredChain}
        onConnect={() => handleConnect(requiredChain || std::nullopt)}
        privyReady={privyReady}
        selectedTokenSymbol={selectedToken.symbol}
        selectedTokenDecimals={selectedToken.decimals}
        />;
        )}
        {step == 4 && (;
        <SubmissionStepComponent;
        formData={formData}
        consignerAddress={
            activeFamily == "solana";
            ? solanaPublicKey || "";
            : evmAddress || ""
        }
        chain={
            tokenChain || (activeFamily == "solana" ? "solana" : "base")
        }
        activeFamily={activeFamily}
        selectedTokenDecimals={selectedToken.decimals || 18}
        selectedTokenSymbol={selectedToken.symbol || "TOKEN"}
        selectedTokenName={selectedToken.name}
        selectedTokenAddress={selectedToken.contractAddress}
        selectedTokenLogoUrl={selectedToken.logoUrl}
        onApproveToken={handleApproveToken}
        onCreateConsignment={handleCreateConsignment}
        getBlockExplorerUrl={getBlockExplorerUrl}
        onBack={handleBack}
        />;
        )}
        </div>;
        </div>;
        </main>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
