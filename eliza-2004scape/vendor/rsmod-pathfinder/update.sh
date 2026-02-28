#!/bin/bash
# Updates the vendored rsmod-pathfinder WASM files from source
# Requires: rust, wasm-pack

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_URL="https://github.com/MaxBittker/rsmod-pathfinder.git"
TMP_DIR=$(mktemp -d)

echo "Cloning rsmod-pathfinder..."
git clone --depth 1 "$REPO_URL" "$TMP_DIR"

echo "Building WASM with wasm-pack..."
cd "$TMP_DIR"
wasm-pack build --target nodejs --out-dir dist

echo "Copying dist files to vendor..."
cp dist/rsmod-pathfinder.js "$SCRIPT_DIR/"
cp dist/rsmod-pathfinder.d.ts "$SCRIPT_DIR/"
cp dist/rsmod-pathfinder_bg.wasm "$SCRIPT_DIR/"
cp dist/rsmod-pathfinder_bg.wasm.d.ts "$SCRIPT_DIR/"

echo "Cleaning up..."
rm -rf "$TMP_DIR"

echo "Done! Vendored files updated."
ls -la "$SCRIPT_DIR"
