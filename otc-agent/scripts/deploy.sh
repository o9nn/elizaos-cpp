#!/bin/bash
# Deploy contracts to testnet or mainnet
set -e

cd "$(dirname "$0")/.."

ENV="${1:-testnet}"

if [ "$ENV" != "testnet" ] && [ "$ENV" != "mainnet" ]; then
    echo "Usage: ./scripts/deploy.sh [testnet|mainnet]"
    exit 1
fi

echo "Deploying to $ENV..."

# Load env vars
export $(grep -v '^#' .env.local | xargs)

if [ "$ENV" = "testnet" ]; then
    # EVM (Base Sepolia)
    echo "Deploying EVM contracts to Base Sepolia..."
    cd contracts
    forge script scripts/DeployOTCSepolia.s.sol:DeployOTCSepolia --rpc-url "$BASE_SEPOLIA_RPC" --broadcast --verify
    cp deployments/testnet-evm.json ../src/config/deployments/testnet-evm.json
    cd ..
    
    # Solana (Devnet)
    echo "Deploying Solana program to Devnet..."
    cd solana/otc-program
    ANCHOR_PROVIDER_URL=https://api.devnet.solana.com ANCHOR_WALLET=./id.json bun scripts/deploy-devnet.ts
    cd ../..
else
    # EVM (Base Mainnet)
    echo "Deploying EVM contracts to Base Mainnet..."
    cd contracts
    forge script scripts/DeployOTCMainnet.s.sol:DeployOTCMainnet --rpc-url "$BASE_RPC" --broadcast --verify
    cp deployments/mainnet-evm.json ../src/config/deployments/mainnet-evm.json
    cd ..
    
    # Solana (Mainnet)
    echo "Deploying Solana program to Mainnet..."
    cd solana/otc-program
    ANCHOR_PROVIDER_URL=https://api.mainnet-beta.solana.com ANCHOR_WALLET=./id.json bun scripts/deploy-mainnet.ts
    cd ../..
fi

echo "Deployment complete."

