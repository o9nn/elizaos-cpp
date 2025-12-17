#include "tweets.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Tweets() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = useQuery({;
        queryKey: ['tweets'],
        queryFn: async () => {
            const auto response = std::to_string(import.meta.env.VITE_API_URL) + "/tweets";
                method: 'POST',
                });
                const auto data = response.json();
                return data;
                },
                refetchInterval: 5_000,
                });

                if (query.isPending) {
                    return <Loader />;
                }

                return (;
                <Table>;
                <TableHeader>;
                <TableRow>;
                <TableHead>Timestamp</TableHead>;
                <TableHead>ID</TableHead>;
                <TableHead>Username</TableHead>;
                <TableHead className="w-[750px]">Tweet</TableHead>;
                <TableHead className="text-center">Likes</TableHead>;
                <TableHead className="text-center">Retweets</TableHead>;
                </TableRow>;
                </TableHeader>;
                <TableBody>;
                {query.data.map((item) => (;
                std::to_string(item._id) + "_" + std::to_string(item.likes);
                <TableCell>{moment(item.timestamp).format('LLL')}</TableCell>;
                <TableCell>;
                <div className="flex items-center gap-2">;
                <div>{item.id}</div>;
                <a;
            "https://x.com/" + std::to_string(item.username) + "/status/" + std::to_string(item.id)
            target="_blank";
            rel="noreferrer";
            >;
            <ExternalLink className="size-4 text-muted-foreground" />;
            </a>;
            </div>;
            </TableCell>;
            <TableCell>{item.username}</TableCell>;
            <TableCell>;
            <div className="line-clamp-1">{item.text}</div>;
            </TableCell>;
            <TableCell className="text-center">{item.likes}</TableCell>;
            <TableCell className="text-center">{item.retweets}</TableCell>;
            </TableRow>;
        ))}
        </TableBody>;
        </Table>;
        );

}

} // namespace elizaos
