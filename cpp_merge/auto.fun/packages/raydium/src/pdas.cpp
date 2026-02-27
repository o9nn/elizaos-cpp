#include "pdas.hpp"

std::shared_ptr<anchor::web3::PublicKey> getVaultConfig(std::shared_ptr<anchor::web3::PublicKey> programId)
{
    return const_(anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(VAULT_CONFIG_SEED) }, programId))[0];
};


std::shared_ptr<anchor::web3::PublicKey> getUserPosition(std::shared_ptr<anchor::web3::PublicKey> programId, std::shared_ptr<anchor::web3::PublicKey> positionNft)
{
    return const_(anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(POSITION_SEED), positionNft->toBuffer() }, programId))[0];
};


std::shared_ptr<anchor::web3::PublicKey> getNftTokenFaucet(std::shared_ptr<anchor::web3::PublicKey> programId, std::shared_ptr<anchor::web3::PublicKey> positionNft)
{
    return const_(anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(NFT_FAUCET_SEED), positionNft->toBuffer() }, programId))[0];
};


std::shared_ptr<anchor::web3::PublicKey> getLockedLiquidity(std::shared_ptr<anchor::web3::PublicKey> positionNft, std::shared_ptr<anchor::web3::PublicKey> lockingProgram)
{
    return const_(anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(LOCKED_CP_LIQUIDITY_SEED), positionNft->toBuffer() }, lockingProgram))[0];
};


any LOCKING_PROGRAM = std::make_shared<anchor->web3->PublicKey>(std:("LockrWmn6K5twhz3y9w1dQERbmgSaRkfnTeTKbpofwE"));
string LOCK_CP_AUTH_SEED = std:("lock_cp_authority_seed");
string VAULT_CONFIG_SEED = std:("raydium_vault_config");
string POSITION_SEED = std:("raydium_position");
string NFT_FAUCET_SEED = std:("raydium_vault_nft_seed");
string LOCKED_CP_LIQUIDITY_SEED = std:("locked_liquidity");

void Main(void)
{
}

MAIN
