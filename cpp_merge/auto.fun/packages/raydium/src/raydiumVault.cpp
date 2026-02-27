#include "raydiumVault.hpp"

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
            object::pair{std:("authority"), signerWallet->publicKey}, 
            object::pair{std:("vaultConfig"), vault_config}, 
            object::pair{std:("userPosition"), user_position}, 
            object::pair{std:("positionNft"), position_nft}, 
            object::pair{std:("fromAccount"), from_account}, 
            object::pair{std:("nftTokenFaucet"), nft_token_faucet}, 
            object::pair{std:("tokenProgram"), spl->TOKEN_PROGRAM_ID}, 
            object::pair{std:("systemProgram"), anchor->web3->SystemProgram->programId}
        };
        auto call = program->methods->deposit(claimer_address)->accounts(accounts);
        shared txSignature = std::async([=]() { call->rpc(); });
        console->log(std:("Transaction Signature"), txSignature);
        shared latestBlockhash = std::async([=]() { provider->connection->getLatestBlockhash(); });
        std::async([=]() { retryOperation([=]() mutable
        {
            std::async([=]() { provider->connection->confirmTransaction(object{
                object::pair{std:("signature"), txSignature}, 
                object::pair{std:("blockhash"), latestBlockhash->blockhash}, 
                object::pair{std:("lastValidBlockHeight"), latestBlockhash->lastValidBlockHeight}
            }, std:("finalized")); });
        }
        , 3, 2000); });
        return txSignature;
    }
    catch (const any& error)
    {
        console->error(std:("Error in depositRaydiumVault:"), error);
        throw any(error);
    }
};


