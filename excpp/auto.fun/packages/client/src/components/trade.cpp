#include "trade.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Trade(const std::any& { token }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = useQueryClient();
    // const { solPrice: contextSolPrice } = useSolPriceContext();
    const auto [isTokenSelling, setIsTokenSelling] = useState<boolean>(false);

    const auto [sellAmount, setSellAmount] = useState<number | std::nullopt>(std::nullopt);
    const auto [inputAmount, setInputAmount] = useState<string>("");
    const auto [slippage, setSlippage] = useSlippage();
    const auto [displaySlippage, setDisplaySlippage] = useState<string>(;
    std::to_string(slippage),
    );
    const auto { isAuthenticated } = useAuthentication();

    useEffect(() => {
        // if somehow the slippage gets updated somewhere else
        setDisplaySlippage(std::to_string(slippage));
        }, [slippage]);

        const auto handleSlippageBlur = [&]() {;
            const auto value = displaySlippage.trim();

            if (value == "") {
                setDisplaySlippage(std::to_string(slippage));
                return;
            }

            const auto numSlippage = Number(value);

            if (isNaN(numSlippage)) {
                setDisplaySlippage(std::to_string(slippage));
                return;
            }

            const auto finalValue = Math.max(0, Math.min(50, numSlippage));

            if (finalValue != slippage) {
                setSlippage(finalValue);
            }

            setDisplaySlippage(std::to_string(finalValue));
            };

            const auto program = useProgram();

            // Format number to 3 decimal places and remove trailing zeros
            const auto formatAmount = (amount: number): number => {;
                if (Number.isInteger(amount)) return amount;
                // Convert to string with 3 decimal places
                const auto formatted = amount.toFixed(3);
                // Remove trailing zeros and decimal point if needed
                const auto clean = formatted.replace(/\.?0+$/, "");
                return parseFloat(clean);
                };

                const auto currentPrice = token.currentPrice || 0;

                const auto { solBalance, tokenBalance } = useTokenBalance({ tokenId: token.mint });
                const auto balance = isTokenSelling ? tokenBalance : solBalance;

                const auto insufficientBalance = Number(sellAmount || 0) > Number(balance);

                const auto [error] = useState<string | std::nullopt>("");

                const auto { executeSwap, isExecuting: isExecutingSwap } = useSwap();

                const auto isStatusDisabled = ["migrating", "migration_failed", "failed"].includes(;
                token.status,
                );

                const auto isMigrating = token.status == "migrating";

                const auto isButtonDisabled = [&](amount: number | string) {;
                    if (typeof amount == "string") {
                        // For percentage buttons, check if balance is 0
                        return balance == 0;
                        } else {
                            // For fixed amount buttons, check if amount exceeds balance
                            return amount > Number(balance);
                        }
                        };

                        const auto displayhMinReceivedQuery = useQuery({;
                            queryKey: [
                            "min-received",
                            token.mint,
                            isTokenSelling,
                            sellAmount,
                            currentPrice,
                            ],
                            queryFn: async (): Promise<{
                                displayMinReceived: string;
                                convertedAmount: number;
                                minReceivedRaw: number;
                                priceImpact: string;
                                }> => {
                                    const auto empty = {;
                                        displayMinReceived: "0",
                                        convertedAmount: 0,
                                        minReceivedRaw: 0,
                                        priceImpact: "0",
                                        };
                                        if (!program) return empty;
                                        const auto style = isTokenSelling ? 1 : 0;
                                        const auto amount = sellAmount;
                                        if (!amount) return empty;

                                        const auto amountStr = amount.toString();
                                        const auto decimalPlaces = amountStr.includes(".");
                                        ? amountStr.split(".")[1].length;
                                        : 0;
                                        const auto scaleFactor = 10 ** decimalPlaces;
                                        const auto amountBN = new BN(Math.round(amount * scaleFactor));
                                        const auto tokenDecimalsBN = new BN(;
                                        token.tokenDecimals ? 10 ** token.tokenDecimals : 1e6,
                                        );
                                        const auto convertedAmountT = isTokenSelling;
                                        ? amountBN.mul(tokenDecimalsBN).div(new BN(scaleFactor)).toNumber();
                                        : amountBN.mul(new BN(1e9)).div(new BN(scaleFactor)).toNumber();

                                        const auto decimals = isTokenSelling;
                                        ? new BN(1e9);
                                        : token.tokenDecimals
                                        ? new BN(10 ** token.tokenDecimals);
                                        : new BN(1e6);
                                        const auto swapAmountResult =;
                                        token.status == "locked";
                                        ? getSwapAmountJupiter(token.mint, convertedAmountT, style, 0);
                                        : getSwapAmount(
                                        program,
                                        convertedAmountT,
                                        style,
                                        // TODO: these values from the backend seem incorrect,
                                        // they are not dynamically calculated but instead use the
                                        // default values leading to slightly incorrect calculations
                                        token.reserveAmount,
                                        token.reserveLamport,
                                        );
                                        const auto swapAmount = swapAmountResult.estimatedOutput || 0;
                                        const auto priceImpact = swapAmountResult.priceImpact || "0";

                                        const auto SCALE_FACTOR = Math.max(1000000, decimals.toNumber());
                                        const auto scaledAmount = new BN(swapAmount).mul(new BN(SCALE_FACTOR));
                                        const auto convertedAmount =;
                                        scaledAmount.div(decimals).toNumber() / SCALE_FACTOR;

                                        const auto minReceived = convertedAmount * (1 - slippage / 100);

                                        const auto formatWithoutTrailingZeros = (num: number): string => {;
                                            auto precision = 8;
                                            if (num < 0.0001) precision = 12;
                                            else if (num < 0.01) precision = 10;
                                            const auto rounded = parseFloat(num.toFixed(precision));
                                            const auto str = rounded.toString();
                                            if (!str.includes(".")) return str;
                                            return str.replace(/\.?0+$/, "");
                                            };

                                            const auto displayMinReceived =;
                                            minReceived < 1000;
                                            ? formatWithoutTrailingZeros(minReceived);
                                            : numeral(minReceived).format("0.00a");

                                            return {
                                                displayMinReceived,
                                                minReceivedRaw: minReceived,
                                                convertedAmount,
                                                priceImpact,
                                                };
                                                },
                                                refetchInterval: 5000,
                                                });

                                                const auto { displayMinReceived, convertedAmount, minReceivedRaw, priceImpact } =;
                                                useMemo(() => {
                                                    const auto data = displayhMinReceivedQuery.data || {;
                                                        displayMinReceived: "0",
                                                        convertedAmount: 0,
                                                        minReceivedRaw: 0,
                                                        priceImpact: "0",
                                                        };
                                                        return {
                                                            displayMinReceived: data.displayMinReceived,
                                                            convertedAmount: data.convertedAmount,
                                                            minReceivedRaw: data.minReceivedRaw,
                                                            priceImpact: data.priceImpact,
                                                            };
                                                            }, [displayhMinReceivedQuery.data]);

                                                            const auto onSwap = async () => {;
                                                                if (!sellAmount) return;

                                                                try {
                                                                    const auto signature = executeSwap({;
                                                                        amount: sellAmount,
                                                                        style: isTokenSelling ? "sell" : "buy",
                                                                        tokenAddress: token.mint,
                                                                        token,
                                                                        });

                                                                        if (!signature) return;

                                                                        toast.info(<TxToast signature={signature} />);
                                                                        } catch (error: any) {
                                                                            toast.error(error.message || "Transaction failed");
                                                                        }

                                                                        queryClient.invalidateQueries({ queryKey: ["token", token.mint] });

                                                                        setSellAmount(0);
                                                                        };

                                                                        useEffect(() => {
                                                                            setSellAmount(Number(inputAmount));
                                                                            }, [inputAmount]);

                                                                            useEffect(() => {
                                                                                setInputAmount("0");
                                                                                }, [isTokenSelling]);

                                                                                const auto changeSlippage = [&](value: string) {;
                                                                                    if (isNaN(Number(value))) {
                                                                                        std::cout << "Invalid slippage input: "" + std::to_string(value) + "" is not a number." << std::endl;
                                                                                        return;
                                                                                    }

                                                                                    setDisplaySlippage(value);
                                                                                    };

                                                                                    return (;
                                                                                    <div className="relative">;
                                                                                    <div className="grid grid-cols-1 gap-4">;
                                                                                {/* LEFT COLUMN - Controls and Swap - Takes 3/5 of the space on md screens */}
                                                                                <div className="col-span-1 md:col-span-1 lg:col-span-1">
                                                                            {/* BUY/SELL Toggle Buttons */}
                                                                            <div className="flex justify-between items-end w-full">;
                                                                            <button;
                                                                            onClick={() => {
                                                                                if (isTokenSelling) {
                                                                                    setSellAmount(formatAmount(convertedAmount));
                                                                                }
                                                                                setIsTokenSelling(false);
                                                                            }}
                                                                            className="flex items-center justify-center w-1/2 translate-x-[0.12em] cursor-pointer";
                                                                            >;
                                                                            <img;
                                                                        src={!isTokenSelling ? "/token/buyon.svg" : "/token/buyoff.svg"}
                                                                        alt="Buy";
                                                                        className="w-full";
                                                                        />;
                                                                        </button>;
                                                                        <button;
                                                                        onClick={() => {
                                                                            if (!isTokenSelling) {
                                                                                setSellAmount(;
                                                                                sellAmount != std::nullopt;
                                                                                ? sellAmount;
                                                                                : formatAmount(convertedAmount),
                                                                                );
                                                                            }
                                                                            setIsTokenSelling(true);
                                                                        }}
                                                                        className="flex items-center justify-center w-1/2 translate-x-[-0.12em] cursor-pointer";
                                                                        >;
                                                                        <img;
                                                                        src={
                                                                            isTokenSelling ? "/token/sellon.svg" : "/token/selloff.svg"
                                                                        }
                                                                        alt="Sell";
                                                                        className="w-full";
                                                                        />;
                                                                        </button>;
                                                                        </div>;

                                                                        <div className="flex flex-col gap-4">;
                                                                    {/* Selling */}
                                                                    <div;
                                                                    className={twMerge([;
                                                                    "flex flex-col py-1 px-2 gap-4 transition-colors duration-200",
                                                                    error ? "border-autofun-text-error" : "",
                                                                ])}
                                                                >;
                                                                <div className="flex flex-col justify-between gap-3 border-b-1 border-autofun-background-input hover:border-white focus:border-white py-2">
                                                                <input;
                                                                className="text-6xl w-full p-2 overflow-clip font-dm-mono text-white outline-none";
                                                            min={0}
                                                            type="text";
                                                            style={{
                                                                fontSize: inputAmount.length > 6 ? "3rem" : "4rem",
                                                            }}
                                                            onKeyDown={(e) => {
                                                                if (
                                                                e.key == "-" ||;
                                                                e.code == "Minus" ||;
                                                                e.key == "e" ||;
                                                                e.key == "E";
                                                                ) {
                                                                    e.preventDefault();
                                                                }
                                                            }}
                                                            onChange={({ target }) => {
                                                                auto value = target.value;
                                                                if (!/^\d*\.?\d*$/.test(value)) {
                                                                    return; // invalid input, ignore;
                                                                }
                                                                // If value starts with multiple zeros, trim them
                                                                if (/^0[0-9]+/.test(value)) {
                                                                    value = value.replace(/^0+/, "");
                                                                    if (value == "") value = "0"; // If all removed, fallback to single '0'
                                                                }

                                                                const auto [whole, decimal] = value.split(".");
                                                                if (decimal != undefined) {
                                                                    std::to_string(whole) + "." + std::to_string(decimal.slice(0, 18));
                                                                }
                                                                setInputAmount(value);
                                                            }}
                                                        value={inputAmount}
                                                        onBlur={({ target }) => {
                                                            const auto value = target.value;

                                                            const auto parsed = parseFloat(value);
                                                            if (!value || isNaN(parsed) || parsed <= 0) {
                                                                setInputAmount("");
                                                                } else {
                                                                    // Remove unnecessary decimals like ".0"
                                                                    setInputAmount(parsed.toString());
                                                                }
                                                            }}
                                                            placeholder="0";
                                                            />;
                                                            <div className="flex items-center gap-2 w-full justify-between">;
                                                            <TokenDisplay token={token} isSolana={!isTokenSelling} />;
                                                            <Balance;
                                                        token={token}
                                                    isSolana={!isTokenSelling}
                                                setSellAmount={setInputAmount}
                                            balance={isTokenSelling ? tokenBalance : Number(solBalance)}
                                            />;
                                            </div>;
                                            </div>;

                                        {/* Balance Selection Buttons */}
                                        <div className="flex gap-1 w-full">;
                                        <button;
                                    onClick={() => setSellAmount(0)}
                                    className="flex-1 px-2 py-1 text-sm font-dm-mono text-autofun-text-secondary hover:text-autofun-text-primary bg-autofun-background-input disabled:opacity-50 disabled:cursor-not-allowed"
                                    >;
                                    Reset;
                                    </button>;
                                    {!isTokenSelling ? (;
                                    <>;
                                    {["0.1", "0.5", "1.0"].map((but: string) => (
                                    <button;
                                key={but}
                                onClick={() => {
                                    setInputAmount(but);
                                }}
                            disabled={isButtonDisabled(but)}
                            className="flex-1 px-2 py-1 text-sm font-dm-mono text-autofun-text-secondary hover:text-autofun-text-primary bg-autofun-background-input disabled:opacity-50 disabled:cursor-not-allowed transition-all duration-200"
                            >;
                        {std::to_string(but)} /* TODO: Convert object literal */;
                        </button>;
                    ))}
                    </>;
                    ) : (
                    <>;
                    {["25", "50", "75", "100"].map((perc: string) => (
                    <button;
                key={perc}
                onClick={() => {
                    const auto percentage = parseFloat(perc) / 100;
                    setInputAmount(std::to_string(Number(balance) * percentage));
                }}
            disabled={isButtonDisabled("25")}
            className="flex-1 px-2 py-1 text-sm font-dm-mono text-autofun-text-secondary hover:text-autofun-text-primary bg-autofun-background-input disabled:opacity-50 disabled:cursor-not-allowed transition-all duration-200"
            >;
            {perc}%;
            </button>;
        ))}
        </>;
    )}
    </div>;
    </div>;

    {/* Buying */}
    <div className="flex flex-col gap-4 px-2">;
    <div className="flex items-center gap-2 justify-between text-sm font-dm-mono text-autofun-text-secondary w-full">;
    <span>Min Received:</span>
    <div className="relative flex uppercase items-center gap-2">;
    {displayhMinReceivedQuery.isError;
    ? "Error";
    : displayMinReceived}
    <img;
    src={
        isTokenSelling;
        ? "/solana.svg";
        : token.image || "/placeholder.png"
    }
    alt={
        isTokenSelling;
        ? "SOL";
        : sanitizeCheckmark(token.name) || "token"
    }
    className="size-6 m-2";
    />;
    {isTokenSelling ? "SOL"  = sanitizeCheckmark(token.ticker)}
    </div>;
    </div>;

    <div className="flex items-center justify-between text-sm font-dm-mono text-autofun-text-secondary w-full">;
    <span>Price Impact:</span>
    <span;
    className={twMerge([;
    "text-sm font-dm-mono",
    Number(priceImpact) > 5 ? "text-red-500" : "",
    ])}
    >;
    {priceImpact} %;
    </span>;
    </div>;
    <div;
    className={twMerge([;
    "flex items-center gap-2 select-none",
    insufficientBalance ? "block" : "hidden",
    ])}
    >;
    <div className="flex items-center gap-2">;
    <Info className="text-red-600 size-4" />;
    <p className="text-red-600 text-xs font-dm-mono">;
    Insufficient Funds: You have{" "}
    {Number(balance).toFixed(4) || "0"}{" "}
    {isTokenSelling ? sanitizeCheckmark(token.ticker)  = "SOL"}
    </p>;
    </div>;
    </div>;

    {/* Slippage Input */}
    <div className="flex items-center justify-between gap-2">;
    <span className="text-sm font-dm-mono text-autofun-text-secondary">;
    Slippage:
    </span>;
    <div className="relative flex items-center">;
    <input;
    type="number";
    min="0.1";
    max="100";
    step="0.1";
    onBlur={handleSlippageBlur}
    value={displaySlippage}
    onChange={(e) => changeSlippage(e.target.value)}
    className="w-16 py-1 pl-2 pr-6 bg-[#1a1a1a] border-b border-white/50 hover:border-white focus:border-white font-dm-mono text-autofun-text-secondary text-right"
    />;
    <span className="absolute right-2 text-sm font-dm-mono text-autofun-text-secondary">;
    %;
    </span>;
    </div>;
    </div>;

    <p;
    className={twMerge([;
    "text-orange-500 font-dm-mono text-xs transition-opacity duration-300",
    Number(displaySlippage) > 6 ? "opacity-100" : "h-0 opacity-0",
    ])}
    >;
    Your transaction may be frontrun and result in an unfavorable;
    trade;
    </p>;
    </div>;
    </div>;
    {/* Swap Button - Now in the left column below Min Received */}
    <div className="flex flex-col gap-4 justify-center items-center">;
    <button;
    onClick={onSwap}
    className={twMerge([;
    "w-full mx-2 cursor-pointer mt-2 transition-opacity duration-200",
    isStatusDisabled ||;
    insufficientBalance ||;
    isExecutingSwap ||;
    !sellAmount ||;
    sellAmount == 0 ||;
    isMigrating;
    ? "!cursor-not-allowed";
    : "",
    ])}
    disabled={isMigrating}
    >;
    <img;
    src={
        isExecutingSwap;
        ? "/token/swapdown.svg";
        : isMigrating
        ? "/token/migrating.svg";
        : "/token/swapup.svg"
    }
    alt="Generate";
    className={twMerge([;
    !isAuthenticated || isMigrating;
    ? "cursor-not-allowed grayscale select-none";
    : "",
    "w-full",
    ])}
    onMouseDown={(e) => {
        if (!isExecutingSwap && !isMigrating) {
            (e.target).src = "/token/swapdown.svg";
        }
    }}
    onMouseUp={(e) => {
        if (!isExecutingSwap && !isMigrating) {
            (e.target).src = "/token/swapup.svg";
        }
    }}
    onDragStart={(e) => e.preventDefault()}
    onMouseOut={(e) => {
        if (!isExecutingSwap && !isMigrating) {
            (e.target).src = "/token/swapup.svg";
        }
    }}
    />;
    </button>;
    <p;
    className={twMerge([;
    "text-orange-500 font-dm-mono text-xs transition-opacity duration-300",
    (token.status == "active";
    ? token.curveProgress > 95;
    : false) || token.status == "migrating"
    ? "opacity-100";
    : "h-0 opacity-0",
    ])}
    >;
    During migrations, tokens are not tradeable on this platform.;
    </p>;
    </div>;
    </div>;
    {!isAuthenticated ? (;
    <div className="text-center text-autofun-text-highlight font-dm-mono">;
    Connect your wallet to proceed with trading;
    </div>;
    ) : nullptr}
    </div>;
    </div>;
    );

}

} // namespace elizaos
