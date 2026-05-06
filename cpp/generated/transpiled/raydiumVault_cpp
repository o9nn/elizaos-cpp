#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/raydium/src/raydiumVault.h"

any depositToRaydiumVault(std::shared_ptr<anchor::AnchorProvider> provider, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> claimer_address)
{
    try
    {
        anchor->setProvider(provider);
        auto vault_config = getVaultConfig(program->programId);
        auto user_position = getUserPosition(program->programId, position_nft);
        auto from_account = spl->getAssociatedTokenAddressSync(position_nft, signerWallet->publicKey);
        auto nft_token_faucet = getNftTokenFaucet(program->programId, position_nft);
        auto accounts = object{
            object::pair{std::string("authority"), signerWallet->publicKey}, 
            object::pair{std::string("vaultConfig"), vault_config}, 
            object::pair{std::string("userPosition"), user_position}, 
            object::pair{std::string("positionNft"), position_nft}, 
            object::pair{std::string("fromAccount"), from_account}, 
            object::pair{std::string("nftTokenFaucet"), nft_token_faucet}, 
            object::pair{std::string("tokenProgram"), spl->TOKEN_PROGRAM_ID}, 
            object::pair{std::string("systemProgram"), anchor->web3->SystemProgram->programId}
        };
        auto call = program->methods->deposit(claimer_address)->accounts(accounts);
        shared txSignature = std::async([=]() { call->rpc(); });
        console->log(std::string("Transaction Signature"), txSignature);
        shared latestBlockhash = std::async([=]() { provider->connection->getLatestBlockhash(); });
        std::async([=]() { retryOperation([=]() mutable
        {
            std::async([=]() { provider->connection->confirmTransaction(object{
                object::pair{std::string("signature"), txSignature}, 
                object::pair{std::string("blockhash"), latestBlockhash->blockhash}, 
                object::pair{std::string("lastValidBlockHeight"), latestBlockhash->lastValidBlockHeight}
            }, std::string("finalized")); });
        }
        , 3, 2000); });
        return txSignature;
    }
    catch (const any& error)
    {
        console->error(std::string("Error in depositRaydiumVault:"), error);
        throw any(error);
    }
};


any changeClaimer(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> new_claimer_address)
{
    auto vault_config = getVaultConfig(program->programId);
    auto user_position = getUserPosition(program->programId, position_nft);
    auto accounts = object{
        object::pair{std::string("authority"), signerWallet->publicKey}, 
        object::pair{std::string("vaultConfig"), vault_config}, 
        object::pair{std::string("userPosition"), user_position}, 
        object::pair{std::string("positionNft"), position_nft}
    };
    auto call = program->methods->changeClaimer(new_claimer_address)->accounts(accounts);
    auto txSignature = std::async([=]() { call->rpc(); });
    console->log(std::string("Transaction Signature"), txSignature);
    std::async([=]() { program->provider->connection->getParsedTransaction(txSignature, object{
        object::pair{std::string("commitment"), std::string("confirmed")}
    }); });
    return txSignature;
};


any emergencyWithdraw(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft)
{
    auto vault_config = getVaultConfig(program->programId);
    auto user_position = getUserPosition(program->programId, position_nft);
    auto to_account = spl->getAssociatedTokenAddressSync(position_nft, signerWallet->publicKey);
    auto nft_token_faucet = getNftTokenFaucet(program->programId, position_nft);
    auto accounts = object{
        object::pair{std::string("authority"), signerWallet->publicKey}, 
        object::pair{std::string("vaultConfig"), vault_config}, 
        object::pair{std::string("userPosition"), user_position}, 
        object::pair{std::string("positionNft"), position_nft}, 
        object::pair{std::string("toAccount"), to_account}, 
        object::pair{std::string("nftTokenFaucet"), nft_token_faucet}, 
        object::pair{std::string("tokenProgram"), spl->TOKEN_PROGRAM_ID}
    };
    auto call = program->methods->emergencyWithdraw()->accounts(accounts);
    auto txSignature = std::async([=]() { call->rpc(); });
    console->log(std::string("Transaction Signature"), txSignature);
    std::async([=]() { program->provider->connection->getParsedTransaction(txSignature, object{
        object::pair{std::string("commitment"), std::string("confirmed")}
    }); });
    return txSignature;
};


