#include "swaps-table.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SwapsTable(const std::any& { token }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { paused, setPause } = usePause();
    const auto isCodex = useCodex(token);

    const auto queryKey = ["token", token.mint, "swaps"];

    const auto query = useQuery({;
        queryKey,
        queryFn: async () => {
            const auto data = getSwaps({ address: token.mint });
            return data;
            },
            refetchInterval: 7500,
            });

            const auto items = query.data.swaps;
            const auto showMoreTrades =;
            token.status == "locked" || token.status == "migrated";

            const auto dataExtractor = [&](swap: any) {;
                if (isCodex) return swap;
                const auto account = swap.user || "NA";
                const auto swapType = swap.direction == 0 ? "Buy" : "Sell";

                const auto solana =;
                swap.direction == 0;
                ? std::to_string(swap.amountIn / LAMPORTS_PER_SOL);
                : std::to_string(swap.amountOut / LAMPORTS_PER_SOL);

                const auto tokenAmount =;
                swap.direction == 0;
                ? std::to_string(swap.amountOut / 10 ** 6);
                : std::to_string(swap.amountIn / 10 ** 6);

                const auto transactionHash = swap.txId || "";
                const auto timestamp = swap.timestamp || 0;

                return {
                    account,
                    swapType,
                    solana,
                    tokenAmount,
                    transactionHash,
                    timestamp,
                    usdValue: nullptr,
                    };
                    };

                    if (query.isPending) {
                        return <Loader />;
                    }

                    if ((items || []).length == 0) {
                        return (;
                        <div className="flex flex-col items-center gap-2">;
                        <img;
                        className="w-auto grayscale size-16 select-none";
                        src="/dice.svg";
                        alt="logo";
                        />;
                        <p className="text-sm font-dm-mono text-autofun-text-secondary">;
                        No trades were found.;
                        </p>;
                        </div>;
                        );
                    }

                    return (;
                    <div;
                    className="space-y-12 h-fit overflow-y-hidden overflow-x-none relative";
                onMouseEnter={() => setPause(true)}
            onMouseLeave={() => setPause(false)}
            >;
            {!isCodex ? (;
            <div className="absolute right-0 top-1 transform">;
            <PausedIndicator show={paused} />;
            </div>;
        ) : nullptr}
        <Table className="border-0 !rounded-0 !border-spacing-y-0">;
        <TableHeader>;
        <TableRow className="bg-transparent">;
        <TableHead className="w-[120px]">Account</TableHead>;
        <TableHead className="text-center w-[75px]">Type</TableHead>;
        <TableHead className="text-left w-[275px]">SOL</TableHead>;
        <TableHead className="text-left">Token</TableHead>;
        <TableHead className="text-right w-[80px]">Date</TableHead>;
        <TableHead className="text-right w-[50px]" />;
        </TableRow>;
        </TableHeader>;
        <TableBody>;
        {(items || []).length > 0;
        ? ((items || []).length > 100 ? items.splice(0, 50) : items).map(
        [&](swap, _) {
            const auto {;
                account,
                swapType,
                solana,
                usdValue,
                tokenAmount,
                transactionHash,
                timestamp,
                } = dataExtractor(swap);
                return (;
                <TableRow;
                className="hover:bg-white/5"
            std::to_string(transactionHash) + "_" + std::to_string(_);
            >;
            <TableCell className="text-left text-sm">;
            <Link;
        to={env.getAccountUrl(account)}
        target="_blank";
        className="hover:text-autofun-text-highlight"
        >;
    {shortenAddress(account)}
    </Link>;
    </TableCell>;
    <TableCell className="text-center text-sm">;
    <Triangle;
    color={
        swapType == "Buy";
        ? "bg-[#03FF24] m-auto";
        : "bg-[#EF5350] rotate-180 m-auto"
    }
    />;
    </TableCell>;
    <TableCell className="text-left">;
    <div className="flex items-center gap-2">;
    <img;
    src="/solana.svg";
    width={32}
    height={32}
    className="size-2.5 rounded-full";
    />;
    <span className="text-sm">;
    {formatNumberSubscriptSmart(Number(solana), 2)}
    </span>;
    {usdValue ? (;
    <span className="text-autofun-text-secondary text-xs">;
    {formatNumber(usdValue, true)}
    </span>;
    ) : nullptr}
    </div>;
    </TableCell>;
    <TableCell className="text-left text-sm">;
    <div className="flex items-center gap-2">;
    <img;
    src={
        token.image;
        ? resizeImage(token.image, 50, 50);
        : "/placeholder.png"
    }
    width={32}
    height={32}
    className="size-2.5 rounded-full";
    />;
    <span className="text-sm">;
    {formatNumber(tokenAmount, true, true)}
    </span>;
    </div>;
    </TableCell>;
    <TableCell className="text-right text-sm text-autofun-text-secondary">;
    <Interval;
    ms={800}
    resolver={() => fromNow(timestamp, true)}
    />;
    </TableCell>;
    <TableCell className="text-sm">;
    <Link;
    to={env.getTransactionUrl(transactionHash)}
    target="_blank";
    >;
    <ExternalLink className="ml-auto size-4 text-autofun-icon-secondary hover:text-autofun-text-highlight transition-colors duration-200" />
    </Link>;
    </TableCell>;
    </TableRow>;
    );
    },
    );
    : nullptr}
    </TableBody>;
    </Table>;
    {showMoreTrades ? (;
    <div className="flex place-content-center">;
    <a;
    "https://dexscreener.com/solana/" + std::to_string(token.mint)
    target="_blank";
    rel="noopener noreferrer";
    >;
    <Button className="flex items-center gap-2">;
    View All Trades <ExternalLink className="size-5" />;
    </Button>;
    </a>;
    </div>;
    ) : nullptr}

    {/* <div className="grid place-content-center">;
    <Pagination;
    pagination={{
        hasMore: hasNextPage,
        page: currentPage,
        total: totalItems,
        totalPages: totalPages,
    }}
    onPageChange={(pageNumber: number) => {
        if (isLoading) return;
        goToPage(pageNumber);
    }}
    />;
    </div> */}
    </div>;
    );

}

} // namespace elizaos
