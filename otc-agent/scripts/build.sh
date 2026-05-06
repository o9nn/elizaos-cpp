#!/bin/bash
# Build for production
set -e

cd "$(dirname "$0")/.."

echo "Building for production..."

./scripts/ensure-deployments.sh
next build

