#!/bin/bash
# Run all tests - ensures infrastructure is up, then runs everything
set -e

cd "$(dirname "$0")/.."

echo "Running all tests..."
echo ""

# ============================================
# 1. Contract Tests (no infrastructure needed)
# ============================================
echo "=== Contract Tests ==="
cd contracts && forge test --summary
cd ..
echo ""

# ============================================
# 2. Ensure infrastructure for integration tests
# ============================================
echo "=== Starting Infrastructure ==="

# PostgreSQL
./scripts/ensure-postgres.sh

# Kill any existing Next.js to ensure fresh env
pkill -f "next dev" 2>/dev/null || true

# Anvil (EVM)
NEED_DEPLOY=false
if ! lsof -i :8545 >/dev/null 2>&1; then
    echo "Starting Anvil..."
    ./scripts/start-anvil.sh &
    
    # Wait for Anvil
    echo "Waiting for Anvil..."
    for i in {1..15}; do
        if lsof -i :8545 >/dev/null 2>&1; then
            echo "Anvil is ready"
            break
        fi
        sleep 1
    done
    sleep 2
    NEED_DEPLOY=true
fi

# Always deploy fresh contracts to ensure test wallet has funds
echo "Deploying contracts..."
cd contracts && forge script scripts/DeployElizaOTC.s.sol --broadcast --rpc-url http://127.0.0.1:8545 2>&1 | grep -E "(OTC|Token|Test|Deployed)" || true
cd ..

# Sync deployment addresses
DEPLOYMENT_FILE="contracts/deployments/eliza-otc-deployment.json"
if [ -f "$DEPLOYMENT_FILE" ]; then
    echo "Syncing deployment addresses..."
    
    OTC_ADDR=$(jq -r '.contracts.deal' "$DEPLOYMENT_FILE" 2>/dev/null)
    TOKEN_ADDR=$(jq -r '.contracts.elizaToken' "$DEPLOYMENT_FILE" 2>/dev/null)
    USDC_ADDR=$(jq -r '.contracts.usdcMock' "$DEPLOYMENT_FILE" 2>/dev/null)
    TEST_WALLET=$(jq -r '.accounts.testWallet' "$DEPLOYMENT_FILE" 2>/dev/null)
    
    echo "  OTC: $OTC_ADDR"
    echo "  Token: $TOKEN_ADDR"
    echo "  Test Wallet: $TEST_WALLET"
    
    # Update .env.local with new addresses
    if [ -n "$OTC_ADDR" ] && [ "$OTC_ADDR" != "null" ]; then
        if grep -q "NEXT_PUBLIC_OTC_ADDRESS" .env.local 2>/dev/null; then
            sed -i '' "s|NEXT_PUBLIC_OTC_ADDRESS=.*|NEXT_PUBLIC_OTC_ADDRESS=$OTC_ADDR|" .env.local
        else
            echo "NEXT_PUBLIC_OTC_ADDRESS=$OTC_ADDR" >> .env.local
        fi
    fi
    
    # Fund test wallet
    OWNER_KEY="0xac0974bec39a17e36ba4a6b4d238ff944bacb478cbed5efcae784d7bf4f2ff80"
    if [ -n "$TEST_WALLET" ] && [ "$TEST_WALLET" != "null" ]; then
        echo "Funding test wallet..."
        cast send "$TEST_WALLET" --value 100ether --private-key "$OWNER_KEY" --rpc-url http://127.0.0.1:8545 >/dev/null 2>&1 || true
        [ -n "$TOKEN_ADDR" ] && [ "$TOKEN_ADDR" != "null" ] && \
            cast send "$TOKEN_ADDR" "transfer(address,uint256)" "$TEST_WALLET" 1000000000000000000000000 \
            --private-key "$OWNER_KEY" --rpc-url http://127.0.0.1:8545 >/dev/null 2>&1 || true
    fi
fi

# Start Next.js with fresh env
echo "Starting Next.js..."
bun run next dev -p 4444 >/dev/null 2>&1 &

# Wait for server to be ready
echo "Waiting for Next.js to start..."
for i in {1..30}; do
    if curl -s http://localhost:4444 >/dev/null 2>&1; then
        echo "Next.js is ready"
        break
    fi
    sleep 2
done

echo ""

# ============================================
# 3. Integration Tests (Vitest)
# ============================================
echo "=== Integration Tests ==="
bun run vitest run tests/otc-e2e.test.ts
echo ""

# ============================================
# 4. E2E UI Tests (Playwright)
# ============================================
echo "=== E2E UI Tests ==="
npx playwright test tests/ui.spec.ts
echo ""

echo "=== All Tests Complete ==="

