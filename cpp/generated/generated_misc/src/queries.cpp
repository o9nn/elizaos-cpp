#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/walletLinking/queries.h"

std::shared_ptr<Promise<any>> getUserWalletData(string username)
{
    auto userWallets = std::async([=]() { db->query->walletAddresses->findMany(object{
        object::pair{std::string("where"), and(eq(walletAddresses->userId, username), eq(walletAddresses->isActive, true))}, 
        object::pair{std::string("columns"), object{
            object::pair{std::string("chainId"), true}, 
            object::pair{std::string("accountAddress"), true}, 
            object::pair{std::string("updatedAt"), true}
        }}
    }); });
    if (userWallets->length > 0) {
        auto wallets = userWallets->map([=](auto wallet) mutable
        {
            return (object{
                object::pair{std::string("chain"), getChainByChainId(wallet["chainId"])}, 
                object::pair{std::string("address"), wallet["accountAddress"]}
            });
        }
        );
        auto lastUpdated = userWallets->reduce([=](auto latest, auto wallet) mutable
        {
            auto walletDate = std::make_shared<Date>(wallet["updatedAt"]);
            return (walletDate > latest) ? any(walletDate) : any(latest);
        }
        , std::make_shared<Date>(0));
        return object{
            object::pair{std::string("wallets"), std::string("wallets")}, 
            object::pair{std::string("lastUpdated"), lastUpdated->toISOString()}
        };
    }
    return nullptr;
};