any claim(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> poolId, std::shared_ptr<anchor::web3::Connection> connection, std::shared_ptr<anchor::web3::PublicKey> claimer, std::shared_ptr<Token> token)
{
    auto vault_config = getVaultConfig(program->programId);
    auto [locked_authority] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(LOCK_CP_AUTH_SEED) }, LOCKING_PROGRAM);
    auto CPSWAP_AUTH_SEED = Buffer::from(anchor->utils->bytes->utf8->encode(std::string("vault_and_lp_mint_auth_seed")));
    auto user_position = getUserPosition(program->programId, position_nft);
    auto nft_token_faucet = getNftTokenFaucet(program->programId, position_nft);
    auto fee_nft_owner = vault_config;
    auto fee_nft_account = nft_token_faucet;
    auto cpmm_program = CREATE_CPMM_POOL_PROGRAM;
    auto locked_liquidity = getLockedLiquidity(position_nft);
    auto [cp_authority] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ CPSWAP_AUTH_SEED }, cpmm_program);
    auto raydium = std::async([=]() { Raydium->load(object{
        object::pair{std::string("owner"), signerWallet}, 
        object::pair{std::string("connection"), std::string("connection")}, 
        object::pair{std::string("cluster"), std::string("mainnet")}, 
        object::pair{std::string("disableFeatureCheck"), true}, 
        object::pair{std::string("disableLoadToken"), false}, 
        object::pair{std::string("blockhashCommitment"), std::string("confirmed")}
    }); });
    auto poolInfo = as<any>(token->poolInfo);
    if (type_of(poolInfo) == std::string("string")) {
        try
        {
            poolInfo = JSON->parse(poolInfo);
        }
        catch (const any& e)
        {
            console->error(std::string("Failed to parse poolInfo string:"), e);
            throw any(std::make_shared<Error>(std::string("Invalid poolInfo format")));
        }
    }
    if (OR((OR((OR((OR((!poolInfo), (!poolInfo["lpMint"]))), (!poolInfo["lpMint"]["address"]))), (!poolInfo["mintA"]))), (!poolInfo["mintB"]))) {
        poolInfo = const_((std::async([=]() { raydium->api->fetchPoolById(object{
            object::pair{std::string("ids"), poolId->toString()}
        }); })))[0];
        if (!poolInfo) {
            throw any(std::make_shared<Error>(std::string("Pool info not found")));
        }
        auto db = getDB();
        std::async([=]() { db->update(tokens)->set(object{
            object::pair{std::string("poolInfo"), JSON->stringify(poolInfo)}
        })->where(eq(tokens->mint, token->mint)); });
    }
    auto pool_state = std::make_shared<anchor->web3->PublicKey>(poolId->toString());
    if (!poolInfo["lpMint"]["address"]) {
        console->error(std::string("lpMint address not found in poolInfo:"), poolInfo);
        throw any(std::make_shared<Error>(std::string("lpMint address not found in pool info")));
    }
    auto lp_mint = std::make_shared<anchor->web3->PublicKey>(poolInfo["lpMint"]["address"]);
    console->log(std::string("lp_mint"), lp_mint->toString());
    auto vault0_mint = std::make_shared<anchor->web3->PublicKey>(poolInfo["mintA"]["address"]["toString"]());
    auto vault1_mint = std::make_shared<anchor->web3->PublicKey>(poolInfo["mintB"]["address"]["toString"]());
    auto cpmm_pool_key = std::async([=]() { raydium->cpmm->getCpmmPoolKeys(poolId->toString()); });
    auto token0_vault = std::make_shared<anchor->web3->PublicKey>(cpmm_pool_key->vault->A->toString());
    auto token1_vault = std::make_shared<anchor->web3->PublicKey>(cpmm_pool_key->vault->B->toString());
    std::async([=]() { spl->getOrCreateAssociatedTokenAccount(connection, signerWallet, vault0_mint, claimer); });
    std::async([=]() { spl->getOrCreateAssociatedTokenAccount(connection, signerWallet, vault1_mint, claimer); });
    auto recv_token0_account = spl->getAssociatedTokenAddressSync(vault0_mint, claimer, true, spl->TOKEN_PROGRAM_ID);
    auto recv_token1_account = spl->getAssociatedTokenAddressSync(vault1_mint, claimer, true, spl->TOKEN_PROGRAM_ID);
    auto locked_lp_vault = spl->getAssociatedTokenAddressSync(lp_mint, locked_authority, true, spl->TOKEN_PROGRAM_ID);
    auto accounts = object{
        object::pair{std::string("authority"), signerWallet->publicKey}, 
        object::pair{std::string("vaultConfig"), vault_config}, 
        object::pair{std::string("userPosition"), user_position}, 
        object::pair{std::string("lockingProgram"), LOCKING_PROGRAM}, 
        object::pair{std::string("positionNft"), position_nft}, 
        object::pair{std::string("nftTokenFaucet"), nft_token_faucet}, 
        object::pair{std::string("tokenProgram"), spl->TOKEN_PROGRAM_ID}, 
        object::pair{std::string("systemProgram"), anchor->web3->SystemProgram->programId}, 
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
        object::pair{std::string("tokenProgram2022"), spl->TOKEN_2022_PROGRAM_ID}, 
        object::pair{std::string("memoProgram"), raydium_api->MEMO_PROGRAM_ID}
    };
    auto call = program->methods->claim()->accounts(accounts);
    try
    {
        auto modifyComputeUnits = ComputeBudgetProgram->setComputeUnitLimit(object{
            object::pair{std::string("units"), 400000}
        });
        auto addPriorityFee = ComputeBudgetProgram->setComputeUnitPrice(object{
            object::pair{std::string("microLamports"), 5000}
        });
        auto callWithComputeBudget = call->preInstructions(array<any>{ modifyComputeUnits, addPriorityFee });
        std::async([=]() { callWithComputeBudget->simulate(); });
        console->log(std::string("Compute budget instructions:"), object{
            object::pair{std::string("modifyComputeUnits"), std::string("modifyComputeUnits")}, 
            object::pair{std::string("addPriorityFee"), std::string("addPriorityFee")}
        });
        shared txSignature = std::async([=]() { callWithComputeBudget->rpc(); });
        console->log(std::string("Transaction Signature"), txSignature);
        shared latestBlockhash = std::async([=]() { connection->getLatestBlockhash(); });
        std::async([=]() { retryOperation([=]() mutable
        {
            std::async([=]() { connection->confirmTransaction(object{
                object::pair{std::string("signature"), txSignature}, 
                object::pair{std::string("blockhash"), latestBlockhash->blockhash}, 
                object::pair{std::string("lastValidBlockHeight"), latestBlockhash->lastValidBlockHeight}
            }, std::string("confirmed")); });
        }
        , 3, 10000); });
        return txSignature;
    }
    catch (const any& error)
    {
        if (is<anchor->web3->SendTransactionError>(error)) {
            console->error(std::string("Transaction failed with logs:"), error["logs"]);
            throw any(std::make_shared<Error>(std::string("Transaction failed: ") + error["message"] + std::string("\
Logs: ") + JSON->stringify(error["logs"], nullptr, 2) + string_empty));
        }
        console->error(std::string("Error in claim:"), error);
        throw any(error);
    }
};


any checkBalance(std::shared_ptr<anchor::web3::Connection> connection, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> claimer_address)
{
    auto position_nft_account_signer = spl->getAssociatedTokenAddressSync(position_nft, signerWallet->publicKey);
    auto position_nft_account_claimer = spl->getAssociatedTokenAddressSync(position_nft, claimer_address);
    if (OR((!position_nft_account_signer), (!position_nft_account_claimer))) {
        console->log(std::string("NFT account not found for signer or claimer"));
        return 0;
    }
    console->log(std::string("signer balance: "), (std::async([=]() { connection->getTokenAccountBalance(position_nft_account_signer); }))->value->amount);
    auto claimerBalance = std::async([=]() { connection->getTokenAccountBalance(position_nft_account_claimer); });
    console->log(std::string("claimer balance: "), claimerBalance->value->amount);
    if (claimerBalance->value->amount == std::string("0")) {
        console->log(std::string("claimer balance: "), claimerBalance->value->amount);
        return std::shared_ptr<Promise<double>>();
    }
    auto formattedBalance = parseFloat(claimerBalance->value->amount) / Math.pow(10, 9);
    console->log(std::string("Formatted balance: "), formattedBalance);
    return formattedBalance;
};


