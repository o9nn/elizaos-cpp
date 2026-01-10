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
        console->log(std::string("Returning ") + paginatedUsers->get_length() + std::string(" mock users for trust scoreboard"));
        return res->status(200)->json(object{
            object::pair{std::string("users"), paginatedUsers}, 
            object::pair{std::string("total"), mockUsers->get_length()}, 
            object::pair{std::string("cursor"), cursor}, 
            object::pair{std::string("hasMore"), endIndex < mockUsers->get_length()}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("Error:"), error);
        return res->status(500)->json(object{
            object::pair{std::string("error"), std::string("Internal Server Error")}
        });
    }
};


array<object> mockUsers = array<object>{ object{
    object::pair{std::string("id"), std::string("1")}, 
    object::pair{std::string("name"), std::string("QuantumTrader")}, 
    object::pair{std::string("rank"), 1}, 
    object::pair{std::string("index"), 1}, 
    object::pair{std::string("score"), 95.5}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 2500000}, 
    object::pair{std::string("pnl"), 450000}, 
    object::pair{std::string("badge"), std::string("Diamond Partner")}
}, object{
    object::pair{std::string("id"), std::string("2")}, 
    object::pair{std::string("name"), std::string("AlphaWhale")}, 
    object::pair{std::string("rank"), 2}, 
    object::pair{std::string("index"), 2}, 
    object::pair{std::string("score"), 87.2}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 1800000}, 
    object::pair{std::string("pnl"), 320000}, 
    object::pair{std::string("badge"), std::string("Gold Partner")}
}, object{
    object::pair{std::string("id"), std::string("3")}, 
    object::pair{std::string("name"), std::string("DefiMaster")}, 
    object::pair{std::string("rank"), 3}, 
    object::pair{std::string("index"), 3}, 
    object::pair{std::string("score"), 82.7}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 1500000}, 
    object::pair{std::string("pnl"), 275000}, 
    object::pair{std::string("badge"), std::string("Gold Partner")}
}, object{
    object::pair{std::string("id"), std::string("4")}, 
    object::pair{std::string("name"), std::string("CryptoSage")}, 
    object::pair{std::string("rank"), 4}, 
    object::pair{std::string("index"), 4}, 
    object::pair{std::string("score"), 79.3}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 1200000}, 
    object::pair{std::string("pnl"), 210000}, 
    object::pair{std::string("badge"), std::string("Silver Partner")}
}, object{
    object::pair{std::string("id"), std::string("5")}, 
    object::pair{std::string("name"), std::string("BlockchainBear")}, 
    object::pair{std::string("rank"), 5}, 
    object::pair{std::string("index"), 5}, 
    object::pair{std::string("score"), 75.8}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 950000}, 
    object::pair{std::string("pnl"), 180000}, 
    object::pair{std::string("badge"), std::string("Silver Partner")}
}, object{
    object::pair{std::string("id"), std::string("6")}, 
    object::pair{std::string("name"), std::string("YieldFarmer")}, 
    object::pair{std::string("rank"), 6}, 
    object::pair{std::string("index"), 6}, 
    object::pair{std::string("score"), 71.4}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 800000}, 
    object::pair{std::string("pnl"), 145000}, 
    object::pair{std::string("badge"), std::string("Silver Partner")}
}, object{
    object::pair{std::string("id"), std::string("7")}, 
    object::pair{std::string("name"), std::string("SmartContractGuru")}, 
    object::pair{std::string("rank"), 7}, 
    object::pair{std::string("index"), 7}, 
    object::pair{std::string("score"), 68.9}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 650000}, 
    object::pair{std::string("pnl"), 125000}, 
    object::pair{std::string("badge"), std::string("Bronze Partner")}
}, object{
    object::pair{std::string("id"), std::string("8")}, 
    object::pair{std::string("name"), std::string("DeFiNinja")}, 
    object::pair{std::string("rank"), 8}, 
    object::pair{std::string("index"), 8}, 
    object::pair{std::string("score"), 65.1}, 
    object::pair{std::string("avatarUrl"), string_empty}, 
    object::pair{std::string("aum"), 550000}, 
    object::pair{std::string("pnl"), 95000}, 
    object::pair{std::string("badge"), std::string("Bronze Partner")}
} };

void Main(void)
{
}

MAIN
