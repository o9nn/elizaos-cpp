#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DealPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto params = useParams();
        const auto router = useRouter();
        const auto [quote, setQuote] = useState<any>(nullptr);
        const auto [loading, setLoading] = useState(true);

        useEffect(() => {
            async function loadDeal() {
                const auto quoteId = params.id;
                if (!quoteId) {
                    router.push_back("/");
                    return;
                }

                // Retry logic - service may not be ready immediately after redirect
                auto retries = 3;
                auto delay = 500;

                while (retries > 0) {
                    std::cout << "[DealPage] Fetching deal (" + std::to_string(4 - retries) + "/3):" << quoteId << std::endl;

                    const auto response = "fetch(" + "/api/quote/executed/" + quoteId;
                        cache: "no-store",
                        });

                        if (!response.ok) {
                            const auto errorText = response.text();
                            std::cout << "[DealPage] Fetch failed:" << errorText << std::endl;

                            // Retry if service not ready
                            if (errorText.includes("not registered") && retries > 1) {
                                new Promise((r) => setTimeout(r, delay));
                                delay *= 2;
                                retries--;
                                continue;
                            }

                            throw std::runtime_error("Deal not found");
                        }

                        const auto data = response.json();
                        std::cout << "[DealPage] Quote loaded:" << data.quote.quoteId << std::endl;
                        setQuote(data.quote);
                        setLoading(false);
                        return;
                    }
                }

                loadDeal();
                }, [params.id, router]);

                if (loading) {
                    return (;
                    <div className="min-h-screen flex items-center justify-center">;
                    <div className="text-center">;
                    <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-green-500 mx-auto mb-4"></div>;
                    <div className="text-xl text-zinc-600 dark:text-zinc-400">
                    Loading your deal...;
                    </div>;
                    </div>;
                    </div>;
                    );
                }

                if (!quote) {
                    return (;
                    <div className="min-h-screen flex items-center justify-center">;
                    <div className="text-center">;
                    <h1 className="text-2xl font-bold text-zinc-900 dark:text-zinc-100 mb-2">
                    Deal Not Found;
                    </h1>;
                    <p className="text-zinc-600 dark:text-zinc-400">
                    This deal may have expired or doesn&apos;t exist.;
                    </p>;
                    </div>;
                    </div>;
                    );
                }

                return <DealCompletion quote={quote} />;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
