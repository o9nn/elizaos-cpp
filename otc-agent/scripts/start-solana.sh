#!/bin/bash
# Start Solana test validator and deploy program
set -e

cd "$(dirname "$0")/.."

# Check if already running
if lsof -i :8899 >/dev/null 2>&1; then
    echo "Using existing Solana validator at localhost:8899"
    tail -f /dev/null
    exit 0
fi

echo "Starting Solana test validator..."
cd solana/otc-program

# Kill any existing validator
pkill -9 -f solana-test-validator 2>/dev/null || true
sleep 1
rm -rf test-ledger 2>/dev/null || true

# Start validator in background
solana-test-validator --reset --log > ../../solana.log 2>&1 &
sleep 8

# Deploy program
if [ -f target/deploy/otc.so ]; then
    echo "Deploying Solana program..."
    solana program deploy target/deploy/otc.so \
        --url http://127.0.0.1:8899 \
        --program-id target/deploy/otc-keypair.json \
        --upgrade-authority target/deploy/otc-keypair.json 2>/dev/null || true
    
    # Airdrop SOL
    solana airdrop 25 target/deploy/otc-keypair.json --url http://127.0.0.1:8899 2>/dev/null || true
    solana airdrop 2 DScqtGwFoDTme2Rzdjpdb2w7CtuKc6Z8KF7hMhbx8ugQ --url http://127.0.0.1:8899 2>/dev/null || true
    
    # Initialize desk
    echo "Initializing Solana desk..."
    ANCHOR_PROVIDER_URL=http://127.0.0.1:8899 ANCHOR_WALLET=./id.json bun scripts/quick-init.ts 2>/dev/null || true
fi

cd ../..
tail -f /dev/null

