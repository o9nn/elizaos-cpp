#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/claim.h"

void Main(void)
{
    describe(std::string("raydium_vault"), [=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        shared connection = provider->connection;
        auto nodeWallet = as<std::shared_ptr<NodeWallet>>(provider->wallet);
        shared signerWallet = anchor->web3->Keypair->fromSecretKey(nodeWallet->payer->secretKey);
        shared program = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        it(std::string("Claim Rewards"), [=]() mutable
        {
            auto isDev = isDevnet(connection);
            auto position_nft = getNftAddress(isDev);
            auto locking_program = std::make_shared<anchor->web3->PublicKey>(std::string("LockrWmn6K5twhz3y9w1dQERbmgSaRkfnTeTKbpofwE"));
            auto LOCK_CP_AUTH_SEED = std::string("lock_cp_authority_seed");
            auto LOCKED_CP_LIQUIDITY_SEED = std::string("locked_liquidity");
            auto CPSWAP_AUTH_SEED = Buffer::from(anchor->utils->bytes->utf8->encode(std::string("vault_and_lp_mint_auth_seed")));
            auto [locked_authority] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(LOCK_CP_AUTH_SEED) }, locking_program);
            auto [nft_token_faucet] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(nftFaucetSeed), position_nft["toBuffer"]() }, program->programId);
            auto [vault_config] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            auto fee_nft_owner = vault_config;
            auto [locked_liquidity] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(LOCKED_CP_LIQUIDITY_SEED), position_nft["toBuffer"]() }, locking_program);
            auto fee_nft_account = nft_token_faucet;
            auto cpmm_program = CREATE_CPMM_POOL_PROGRAM;
            auto [cp_authority] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ CPSWAP_AUTH_SEED }, cpmm_program);
            auto raydium = std::async([=]() { Raydium->load(object{
                object::pair{std::string("owner"), signerWallet}, 
                object::pair{std::string("connection"), std::string("connection")}, 
                object::pair{std::string("cluster"), std::string("mainnet")}, 
                object::pair{std::string("disableFeatureCheck"), true}, 
                object::pair{std::string("disableLoadToken"), false}, 
                object::pair{std::string("blockhashCommitment"), std::string("finalized")}
            }); });
            auto poolId = std::make_shared<anchor->web3->PublicKey>(std::string("CExPZUmwAdu6dDZFGQmkxM8UFu1zVkGwk3tDVHWMifR9"));
            auto poolInfo = const_((std::async([=]() { raydium->api->fetchPoolById(object{
                object::pair{std::string("ids"), poolId->toString()}
            }); })))[0];
            auto poolInfoJson = JSON->parse(JSON->stringify(poolInfo));
            auto pool_state = std::make_shared<anchor->web3->PublicKey>(poolId->toString());
            auto lp_mint = std::make_shared<anchor->web3->PublicKey>(poolInfoJson["lpMint"]["address"]);
            auto vault0_mint = std::make_shared<anchor->web3->PublicKey>(poolInfo->mintA->address->toString());
            auto vault1_mint = std::make_shared<anchor->web3->PublicKey>(poolInfo->mintB->address->toString());
            auto cpmm_pool_key = std::async([=]() { raydium->cpmm->getCpmmPoolKeys(poolId->toString()); });
            auto token0_vault = std::make_shared<anchor->web3->PublicKey>(cpmm_pool_key->vault->A->toString());
            auto token1_vault = std::make_shared<anchor->web3->PublicKey>(cpmm_pool_key->vault->B->toString());
            std::async([=]() { spl->getOrCreateAssociatedTokenAccount(connection, signerWallet, vault0_mint, signerWallet->publicKey); });
            std::async([=]() { spl->getOrCreateAssociatedTokenAccount(connection, signerWallet, vault1_mint, signerWallet->publicKey); });
            auto recv_token0_account = spl->getAssociatedTokenAddressSync(vault0_mint, signerWallet->publicKey, true, spl->TOKEN_PROGRAM_ID);
            auto recv_token1_account = spl->getAssociatedTokenAddressSync(vault1_mint, signerWallet->publicKey, true, spl->TOKEN_PROGRAM_ID);
            auto locked_lp_vault = spl->getAssociatedTokenAddressSync(lp_mint, locked_authority, true, spl->TOKEN_PROGRAM_ID);
            auto [user_position] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(positionSeed), position_nft["toBuffer"]() }, program->programId);
            std::async([=]() { program->rpc->claim(object{
                object::pair{std::string("accounts"), object{
                    object::pair{std::string("authority"), signerWallet->publicKey}, 
                    object::pair{std::string("vaultConfig"), vault_config}, 
                    object::pair{std::string("userPosition"), user_position}, 
                    object::pair{std::string("lockingProgram"), locking_program}, 
                    object::pair{std::string("lockedAuthority"), locked_authority}, 
                    object::pair{std::string("feeNftOwner"), fee_nft_owner}, 
                    object::pair{std::string("feeNftAccount"), fee_nft_account}, 
                    object::pair{std::string("lockedLiquidity"), locked_liquidity}, 
                    object::pair{std::string("cpmmProgram"), cpmm_program}, 
                    object::pair{std::string("cpAuthority"), cp_authority}, 
                    object::pair{std::string("poolState"), pool_state}, 
                    object::pair{std::string("lpMint"), lp_mint}, 
                    object::pair{std::string("recipientToken0Account"), recv_token0_account}, 
                    object::pair{std::string("recipientToken1Account"), recv_token1_account}, 
                    object::pair{std::string("token0Vault"), token0_vault}, 
                    object::pair{std::string("token1Vault"), token1_vault}, 
                    object::pair{std::string("vault0Mint"), vault0_mint}, 
                    object::pair{std::string("vault1Mint"), vault1_mint}, 
                    object::pair{std::string("lockedLpVault"), locked_lp_vault}, 
                    object::pair{std::string("tokenProgram"), spl->TOKEN_PROGRAM_ID}, 
                    object::pair{std::string("tokenProgram2022"), spl->TOKEN_2022_PROGRAM_ID}, 
                    object::pair{std::string("memoProgram"), raydium_api->MEMO_PROGRAM_ID}
                }}
            }); });
        }
        );
    }
    );
}

MAIN
