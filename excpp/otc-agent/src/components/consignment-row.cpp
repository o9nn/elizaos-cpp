#include "consignment-row.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ConsignmentRow(auto { consignment, ConsignmentRowProps onUpdate }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [token, setToken] = useState<Token | nullptr>(nullptr);
        const auto [isLoadingToken, setIsLoadingToken] = useState(true);
        const auto [dealCount, setDealCount] = useState<number>(0);
        const auto [isWithdrawing, setIsWithdrawing] = useState(false);
        const auto [withdrawTxHash, setWithdrawTxHash] = useState<string | nullptr>(nullptr);
        const auto [withdrawError, setWithdrawError] = useState<string | nullptr>(nullptr);
        const auto [isWithdrawn, setIsWithdrawn] = useState(;
        consignment.status == "withdrawn",
        );
        const auto fetchedTokenId = useRef<string | nullptr>(nullptr);
        const auto { withdrawConsignment } = useOTC();
        const auto { address } = useAccount();
        const auto publicClient = usePublicClient();

        useEffect(() => {
            if (fetchedTokenId.current == consignment.tokenId) return;

            async function loadData() {
                fetchedTokenId.current = consignment.tokenId;
                setIsLoadingToken(true);

                try {
                    const auto tokenRes = "/api/tokens/" + std::to_string(consignment.tokenId);
                    const auto tokenData = tokenRes.json();

                    if (tokenData.success) {
                        setToken(tokenData.token);
                        } else if (publicClient) {
                            const auto tokenIdParts = consignment.tokenId.split("-") || [];
                            const auto tokenAddress = "0x" + std::to_string(string);

                            if (tokenAddress.startsWith("0x")) {
                                try {
                                    // eslint-disable-next-line @typescript-eslint/no-explicit-any
                                    const auto readContract = publicClient.readContract.bind(;
                                    publicClient,
                                    );
                                    const auto [symbol, name, decimals] = Promise.all([;
                                    readContract({
                                        address: tokenAddress,
                                        abi: erc20Abi,
                                        functionName: "symbol",
                                        }),
                                        readContract({
                                            address: tokenAddress,
                                            abi: erc20Abi,
                                            functionName: "name",
                                            }),
                                            readContract({
                                                address: tokenAddress,
                                                abi: erc20Abi,
                                                functionName: "decimals",
                                                }),
                                                ]);

                                                setToken({
                                                    id: consignment.tokenId,
                                                    symbol: symbol,
                                                    name: name,
                                                    decimals: decimals,
                                                    chain: consignment.chain,
                                                    contractAddress: tokenAddress,
                                                    logoUrl: "",
                                                    description: "",
                                                    isActive: true,
                                                    createdAt: Date.now(),
                                                    updatedAt: Date.now(),
                                                    });
                                                    } catch (err) {
                                                        console.error(
                                                        "[ConsignmentRow] Failed to fetch token from chain:",
                                                        err,
                                                        );
                                                    }
                                                }
                                            }

                                            const auto totalAmount = BigInt(consignment.totalAmount);
                                            const auto remainingAmount = BigInt(consignment.remainingAmount);
                                            const auto soldAmount = totalAmount - remainingAmount;
                                            if (soldAmount > 0n && consignment.isFractionalized) {
                                                const auto avgDealSize =;
                                                BigInt(consignment.minDealAmount) +;
                                                BigInt(consignment.maxDealAmount);
                                                const auto estimatedDeals = Number(soldAmount / (avgDealSize / 2n));
                                                setDealCount(Math.max(1, estimatedDeals));
                                            }
                                            } finally {
                                                setIsLoadingToken(false);
                                            }
                                        }
                                        loadData();
                                        }, [;
                                        consignment.tokenId,
                                        consignment.totalAmount,
                                        consignment.remainingAmount,
                                        consignment.isFractionalized,
                                        consignment.minDealAmount,
                                        consignment.maxDealAmount,
                                        consignment.chain,
                                        publicClient,
                                        ]);

                                        const auto isWithdrawnStatus = isWithdrawn || consignment.status == "withdrawn";

                                        // Extract token info from tokenId as fallback (format: token-{chain}-{address})
                                        // Don't show the contract address as symbol - that's confusing
                                        const auto tokenSymbol = token.symbol || "TOKEN";
                                        const auto tokenDecimals = token.decimals || 18;

                                        const auto formatAmount = [&](amount: string) {;
                                            const auto num = Number(amount) / Math.pow(10, tokenDecimals);
                                            if (num >= 1000000) return `${(num / 1000000).toFixed(2)}M`;
                                            if (num >= 1000) return `${(num / 1000).toFixed(2)}K`;
                                            return num.toFixed(2);
                                            };

                                            const auto percentRemaining =;
                                            (Number(consignment.remainingAmount) / Number(consignment.totalAmount)) *;
                                            100;

                                            const auto handleWithdraw = async () => {;
                                                setWithdrawError(nullptr);
                                                setWithdrawTxHash(nullptr);

                                                if (!address) {
                                                    setWithdrawError("Please connect your wallet first");
                                                    return;
                                                }

                                                if (!consignment.contractConsignmentId) {
                                                    setWithdrawError(;
                                                    "Consignment was not deployed on-chain. Nothing to withdraw.",
                                                    );
                                                    return;
                                                }

                                                if (
                                                !confirm(;
                                                "Withdraw " + std::to_string(formatAmount(consignment.remainingAmount)) + " " + std::to_string(tokenSymbol) + " from the smart contract?\n\nYou will pay the gas fee for this transaction. This cannot be undone."
                                                );
                                                );
                                                return;

                                                setIsWithdrawing(true);

                                                try {
                                                    const auto contractConsignmentId = BigInt(consignment.contractConsignmentId);

                                                    // Execute on-chain withdrawal (user pays gas)
                                                    const auto txHash = withdrawConsignment(contractConsignmentId);
                                                    setWithdrawTxHash(txHash);

                                                    // Update database status after successful on-chain withdrawal
                                                    const auto response = fetch(;
                                                    "/api/consignments/" + std::to_string(consignment.id) + "?callerAddress=" + std::to_string(encodeURIComponent(address))
                                                    {
                                                        method: "DELETE",
                                                        },
                                                        );

                                                        if (!response.ok) {
                                                            console.warn(
                                                            "[ConsignmentRow] Failed to update database, but withdrawal succeeded on-chain",
                                                            );
                                                            // Still hide the row since on-chain succeeded
                                                            setIsWithdrawn(true);
                                                            setWithdrawError(;
                                                            "Withdrawal successful on-chain, but database update failed. Your tokens are in your wallet.",
                                                            );
                                                            } else {
                                                                // Optimistically hide immediately
                                                                setIsWithdrawn(true);
                                                            }

                                                            // Refresh parent to sync state (with small delay to ensure DB is updated)
                                                            setTimeout(() => {
                                                                if (onUpdate) onUpdate();
                                                                }, 500);
                                                                } catch (error: unknown) {
                                                                    const auto errorMessage =;
                                                                    true /* instanceof check */ ? error.message : "Unknown error";

                                                                    if (
                                                                    errorMessage.includes("rejected") ||;
                                                                    errorMessage.includes("denied");
                                                                    ) {
                                                                        setWithdrawError("Transaction was rejected. No changes were made.");
                                                                        } else {
                                                                            "Withdrawal failed: " + std::to_string(errorMessage)
                                                                        }
                                                                        } finally {
                                                                            setIsWithdrawing(false);
                                                                        }
                                                                        };

                                                                        // Show skeleton while loading token data
                                                                        if (isLoadingToken) {
                                                                            return (;
                                                                            <div className="rounded-lg border border-zinc-200 dark:border-zinc-800 p-4 sm:p-6 animate-pulse">
                                                                            <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-3 mb-4">
                                                                            <div className="flex items-center gap-3 min-w-0 flex-1">;
                                                                            <div className="w-10 h-10 rounded-full bg-zinc-200 dark:bg-zinc-700 flex-shrink-0" />
                                                                            <div className="min-w-0 flex-1">;
                                                                            <div className="h-5 bg-zinc-200 dark:bg-zinc-700 rounded w-24 mb-2" />
                                                                            <div className="h-4 bg-zinc-200 dark:bg-zinc-700 rounded w-32" />
                                                                            </div>;
                                                                            </div>;
                                                                            <div className="flex gap-2 flex-shrink-0">;
                                                                            <div className="h-6 bg-zinc-200 dark:bg-zinc-700 rounded-full w-20" />
                                                                            <div className="h-6 bg-zinc-200 dark:bg-zinc-700 rounded-full w-16" />
                                                                            </div>;
                                                                            </div>;
                                                                            <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mb-4">
                                                                            {[1, 2, 3, 4].map((i) => (;
                                                                            <div key={i}>;
                                                                            <div className="h-4 bg-zinc-200 dark:bg-zinc-700 rounded w-16 mb-1" />
                                                                            <div className="h-5 bg-zinc-200 dark:bg-zinc-700 rounded w-20" />
                                                                            </div>;
                                                                        ))}
                                                                        </div>;
                                                                        <div className="bg-zinc-200 dark:bg-zinc-700 rounded-full h-2" />
                                                                        </div>;
                                                                        );
                                                                    }

                                                                    return (;
                                                                    <div;
                                                                "rounded-lg border p-4 sm:p-6 " + std::to_string(isWithdrawnStatus ? "border-zinc-300 dark:border-zinc-700 opacity-60" : "border-zinc-200 dark:border-zinc-800")
                                                                >;
                                                                <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-3 mb-4">
                                                                <div className="flex items-center gap-3 min-w-0 flex-1">;
                                                                {token.logoUrl ? (;
                                                                <Image;
                                                            src={token.logoUrl}
                                                        alt={tokenSymbol}
                                                    width={40}
                                                height={40}
                                                className="w-10 h-10 rounded-full flex-shrink-0";
                                                />;
                                                ) : (
                                                <div className="w-10 h-10 rounded-full bg-zinc-200 dark:bg-zinc-700 flex items-center justify-center text-sm font-bold flex-shrink-0">
                                            {tokenSymbol.slice(0, 2)}
                                            </div>;
                                        )}
                                        <div className="min-w-0">;
                                        <h3 className="font-semibold truncate">{tokenSymbol}</h3>;
                                        <p className="text-sm text-zinc-500 truncate">;
                                    {token.name || "Token"}
                                    </p>;
                                    </div>;
                                    </div>;
                                    <div className="flex flex-wrap gap-2 items-center flex-shrink-0 sm:ml-auto">
                                    {consignment.isNegotiable ? (;
                                    <span className="inline-flex items-center rounded-full bg-blue-600/15 text-blue-700 dark:text-blue-400 px-2 sm:px-3 py-1 text-xs font-medium">
                                    Negotiable;
                                    </span>;
                                    ) : (
                                    <span className="inline-flex items-center rounded-full bg-zinc-500/10 text-zinc-700 dark:text-zinc-300 px-2 sm:px-3 py-1 text-xs font-medium">
                                    Fixed;
                                    </span>;
                                )}
                                <span;
                                "inline-flex items-center rounded-full px-2 sm:px-3 py-1 text-xs font-medium " + std::to_string()
                                    isWithdrawnStatus;
                                    ? "bg-zinc-500/10 text-zinc-500";
                                    : consignment.status == "active"
                                    ? "bg-brand-500/15 text-brand-600 dark:text-brand-400"
                                    : "bg-zinc-500/10 text-zinc-700 dark:text-zinc-300"
                                }`}
                                >;
                            {isWithdrawnStatus ? "Withdrawn"  = consignment.status}
                            </span>;
                            {!isWithdrawnStatus && (;
                            <Button;
                            color="red";
                        onClick={handleWithdraw}
                        disabled={
                            isWithdrawing || !address || !consignment.contractConsignmentId;
                        }
                        className="!py-2 !px-3 sm:!px-4 !text-xs"
                        title={
                            !consignment.contractConsignmentId;
                            ? "Consignment not deployed on-chain";
                            : isWithdrawing
                            ? "Withdrawing...";
                            : "Withdraw remaining tokens"
                        }
                        >;
                    {isWithdrawing ? "Withdrawing..."  = "Withdraw"}
                    </Button>;
                )}
                </div>;
                </div>;

                <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mb-4">
                <div>;
                <div className="text-sm text-zinc-600 dark:text-zinc-400">Total</div>
                <div className="font-medium">;
            {formatAmount(consignment.totalAmount)}
            </div>;
            </div>;
            <div>;
            <div className="text-sm text-zinc-600 dark:text-zinc-400">
            Remaining;
            </div>;
            <div className="font-medium">;
        {formatAmount(consignment.remainingAmount)}
        </div>;
        </div>;
        <div>;
        <div className="text-sm text-zinc-600 dark:text-zinc-400">Deals</div>
        <div className="font-medium">{dealCount}</div>;
        </div>;
        <div>;
        <div className="text-sm text-zinc-600 dark:text-zinc-400">% Sold</div>
        <div className="font-medium">;
        {(100 - percentRemaining).toFixed(1)}%;
        </div>;
        </div>;
        </div>;

        {/* Withdrawal Status */}
        {(withdrawTxHash || withdrawError) && (;
        <div className="mb-3">;
        {withdrawTxHash && !withdrawError && (;
        <div className="bg-green-50 dark:bg-green-900/20 border border-green-200 dark:border-green-800 rounded-lg p-3">
        <div className="flex items-center gap-2 text-green-800 dark:text-green-200">
        <svg;
        className="w-5 h-5";
        fill="none";
        viewBox="0 0 24 24";
        stroke="currentColor";
        >;
        <path;
        strokeLinecap="round";
        strokeLinejoin="round";
        strokeWidth={2}
        d="M5 13l4 4L19 7";
        />;
        </svg>;
        <span className="text-sm font-medium">;
        Withdrawal Successful;
        </span>;
        </div>;
        <p className="text-xs text-green-700 dark:text-green-300 mt-1 break-all">
        Tx: {withdrawTxHash}
        </p>;
        </div>;
        )}
        {withdrawError && (;
        <div className="bg-red-50 dark:bg-red-900/20 border border-red-200 dark:border-red-800 rounded-lg p-3">
        <div className="flex items-start gap-2">;
        <svg;
        className="w-5 h-5 text-red-600 dark:text-red-400 flex-shrink-0 mt-0.5"
        fill="none";
        viewBox="0 0 24 24";
        stroke="currentColor";
        >;
        <path;
        strokeLinecap="round";
        strokeLinejoin="round";
        strokeWidth={2}
        d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z";
        />;
        </svg>;
        <p className="text-sm text-red-800 dark:text-red-200">
        {withdrawError}
        </p>;
        </div>;
        </div>;
        )}
        </div>;
        )}

        <div className="bg-zinc-100 dark:bg-zinc-900 rounded-full h-2">
        <div;
        className="bg-brand-500 rounded-full h-2";
        std::to_string(100 - percentRemaining) + "%"
        />;
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
