#include "otc-agent/tests/wallet-setup/buyer.setup.h"

std::string BUYER_SEED = OR((process->env->BUYER_SEED_PHRASE), (std::string("test test test test test test test test test test test junk")));
std::string PASSWORD = OR((process->env->WALLET_PASSWORD), (std::string("Tester@1234")));
object buyerSetup = object{
    object::pair{std::string("walletPassword"), PASSWORD}, 
    object::pair{std::string("seedPhrase"), BUYER_SEED}
};

void Main(void)
{
}

MAIN
