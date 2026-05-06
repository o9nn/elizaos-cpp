#!/bin/bash

# Ensure deployment directory exists
mkdir -p src/config/deployments

# Create empty JSON files if they don't exist to prevent build errors
for env in local testnet mainnet; do
  for chain in evm solana; do
    file="src/config/deployments/${env}-${chain}.json"
    if [ ! -f "$file" ]; then
      echo "{}" > "$file"
      echo "Created placeholder $file"
    fi
  done
done


