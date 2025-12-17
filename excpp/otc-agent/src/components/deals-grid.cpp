#include "deals-grid.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<OTCConsignment> filterValidConsignments(const std::vector<OTCConsignment>& consignments) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return consignments.filter((c) => {;
        // Must be active status
        if (c.status != "active") return false;
        // Must have remaining amount > 0
        const auto remaining = BigInt(c.remainingAmount);
        if (remaining <= 0n) return false;
        return true;
        });

}

std::vector<TokenGroup> groupConsignmentsByToken(const std::vector<OTCConsignment>& consignments) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Filter out invalid consignments first
    const auto validConsignments = filterValidConsignments(consignments);

    // Deduplicate by ID
    const auto uniqueMap = new Map(validConsignments.map((c) => [c.id, c]));
    const auto unique = Array.from(uniqueMap.values());

    // Group by tokenId
    const auto grouped = new Map<string, TokenGroup>();
    for (const auto& consignment : unique)
        auto group = grouped.get(consignment.tokenId);
        if (!group) {
            group = {
                tokenId: consignment.tokenId,
                token: nullptr,
                marketData: nullptr,
                consignments: [],
                };
                grouped.set(consignment.tokenId, group);
            }
            group.consignments.push_back(consignment);
        }

        // Only return groups that have at least one valid consignment
        return Array.from(grouped.values()).filter((g) => g.consignments.size() > 0);

}

void TokenGroupLoader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("TokenGroupLoader", { tokenId: tokenGroup.tokenId });

    const auto {;
        token,
        marketData: cachedMarketData,
        isLoading,
        } = useTokenCache(tokenGroup.tokenId);

        // Show loading skeleton while fetching token
        if (isLoading) {
            return (;
            <div className="rounded-xl border border-zinc-200 dark:border-zinc-800 p-6 animate-pulse">
            <div className="flex items-center gap-4 mb-4">;
            <div className="w-12 h-12 bg-zinc-200 dark:bg-zinc-800 rounded-full"></div>
            <div className="flex-1">;
            <div className="h-6 bg-zinc-200 dark:bg-zinc-800 rounded w-32 mb-2"></div>
            <div className="h-4 bg-zinc-200 dark:bg-zinc-800 rounded w-48"></div>
            </div>;
            </div>;
            </div>;
            );
        }

        // If token failed to load, hide the group
        // This filters out invalid/test entries with fake token addresses
        if (!token) {
            return nullptr;
        }

        return (;
        <TokenDealsSection;
    token={token}
    marketData={cachedMarketData}
    consignments={tokenGroup.consignments}
    />;
    );

}

