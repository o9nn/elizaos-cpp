#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/solana/otc-program/tests/otc.pool-price.h"

void Main(void)
{
    describe(std::string("OTC Pool Price Tests"), [=]() mutable
    {
        shared provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        shared program = as<std::shared_ptr<Program<std::shared_ptr<Otc>>>>(anchor->workspace->Otc);
        shared<std::shared_ptr<Keypair>> desk;
        shared<std::shared_ptr<Keypair>> owner;
        shared<std::shared_ptr<Keypair>> payer;
        shared<std::shared_ptr<PublicKey>> tokenMint;
        shared<std::shared_ptr<PublicKey>> quoteMint;
        shared<std::shared_ptr<PublicKey>> poolAddress;
        shared<std::shared_ptr<PublicKey>> vaultA;
        shared<std::shared_ptr<PublicKey>> vaultB;
        shared<std::shared_ptr<PublicKey>> tokenRegistry;
        before([=]() mutable
        {
            desk = Keypair->generate();
            owner = Keypair->generate();
            payer = Keypair->generate();
            std::async([=]() { provider->connection->requestAirdrop(owner->publicKey, 10 * LAMPORTS_PER_SOL); });
            std::async([=]() { provider->connection->requestAirdrop(payer->publicKey, 10 * LAMPORTS_PER_SOL); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 1000);
            }
            ); });
            tokenMint = std::async([=]() { createMint(provider->connection, owner, owner->publicKey, nullptr, 9); });
            quoteMint = std::async([=]() { createMint(provider->connection, owner, owner->publicKey, nullptr, 6); });
            std::async([=]() { program->methods->initDesk(std::make_shared<anchor->BN>(5 * 100000000), std::make_shared<anchor->BN>(1800))->accounts(object{
                object::pair{std::string("payer"), owner->publicKey}, 
                object::pair{std::string("owner"), owner->publicKey}, 
                object::pair{std::string("agent"), owner->publicKey}, 
                object::pair{std::string("usdcMint"), quoteMint}, 
                object::pair{std::string("desk"), desk->publicKey}
            })->signers(array<any>{ owner, desk })->rpc(); });
            poolAddress = Keypair->generate()->publicKey;
        }
        );
        it(std::string("registers token permissionlessly with pool"), [=]() mutable
        {
            auto [registryPda] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("registry")), desk->publicKey->toBuffer(), tokenMint->toBuffer() }, program->programId);
            tokenRegistry = registryPda;
            auto priceFeedId = Buffer::alloc(32)->fill(1);
            std::async([=]() { program->methods->registerToken(array<any>{ priceFeedId }, poolAddress, 1)->accounts(object{
                object::pair{std::string("desk"), desk->publicKey}, 
                object::pair{std::string("payer"), payer->publicKey}, 
                object::pair{std::string("tokenMint"), tokenMint}
            })->signers(array<any>{ payer })->rpc(); });
            auto regAccount = std::async([=]() { program->account->tokenRegistry->fetch(registryPda); });
            assert->equal(regAccount->desk->toBase58(), desk->publicKey->toBase58());
            assert->equal(regAccount->tokenMint->toBase58(), tokenMint->toBase58());
            assert->equal(regAccount->poolAddress->toBase58(), poolAddress->toBase58());
            assert->equal(regAccount->registeredBy->toBase58(), payer->publicKey->toBase58());
        }
        );
        it(std::string("updates price from pool"), [=]() mutable
        {
            vaultA = std::async([=]() { createAccount(provider->connection, payer, tokenMint, poolAddress, Keypair->generate()); });
            vaultB = std::async([=]() { createAccount(provider->connection, payer, quoteMint, poolAddress, Keypair->generate()); });
            std::async([=]() { mintTo(provider->connection, payer, tokenMint, vaultA, owner, 100 * Math.pow(10, 9)); });
            std::async([=]() { mintTo(provider->connection, payer, quoteMint, vaultB, owner, 200 * Math.pow(10, 6)); });
            std::async([=]() { program->methods->updateTokenPriceFromPool()->accounts(object{
                object::pair{std::string("tokenRegistry"), tokenRegistry}, 
                object::pair{std::string("pool"), poolAddress}, 
                object::pair{std::string("vaultA"), vaultA}, 
                object::pair{std::string("vaultB"), vaultB}
            })->rpc(); });
            auto regAccount = std::async([=]() { program->account->tokenRegistry->fetch(tokenRegistry); });
            auto price = regAccount->tokenUsdPrice8d->toNumber();
            assert->equal(price, 200000000);
            assert->ok(regAccount->pricesUpdatedAt->toNumber() > 0);
        }
        );
    }
    );
}

MAIN
