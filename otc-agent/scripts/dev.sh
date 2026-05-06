#!/bin/bash
# Development server - starts everything needed for local dev
set -e

cd "$(dirname "$0")/.."

echo "Starting development environment..."

# Ensure dependencies
./scripts/ensure-postgres.sh
./scripts/ensure-deployments.sh

# Cleanup old processes
lsof -t -i:4444 | xargs kill -9 2>/dev/null || true

# Check if using production Solana (mainnet)
USE_LOCAL_SOLANA=true
if grep -q "mainnet-beta" .env.local 2>/dev/null; then
    echo "Using production Solana - skipping local validator"
    USE_LOCAL_SOLANA=false
fi

# Start services
if [ "$USE_LOCAL_SOLANA" = true ]; then
    concurrently --kill-others-on-fail \
        --names "anvil,solana,prices,next,seed" \
        "./scripts/start-rpc.sh" \
        "./scripts/start-solana.sh" \
        "sleep 15 && ./scripts/keep-prices-fresh.sh" \
        "sleep 8 && next dev -p 4444" \
        "sleep 20 && bun run seed && tail -f /dev/null"
else
    concurrently --kill-others-on-fail \
        --names "anvil,prices,next,seed" \
        "./scripts/start-rpc.sh" \
        "sleep 15 && ./scripts/keep-prices-fresh.sh" \
        "sleep 8 && next dev -p 4444" \
        "sleep 20 && bun run seed && tail -f /dev/null"
fi

