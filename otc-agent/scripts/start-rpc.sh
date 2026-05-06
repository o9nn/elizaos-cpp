#!/bin/bash
# Start or connect to EVM RPC (Anvil)
set -e

cd "$(dirname "$0")/.."

# Check if already running
if lsof -i :8545 >/dev/null 2>&1; then
    echo "Using existing RPC at localhost:8545"
    tail -f /dev/null
else
    echo "Starting Anvil..."
    ./scripts/start-anvil.sh &
    sleep 3
    
    echo "Deploying contracts..."
    cd contracts && forge script scripts/DeployElizaOTC.s.sol --broadcast --rpc-url http://127.0.0.1:8545 >/dev/null 2>&1 || bun run deploy:eliza
    cd ..
    
    tail -f /dev/null
fi

