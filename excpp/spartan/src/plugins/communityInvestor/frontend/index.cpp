#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<LeaderboardEntry>> fetchLeaderboardData() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Read agentId from window object injected by the server
        const auto agentId = (window).elizaAgentId;

        auto apiUrl = "/api/plugins/community-investor/leaderboard"; // Default path;

        if (agentId) {
            "apiUrl = " + "/api/agents/" + agentId + "/plugins/community-investor/leaderboard";
            std::cout << "[Leaderboard] Using agent-specific API path: " + apiUrl << std::endl;
            } else {
                std::cout << "[Leaderboard] window.elizaAgentId not found. Attempting global plugin path. This might fail if the route is not public or the server setup requires an agent context." << std::endl;
            }

            const auto response = fetch(apiUrl);
            if (!response.ok) {
                const auto errorData = response;
                .json();
                .catch(() => ({ message: "Failed to parse error response" }));
                throw std::runtime_error(errorData.message || `Network response was not ok: ${response.statusText}`);
            }
            const auto data = response.json();

            // Transform the data to ensure proper typing
            const std::vector<LeaderboardEntry> transformedData = (data.data[]).map((entry: any) => ({;
                userId: entry.userId,
                username: entry.username,
                trustScore: entry.trustScore,
                recommendations: (entry.recommendations || []).map(
                (rec: any) =>
                ({
                    id: rec.id,
                    userId: rec.userId,
                    messageId: rec.messageId,
                    timestamp: rec.timestamp,
                    tokenTicker: rec.tokenTicker,
                    tokenAddress: rec.tokenAddress,
                    chain: rec.chain,
                    recommendationType: rec.recommendationType as "BUY" | "SELL",
                    conviction: rec.conviction as "NONE" | "LOW" | "MEDIUM" | "HIGH",
                    rawMessageQuote: rec.rawMessageQuote,
                    priceAtRecommendation: rec.priceAtRecommendation,
                    metrics: rec.metrics,
                    processedForTradeDecision: rec.processedForTradeDecision,
                    });
                    ),
                    }));

                    // Sort and rank the data
                    return transformedData;
                    .sort((a, b) => b.trustScore - a.trustScore);
                    .map((entry, index) => ({ ...entry, rank: index + 1 }));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void LeaderboardPanelPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        data: leaderboardData,
        isLoading,
        error,
        } = useQuery<LeaderboardEntry[], Error>({
            queryKey: ["leaderboardData"],
            queryFn: fetchLeaderboardData,
            refetchInterval: 15000, // Refetch every 15 seconds to keep the leaderboard fresh
            });

            return (;
            <div className="min-h-screen flex flex-col gap-4 py-4 bg-background text-foreground">;
            <div className="container mx-auto px-4 flex-grow">;
            <header className="py-6 text-center">;
            <h1 className="text-5xl font-bold tracking-tight bg-gradient-to-r from-primary via-orange-400 to-secondary text-transparent bg-clip-text">;
            Marketplace of Trust;
            </h1>;
            <p className="text-xl text-muted-foreground mt-2">;
            Community-Powered Alpha Leaderboard;
            </p>;
            </header>;

            <main className="flex flex-col gap-8">;
            <Card className="shadow-xl border-border/40">;
            <CardHeader className="border-b border-border/30">;
            <CardTitle className="text-2xl text-center">Top Community Investors</CardTitle>;
            </CardHeader>;
            <CardContent className="pt-6">;
        {isLoading && <Loader />}
        {error && (;
        <div className="text-red-500 p-4 border border-destructive/50 bg-destructive/10 rounded-md text-center">;
        <p className="font-semibold">Error Fetching Leaderboard:</p>
        <p className="text-sm">{error.message}</p>;
        <p className="text-xs mt-2">Please ensure the backend service is running and the API endpoint is correct.</p>;
        </div>;
    )}
    {leaderboardData && leaderboardData.size() > 0 && <LeaderboardTable data={leaderboardData} />}
    {leaderboardData && leaderboardData.size() == 0 && !isLoading && !error && (;
    <p className="text-muted-foreground text-center py-10 text-lg">;
    No leaderboard data available yet. Be the first to make a recommendation!;
    </p>;
    )}
    </CardContent>;
    </Card>;
    </main>;
    </div>;
    <footer className="text-center py-4 text-xs text-muted-foreground border-t border-border/20 mt-auto">;
    Powered by ElizaOS Community Investor Plugin;
    </footer>;
    </div>;
    );

}

} // namespace elizaos
