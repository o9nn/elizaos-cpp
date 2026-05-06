#!/bin/bash

# Auto-install script for cloudflared
# Detects OS and installs cloudflared automatically

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BLUE}🚇 Cloudflared Installer for Farcaster Mini App Testing${NC}"
echo ""

# Check if already installed
if command -v cloudflared &> /dev/null; then
    VERSION=$(cloudflared version 2>&1 | head -n 1)
    echo -e "${GREEN}✓ cloudflared is already installed: $VERSION${NC}"
    echo ""
    echo "You can now run: bun run tunnel"
    exit 0
fi

# Detect OS
OS="$(uname -s)"
ARCH="$(uname -m)"

echo -e "${YELLOW}Detected OS: $OS ($ARCH)${NC}"
echo ""

case "$OS" in
    Darwin)
        echo -e "${BLUE}Installing cloudflared on macOS...${NC}"
        
        # Check if Homebrew is available
        if command -v brew &> /dev/null; then
            echo -e "${GREEN}✓ Homebrew detected, installing via brew...${NC}"
            brew install cloudflare/cloudflare/cloudflared
        else
            echo -e "${YELLOW}Homebrew not found, installing manually...${NC}"
            
            # Determine architecture
            if [[ "$ARCH" == "arm64" ]]; then
                DOWNLOAD_URL="https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-darwin-arm64.tgz"
            else
                DOWNLOAD_URL="https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-darwin-amd64.tgz"
            fi
            
            echo "Downloading from $DOWNLOAD_URL..."
            curl -L "$DOWNLOAD_URL" | tar -xz
            
            echo "Installing to /usr/local/bin..."
            sudo mv cloudflared /usr/local/bin/
            sudo chmod +x /usr/local/bin/cloudflared
        fi
        ;;
        
    Linux)
        echo -e "${BLUE}Installing cloudflared on Linux...${NC}"
        
        # Determine architecture
        if [[ "$ARCH" == "x86_64" ]]; then
            DOWNLOAD_URL="https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64"
        elif [[ "$ARCH" == "aarch64" ]] || [[ "$ARCH" == "arm64" ]]; then
            DOWNLOAD_URL="https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-arm64"
        else
            echo -e "${RED}❌ Unsupported architecture: $ARCH${NC}"
            exit 1
        fi
        
        echo "Downloading from $DOWNLOAD_URL..."
        wget -q "$DOWNLOAD_URL" -O cloudflared
        
        echo "Installing to /usr/local/bin..."
        chmod +x cloudflared
        sudo mv cloudflared /usr/local/bin/
        ;;
        
    MINGW*|MSYS*|CYGWIN*)
        echo -e "${RED}❌ Windows detected${NC}"
        echo ""
        echo "Please install cloudflared manually:"
        echo "1. Download from: https://developers.cloudflare.com/cloudflare-one/connections/connect-apps/install-and-setup/installation/"
        echo "2. Extract and add to PATH"
        echo "3. Run: bun run tunnel"
        exit 1
        ;;
        
    *)
        echo -e "${RED}❌ Unsupported OS: $OS${NC}"
        exit 1
        ;;
esac

# Verify installation
if command -v cloudflared &> /dev/null; then
    VERSION=$(cloudflared version 2>&1 | head -n 1)
    echo ""
    echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}✓ cloudflared successfully installed${NC}"
    echo -e "${GREEN}  Version: $VERSION${NC}"
    echo -e "${GREEN}════════════════════════════════════════════════════════${NC}"
    echo ""
    echo -e "${BLUE}Next steps:${NC}"
    echo "1. Start your dev server: ${YELLOW}bun run dev${NC}"
    echo "2. In another terminal, start tunnel: ${YELLOW}bun run tunnel${NC}"
    echo "3. Use the public URL for Farcaster testing"
    echo ""
else
    echo -e "${RED}❌ Installation failed${NC}"
    exit 1
fi


