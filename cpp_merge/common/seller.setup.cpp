#include "seller.setup.h"

string SELLER_SEED = OR((process->env->SELLER_SEED_PHRASE), (std:("test test test test test test test test test test test junk")));
string PASSWORD = OR((process->env->WALLET_PASSWORD), (std:("Tester@1234")));
object sellerSetup = object{
    object::pair{std:("walletPassword"), PASSWORD}, 
    object::pair{std:("seedPhrase"), SELLER_SEED}
};

void Main(void)
{
}

MAIN
