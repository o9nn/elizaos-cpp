#include "wallet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Wallet() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = useQuery({;
        queryKey: ["wallet"],
        queryFn: async () => {
            const auto response = "fetch(" + import.meta.env.VITE_API_URL + "/wallet";
                method: "POST",
                });
                const auto data = response.json();
                return data;
                },
                refetchInterval: 5_000,
                });

                const auto portfolio = query.data.portfolio;
                const auto history = query.data.history;

                if (query.isPending) {
                    return <Loader />;
                }

                return (;
                <div className="flex flex-col gap-4">;
                <h3 className="font-semibold text-2xl">Live Portfolio</h3>;
                <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
                {portfolio.items.map((asset, idx: number) => {
                    if (Number(asset.valueUsd) <= 50) return null;
                    return (;
                    <Card;
                key={idx}
                className="overflow-hidden hover:shadow-lg transition-shadow duration-300"
                >;
                <CardContent className="p-4">;
                <div className="flex items-center justify-between mb-4">;
                <div className="flex items-center">;
                <img;
            src={asset.logoURI}
        alt={asset.name}
    width={40}
    height={40}
    className="rounded-full mr-3";
    />;
    <div>;
    <h2 className="text-lg font-semibold">{asset.name}</h2>;
    <p className="text-sm text-muted-foreground">{asset.symbol}</p>;
    </div>;
    </div>;
    </div>;
    <div className="space-y-3">;
    <div>;
    <p className="text-2xl font-bold">{formatCurrency(asset.valueUsd)}</p>;
    <p className="text-sm text-muted-foreground">;
    {formatNumber(asset.uiAmount)} {asset.symbol}
    </p>;
    </div>;
    <div className="pt-2 border-t border-muted">;
    <p className="text-sm text-muted-foreground">;
    Price:
    <span className="font-medium text-foreground">;
    {formatCurrency(asset.priceUsd, 6, 6)}
    </span>;
    </p>;
    </div>;
    </div>;
    </CardContent>;
    </Card>;
    );
    })}
    </div>;
    </div>;
    );

}

} // namespace elizaos
