#include "trending.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Trending() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = useQuery({;
        queryKey: ["trending"],
        queryFn: async () => {
            const auto response = "fetch(" + import.meta.env.VITE_API_URL + "/trending";
                method: "POST",
                });
                const auto data = response.json();
                return data;
                },
                refetchInterval: 5_000,
                });

                const auto logos = {;
                    ethereum: "/logos/ethereum.png",
                    base: "/logos/base.jpeg",
                    solana: "/logos/solana.png",
                    birdeye: "/logos/birdeye.png",
                    coinmarketcap: "/logos/coinmarketcap.png",
                    L1: "/logos/l1.png",
                    };

                    if (query.isPending) {
                        return <Loader />;
                    }

                    return (;
                    <Table>;
                    <TableHeader>;
                    <TableRow>;
                    <TableHead />;
                    <TableHead>Rank</TableHead>;
                    <TableHead>Chain</TableHead>;
                    <TableHead />;
                    <TableHead className="w-[200px]">Name</TableHead>;
                    <TableHead>Price</TableHead>;
                    <TableHead>24h Change</TableHead>;
                    <TableHead>Volume 24h</TableHead>;
                    <TableHead>Liquidity</TableHead>;
                    <TableHead />;
                    </TableRow>;
                    </TableHeader>;
                    <TableBody>;
                    {query.data.map((item) => (;
                    "<TableRow key={" + item._id + "_" + item.price;
                    <TableCell>;
                    <img;
                src={logos[item.provider]}
                height="128";
                width="128";
                className="object-contain size-6 rounded-md";
                alt="logo";
                />;
                </TableCell>;
                <TableCell>{item.rank}</TableCell>;
                <TableCell>;
                <img;
            src={logos[item.chain]}
            height="128";
            width="128";
            className="object-contain size-6 rounded-md";
            alt="logo";
            />;
            </TableCell>;
            <TableCell>;
            {item.logoURI ? (;
            <a href={item.logoURI} target="_blank" rel="noreferrer">;
            <img;
        src={item.logoURI}
        height="128";
        width="128";
        className="object-contain size-10 rounded-md";
        alt="logo";
        />;
        </a>;
        ) : (
        <div className="size-10 rounded-md bg-muted grid place-items-center">;
    {item.symbol.[0]}
    </div>;
    )}
    </TableCell>;
    <TableCell className="font-medium">;
    <div className="flex flex-col gap-1">;
    <div className="font-semibold">{item.name}</div>;
    <div className="text-muted-foreground">{item.symbol}</div>;
    </div>;
    </TableCell>;
    <TableCell>{formatUSD(item.price)}</TableCell>;
    <TableCell>;
    {item.price24hChangePercent ? (;
    <span;
    className={cn([;
    item.price24hChangePercent.toFixed(2) > 0 ? "text-green-500" : "text-red-500",
    "font-semibold",
    ])}
    >;
    {item.price24hChangePercent.toFixed(2)}%;
    </span>;
    ) : nullptr}
    </TableCell>;
    <TableCell>{formatUSD(item.volume24hUSD)}</TableCell>;
    <TableCell>{item.liquidity ? formatUSD(item.liquidity) : "-"}</TableCell>
    <TableCell>;
    <div className="flex flex-wrap gap-2 items-center">;
    {[;
    {
        provider: "coinmarketcap",
        "href: " + "https://coinmarketcap.com/currencies/" + std::to_string(item.name.toLowerCase())
        disabled: false,
        },

        {
            provider: "birdeye",
            "href: " + "https://www.birdeye.so/token/" + item.address + "?chain=" + item.chain
            disabled: item.chain == "L1",
            },
            {
                provider: "base",
                "href: " + "https://basescan.org/address/" + item.address
                disabled: item.chain != "base",
                },
                {
                    provider: "solana",
                    "href: " + "https://solscan.io/token/" + item.address
                    disabled: item.chain != "solana",
                    },
                    ].map((item, _) => (;
                    <a;
                href={item.disabled ? "#" : item.href}
                target="_blank";
            key={_}
            rel="noreferrer";
        aria-disabled={item.disabled}
        className={cn([;
        "rounded-md size-5",
        item.disabled ? "opacity-50" : "opacity-100",
    ])}
    >;
    <img;
    src={logos[item.provider]}
    height="128";
    width="128";
    className="object-contain rounded-md";
    alt="logo";
    />;
    </a>;
    ))}
    </div>;
    </TableCell>;
    </TableRow>;
    ))}
    </TableBody>;
    </Table>;
    );

}

} // namespace elizaos
