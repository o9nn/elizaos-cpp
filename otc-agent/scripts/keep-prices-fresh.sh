#!/bin/bash
# Keep price feeds fresh by periodically hitting the market-data API
# This prevents stale price errors during development

API_URL="${NEXT_PUBLIC_APP_URL:-http://localhost:4444}"
INTERVAL=60  # Refresh every 60 seconds

echo "🔄 Price refresh daemon started (interval: ${INTERVAL}s)"
echo "   API URL: ${API_URL}"

# Wait for the Next.js server to be ready
wait_for_server() {
    local max_attempts=30
    local attempt=0
    
    while [ $attempt -lt $max_attempts ]; do
        if curl -s -o /dev/null -w "%{http_code}" "${API_URL}/api/health" 2>/dev/null | grep -q "200\|404"; then
            echo "✅ Server is ready"
            return 0
        fi
        
        attempt=$((attempt + 1))
        echo "⏳ Waiting for server... (attempt $attempt/$max_attempts)"
        sleep 2
    done
    
    echo "⚠️  Server not responding after ${max_attempts} attempts, continuing anyway..."
    return 1
}

# Refresh market data for known tokens
refresh_prices() {
    # Try to get token list and refresh each one
    local tokens_response
    tokens_response=$(curl -s "${API_URL}/api/tokens" 2>/dev/null)
    
    if echo "$tokens_response" | grep -q '"success":true'; then
        # Extract token IDs and refresh each one
        local token_ids
        token_ids=$(echo "$tokens_response" | grep -o '"id":"[^"]*"' | sed 's/"id":"//g' | sed 's/"//g' | head -5)
        
        for token_id in $token_ids; do
            curl -s -o /dev/null "${API_URL}/api/market-data/${token_id}" 2>/dev/null
        done
        
        echo "✅ Prices refreshed at $(date '+%H:%M:%S')"
    else
        echo "⚠️  Could not fetch token list, skipping refresh"
    fi
}

# Main loop
wait_for_server

while true; do
    refresh_prices
    sleep $INTERVAL
done