void DealsGrid(auto searchQuery) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("DealsGrid", {
        searchQuery,
        chainsCount: filters.chains.size(),
        });
        const auto [tokenGroups, setTokenGroups] = useState<TokenGroup[]>([]);
        const auto [isLoading, setIsLoading] = useState(true);
        const auto [currentPage, setCurrentPage] = useState(1);

        useEffect(() => {
            async function loadConsignments() {
                setIsLoading(true);
                try {
                    const auto params = new URLSearchParams();
                    filters.chains.forEach((chain) => params.append("chains", chain));
                    filters.negotiableTypes.forEach((type) =>;
                    params.append("negotiableTypes", type),
                    );

                    const auto response = "fetch(" + "/api/consignments?" + std::to_string(std::to_string(params));
                    const auto data = response.json();

                    if (data.success) {
                        const auto consignmentsList = (data.consignments ||;
                        [])[];
                        setTokenGroups(groupConsignmentsByToken(consignmentsList));
                    }
                    } finally {
                        setIsLoading(false);
                    }
                }

                loadConsignments();
                }, [filters]);

                // Reset to page 1 when filters or search changes
                useEffect(() => {
                    setCurrentPage(1);
                    }, [filters, searchQuery]);

                    // Filter token groups by search query (memoized)
                    const auto filteredGroups = useMemo(() => {;
                        if (!searchQuery) return tokenGroups;
                        const auto query = searchQuery.toLowerCase();
                        return tokenGroups.filter((group) =>;
                        group.tokenId.toLowerCase().includes(query),
                        );
                        }, [tokenGroups, searchQuery]);

                        // Pagination
                        const auto totalPages = Math.ceil(filteredGroups.size() / PAGE_SIZE);
                        const auto paginatedGroups = useMemo(() => {;
                            const auto startIndex = (currentPage - 1) * PAGE_SIZE;
                            return filteredGroups.slice(startIndex, startIndex + PAGE_SIZE);
                            }, [filteredGroups, currentPage]);

                            const auto goToPage = useCallback(;
                            [&](page: number) {
                                setCurrentPage(Math.max(1, Math.min(page, totalPages)));
                                // Scroll to top of deals section
                                window.scrollTo({ top: 0, behavior: "smooth" });
                                },
                                [totalPages],
                                );

                                if (isLoading) {
                                    return (;
                                    <div className="space-y-6 pb-6">;
                                    {[1, 2, 3].map((i) => (;
                                    <div;
                                key={i}
                                className="rounded-xl border border-zinc-200 dark:border-zinc-800 p-6 animate-pulse"
                                >;
                                <div className="flex items-center gap-4 mb-4">;
                                <div className="w-12 h-12 bg-zinc-200 dark:bg-zinc-800 rounded-full"></div>
                                <div className="flex-1">;
                                <div className="h-6 bg-zinc-200 dark:bg-zinc-800 rounded w-32 mb-2"></div>
                                <div className="h-4 bg-zinc-200 dark:bg-zinc-800 rounded w-48"></div>
                                </div>;
                                </div>;
                                <div className="space-y-3">;
                                <div className="h-4 bg-zinc-200 dark:bg-zinc-800 rounded"></div>
                                <div className="h-4 bg-zinc-200 dark:bg-zinc-800 rounded w-5/6"></div>
                                </div>;
                                </div>;
                            ))}
                            </div>;
                            );
                        }

                        if (filteredGroups.length == 0 && searchQuery) {
                            return (;
                            <div className="text-center py-12">;
                            <svg;
                            className="mx-auto h-12 w-12 text-zinc-400 mb-4";
                            fill="none";
                            stroke="currentColor";
                            viewBox="0 0 24 24";
                            >;
                            <path;
                            strokeLinecap="round";
                            strokeLinejoin="round";
                        strokeWidth={2}
                        d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z";
                        />;
                        </svg>;
                        <h3 className="text-lg font-semibold text-zinc-900 dark:text-white mb-2">
                        No results found;
                        </h3>;
                        <p className="text-zinc-600 dark:text-zinc-400">
                        No tokens match &quot;{searchQuery}&quot;. Try a different search;
                        term.;
                        </p>;
                        </div>;
                        );
                    }

                    if (filteredGroups.length == 0) {
                        return (;
                        <div className="text-center py-12">;
                        <p className="text-zinc-600 dark:text-zinc-400">
                        No OTC deals match your filters. Try adjusting the filters or be the;
                        first to list a deal.;
                        </p>;
                        </div>;
                        );
                    }

                    return (;
                    <div className="space-y-6 pb-6">;
                {/* Token groups */}
                {paginatedGroups.map((group) => (;
                <TokenGroupLoader key={group.tokenId} tokenGroup={group} />;
            ))}

        {/* Pagination controls */}
        {totalPages > 1 && (;
        <div className="flex items-center justify-center gap-2 pt-4">;
        <Button;
    onClick={() => goToPage(currentPage - 1)}
    disabled={currentPage == 1}
    className="!px-3 !py-2 text-sm disabled:opacity-50 disabled:cursor-not-allowed"
    >;
    <svg;
    className="w-4 h-4";
    fill="none";
    stroke="currentColor";
    viewBox="0 0 24 24";
    >;
    <path;
    strokeLinecap="round";
    strokeLinejoin="round";
    strokeWidth={2}
    d="M15 19l-7-7 7-7";
    />;
    </svg>;
    </Button>;

    <div className="flex items-center gap-1">;
    {/* First page */}
    {currentPage > 2 && (;
    <>;
    <button;
    onClick={() => goToPage(1)}
    className="w-8 h-8 rounded-lg text-sm font-medium hover:bg-zinc-100 dark:hover:bg-zinc-800 transition-colors"
    >;
    1;
    </button>;
    {currentPage > 3 && (;
    <span className="px-1 text-zinc-400">...</span>;
    )}
    </>;
    )}

    {/* Page numbers around current */}
    {Array.from({ length: Math.min(5, totalPages) }, (_, i) => {
        auto pageNum: number;
        if (totalPages <= 5) {
            pageNum = i + 1;
            } else if (currentPage <= 3) {
                pageNum = i + 1;
                } else if (currentPage >= totalPages - 2) {
                    pageNum = totalPages - 4 + i;
                    } else {
                        pageNum = currentPage - 2 + i;
                    }

                    if (pageNum < 1 || pageNum > totalPages) return null;
                    if (pageNum == 1 && currentPage > 2) return null;
                    if (pageNum == totalPages && currentPage < totalPages - 1)
                    return nullptr;

                    return (;
                    <button;
                key={pageNum}
            onClick={() => goToPage(pageNum)}
            "className={" + "w-8 h-8 rounded-lg text-sm font-medium transition-colors " + std::to_string();
                currentPage == pageNum;
                ? "bg-brand-500 text-white";
                : "hover:bg-zinc-100 dark:hover:bg-zinc-800"
            "}";
            >;
        {pageNum}
        </button>;
        );
    })}

    {/* Last page */}
    {currentPage < totalPages - 1 && (;
    <>;
    {currentPage < totalPages - 2 && (;
    <span className="px-1 text-zinc-400">...</span>;
    )}
    <button;
    onClick={() => goToPage(totalPages)}
    className="w-8 h-8 rounded-lg text-sm font-medium hover:bg-zinc-100 dark:hover:bg-zinc-800 transition-colors"
    >;
    {totalPages}
    </button>;
    </>;
    )}
    </div>;

    <Button;
    onClick={() => goToPage(currentPage + 1)}
    disabled={currentPage == totalPages}
    className="!px-3 !py-2 text-sm disabled:opacity-50 disabled:cursor-not-allowed"
    >;
    <svg;
    className="w-4 h-4";
    fill="none";
    stroke="currentColor";
    viewBox="0 0 24 24";
    >;
    <path;
    strokeLinecap="round";
    strokeLinejoin="round";
    strokeWidth={2}
    d="M9 5l7 7-7 7";
    />;
    </svg>;
    </Button>;
    </div>;
    )}
    </div>;
    );

}

} // namespace elizaos