any changeClaimer(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> new_claimer_address)
{
    auto vault_config = getVaultConfig(program->programId);
    auto user_position = getUserPosition(program->programId, position_nft);
    auto accounts = object{
        object::pair{std:("authority"), signerWallet->publicKey}, 
        object::pair{std:("vaultConfig"), vault_config}, 
        object::pair{std:("userPosition"), user_position}, 
        object::pair{std:("positionNft"), position_nft}
    };
    auto call = program->methods->changeClaimer(new_claimer_address)->accounts(accounts);
    auto txSignature = std::async([=]() { call->rpc(); });
    console->log(std:("Transaction Signature"), txSignature);
    std::async([=]() { program->provider->connection->getParsedTransaction(txSignature, object{
        object::pair{std:("commitment"), std:("confirmed")}
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
        object::pair{std:("authority"), signerWallet->publicKey}, 
        object::pair{std:("vaultConfig"), vault_config}, 
        object::pair{std:("userPosition"), user_position}, 
        object::pair{std:("positionNft"), position_nft}, 
        object::pair{std:("toAccount"), to_account}, 
        object::pair{std:("nftTokenFaucet"), nft_token_faucet}, 
        object::pair{std:("tokenProgram"), spl->TOKEN_PROGRAM_ID}
    };
    auto call = program->methods->emergencyWithdraw()->accounts(accounts);
    auto txSignature = std::async([=]() { call->rpc(); });
    console->log(std:("Transaction Signature"), txSignature);
    std::async([=]() { program->provider->connection->getParsedTransaction(txSignature, object{
        object::pair{std:("commitment"), std:("confirmed")}
    }); });
    return txSignature;
};


any claim(std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>> program, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> poolId, std::shared_ptr<anchor::web3::Connection> connection, std::shared_ptr<anchor::web3::PublicKey> claimer, std::shared_ptr<Token> token)
{
    auto vault_config = getVaultConfig(program->programId);
    auto [locked_authority] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(LOCK_CP_AUTH_SEED) }, LOCKING_PROGRAM);
    auto CPSWAP_AUTH_SEED = Buffer::from(anchor->utils->bytes->utf8->encode(std:("vault_and_lp_mint_auth_seed")));
    auto user_position = getUserPosition(program->programId, position_nft);
    auto nft_token_faucet = getNftTokenFaucet(program->programId, position_nft);
    auto fee_nft_owner = vault_config;
    auto fee_nft_account = nft_token_faucet;
    auto cpmm_program = CREATE_CPMM_POOL_PROGRAM;
    auto locked_liquidity = getLockedLiquidity(position_nft);
    auto [cp_authority] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ CPSWAP_AUTH_SEED }, cpmm_program);
    auto raydium = std::async([=]() { Raydium->load(object{
        object::pair{std:("owner"), signerWallet}, 
        object::pair{std:("connection"), std:("connection")}, 
        object::pair{std:("cluster"), std:("mainnet")}, 
        object::pair{std:("disableFeatureCheck"), true}, 
        object::pair{std:("disableLoadToken"), false}, 
        object::pair{std:("blockhashCommitment"), std:("confirmed")}
    }); });
    auto poolInfo = as<any>(token->poolInfo);
    if (type_of(poolInfo) == std:("string")) {
        try
        {
            poolInfo = JSON->parse(poolInfo);
        }
        catch (const any& e)
        {
            console->error(std:("Failed to parse poolInfo string:"), e);
            throw any(std::make_shared<Error>(std:("Invalid poolInfo format")));
        }
    }
    if (OR((OR((OR((OR((!poolInfo), (!poolInfo["lpMint"]))), (!poolInfo["lpMint"]["address"]))), (!poolInfo["mintA"]))), (!poolInfo["mintB"]))) {
        poolInfo = const_((std::async([=]() { raydium->api->fetchPoolById(object{
            object::pair{std:("ids"), poolId->toString()}
        }); })))[0];
        if (!poolInfo) {
            throw any(std::make_shared<Error>(std:("Pool info not found")));
        }
        auto db = getDB();
        std::async([=]() { db->update(tokens)->set(object{
            object::pair{std:("poolInfo"), JSON->stringify(poolInfo)}
        })->where(eq(tokens->mint, token->mint)); });
    }
    auto pool_state = std::make_shared<anchor->web3->PublicKey>(poolId->toString());
    if (!poolInfo["lpMint"]["address"]) {
        console->error(std:("lpMint address not found in poolInfo:"), poolInfo);
        throw any(std::make_shared<Error>(std:("lpMint address not found in pool info")));
    }
    auto lp_mint = std::make_shared<anchor->web3->PublicKey>(poolInfo["lpMint"]["address"]);
    console->log(std:("lp_mint"), lp_mint->toString());
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
        object::pair{std:("authority"), signerWallet->publicKey}, 
        object::pair{std:("vaultConfig"), vault_config}, 
        object::pair{std:("userPosition"), user_position}, 
        object::pair{std:("lockingProgram"), LOCKING_PROGRAM}, 
        object::pair{std:("positionNft"), position_nft}, 
        object::pair{std:("nftTokenFaucet"), nft_token_faucet}, 
        object::pair{std:("tokenProgram"), spl->TOKEN_PROGRAM_ID}, 
        object::pair{std:("systemProgram"), anchor->web3->SystemProgram->programId}, 
        object::pair{std:("lockedAuthority"), locked_authority}, 
        object::pair{std:("feeNftOwner"), fee_nft_owner}, 
        object::pair{std:("feeNftAccount"), fee_nft_account}, 
        object::pair{std:("lockedLiquidity"), locked_liquidity}, 
        object::pair{std:("cpmmProgram"), cpmm_program}, 
        object::pair{std:("cpAuthority"), cp_authority}, 
        object::pair{std:("poolState"), pool_state}, 
        object::pair{std:("lpMint"), lp_mint}, 
        object::pair{std:("recipientToken0Account"), recv_token0_account}, 
        object::pair{std:("recipientToken1Account"), recv_token1_account}, 
        object::pair{std:("token0Vault"), token0_vault}, 
        object::pair{std:("token1Vault"), token1_vault}, 
        object::pair{std:("vault0Mint"), vault0_mint}, 
        object::pair{std:("vault1Mint"), vault1_mint}, 
        object::pair{std:("lockedLpVault"), locked_lp_vault}, 
        object::pair{std:("tokenProgram2022"), spl->TOKEN_2022_PROGRAM_ID}, 
        object::pair{std:("memoProgram"), raydium_api->MEMO_PROGRAM_ID}
    };
    auto call = program->methods->claim()->accounts(accounts);
    try
    {
        auto modifyComputeUnits = ComputeBudgetProgram->setComputeUnitLimit(object{
            object::pair{std:("units"), 400000}
        });
        auto addPriorityFee = ComputeBudgetProgram->setComputeUnitPrice(object{
            object::pair{std:("microLamports"), 5000}
        });
        auto callWithComputeBudget = call->preInstructions(array<any>{ modifyComputeUnits, addPriorityFee });
        std::async([=]() { callWithComputeBudget->simulate(); });
        console->log(std:("Compute budget instructions:"), object{
            object::pair{std:("modifyComputeUnits"), std:("modifyComputeUnits")}, 
            object::pair{std:("addPriorityFee"), std:("addPriorityFee")}
        });
        shared txSignature = std::async([=]() { callWithComputeBudget->rpc(); });
        console->log(std:("Transaction Signature"), txSignature);
        shared latestBlockhash = std::async([=]() { connection->getLatestBlockhash(); });
        std::async([=]() { retryOperation([=]() mutable
        {
            std::async([=]() { connection->confirmTransaction(object{
                object::pair{std:("signature"), txSignature}, 
                object::pair{std:("blockhash"), latestBlockhash->blockhash}, 
                object::pair{std:("lastValidBlockHeight"), latestBlockhash->lastValidBlockHeight}
            }, std:("confirmed")); });
        }
        , 3, 10000); });
        return txSignature;
    }
    catch (const any& error)
    {
        if (is<anchor->web3->SendTransactionError>(error)) {
            console->error(std:("Transaction failed with logs:"), error["logs"]);
            throw any(std::make_shared<Error>(std:("Transaction failed: ") + error["message"] + std:("\
Logs: ") + JSON->stringify(error["logs"], nullptr, 2) + string_empty));
        }
        console->error(std:("Error in claim:"), error);
        throw any(error);
    }
};


any checkBalance(std::shared_ptr<anchor::web3::Connection> connection, std::shared_ptr<anchor::web3::Keypair> signerWallet, std::shared_ptr<anchor::web3::PublicKey> position_nft, std::shared_ptr<anchor::web3::PublicKey> claimer_address)
{
    auto position_nft_account_signer = spl->getAssociatedTokenAddressSync(position_nft, signerWallet->publicKey);
    auto position_nft_account_claimer = spl->getAssociatedTokenAddressSync(position_nft, claimer_address);
    if (OR((!position_nft_account_signer), (!position_nft_account_claimer))) {
        console->log(std:("NFT account not found for signer or claimer"));
        return 0;
    }
    console->log(std:("signer balance: "), (std::async([=]() { connection->getTokenAccountBalance(position_nft_account_signer); }))->value->amount);
    auto claimerBalance = std::async([=]() { connection->getTokenAccountBalance(position_nft_account_claimer); });
    console->log(std:("claimer balance: "), claimerBalance->value->amount);
    if (claimerBalance->value->amount == std:("0")) {
        console->log(std:("claimer balance: "), claimerBalance->value->amount);
        return std::shared_ptr<Promise<double>>();
    }
    auto formattedBalance = parseFloat(claimerBalance->value->amount) / Math.pow(10, 9);
    console->log(std:("Formatted balance: "), formattedBalance);
    return formattedBalance;
};


