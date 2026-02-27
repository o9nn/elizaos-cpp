#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/user/getUsers.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    try
    {
        auto cursor = OR((parseInt(as<string>(req->query->cursor))), (1));
        auto limit = OR((parseInt(as<string>(req->query->limit))), (100));
        auto startIndex = (cursor - 1) * limit;
        auto endIndex = startIndex + limit;
        auto paginatedUsers = mockUsers->slice(startIndex, endIndex);
        console->log(std:("Returning ") + paginatedUsers->get_length() + std:(" mock users for trust scoreboard"));
        return res->status(200)->json(object{
            object::pair{std:("users"), paginatedUsers}, 
            object::pair{std:("total"), mockUsers->get_length()}, 
            object::pair{std:("cursor"), cursor}, 
            object::pair{std:("hasMore"), endIndex < mockUsers->get_length()}
        });
    }
    catch (const any& error)
    {
        console->error(std:("Error:"), error);
        return res->status(500)->json(object{
            object::pair{std:("error"), std:("Internal Server Error")}
        });
    }
};


array<object> mockUsers = array<object>{ object{
    object::pair{std:("id"), std:("1")}, 
    object::pair{std:("name"), std:("QuantumTrader")}, 
    object::pair{std:("rank"), 1}, 
    object::pair{std:("index"), 1}, 
    object::pair{std:("score"), 95.5}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 2500000}, 
    object::pair{std:("pnl"), 450000}, 
    object::pair{std:("badge"), std:("Diamond Partner")}
}, object{
    object::pair{std:("id"), std:("2")}, 
    object::pair{std:("name"), std:("AlphaWhale")}, 
    object::pair{std:("rank"), 2}, 
    object::pair{std:("index"), 2}, 
    object::pair{std:("score"), 87.2}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 1800000}, 
    object::pair{std:("pnl"), 320000}, 
    object::pair{std:("badge"), std:("Gold Partner")}
}, object{
    object::pair{std:("id"), std:("3")}, 
    object::pair{std:("name"), std:("DefiMaster")}, 
    object::pair{std:("rank"), 3}, 
    object::pair{std:("index"), 3}, 
    object::pair{std:("score"), 82.7}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 1500000}, 
    object::pair{std:("pnl"), 275000}, 
    object::pair{std:("badge"), std:("Gold Partner")}
}, object{
    object::pair{std:("id"), std:("4")}, 
    object::pair{std:("name"), std:("CryptoSage")}, 
    object::pair{std:("rank"), 4}, 
    object::pair{std:("index"), 4}, 
    object::pair{std:("score"), 79.3}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 1200000}, 
    object::pair{std:("pnl"), 210000}, 
    object::pair{std:("badge"), std:("Silver Partner")}
}, object{
    object::pair{std:("id"), std:("5")}, 
    object::pair{std:("name"), std:("BlockchainBear")}, 
    object::pair{std:("rank"), 5}, 
    object::pair{std:("index"), 5}, 
    object::pair{std:("score"), 75.8}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 950000}, 
    object::pair{std:("pnl"), 180000}, 
    object::pair{std:("badge"), std:("Silver Partner")}
}, object{
    object::pair{std:("id"), std:("6")}, 
    object::pair{std:("name"), std:("YieldFarmer")}, 
    object::pair{std:("rank"), 6}, 
    object::pair{std:("index"), 6}, 
    object::pair{std:("score"), 71.4}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 800000}, 
    object::pair{std:("pnl"), 145000}, 
    object::pair{std:("badge"), std:("Silver Partner")}
}, object{
    object::pair{std:("id"), std:("7")}, 
    object::pair{std:("name"), std:("SmartContractGuru")}, 
    object::pair{std:("rank"), 7}, 
    object::pair{std:("index"), 7}, 
    object::pair{std:("score"), 68.9}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 650000}, 
    object::pair{std:("pnl"), 125000}, 
    object::pair{std:("badge"), std:("Bronze Partner")}
}, object{
    object::pair{std:("id"), std:("8")}, 
    object::pair{std:("name"), std:("DeFiNinja")}, 
    object::pair{std:("rank"), 8}, 
    object::pair{std:("index"), 8}, 
    object::pair{std:("score"), 65.1}, 
    object::pair{std:("avatarUrl"), string_empty}, 
    object::pair{std:("aum"), 550000}, 
    object::pair{std:("pnl"), 95000}, 
    object::pair{std:("badge"), std:("Bronze Partner")}
} };

void Main(void)
{
}

MAIN
