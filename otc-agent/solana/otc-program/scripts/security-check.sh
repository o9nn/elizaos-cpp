#!/bin/bash
# Security Analysis Script for OTC Solana Program
# Run this script to check for vulnerabilities

set -e

echo "🔒 OTC Program Security Analysis"
echo "================================="
echo ""

# Check for required tools
check_tool() {
    if command -v $1 &> /dev/null; then
        echo "✅ $1 found"
        return 0
    else
        echo "❌ $1 not found - install with: $2"
        return 1
    fi
}

echo "📋 Checking required tools..."
check_tool "cargo" "https://rustup.rs" || true
check_tool "anchor" "cargo install anchor-cli" || true
check_tool "solana" "sh -c \$(curl -sSfL https://release.solana.com/stable/install)" || true

echo ""
echo "📦 Installing security analysis tools..."

# 1. Cargo Audit - Check for known vulnerabilities in dependencies
echo ""
echo "1️⃣ Running cargo-audit (dependency vulnerabilities)..."
if ! command -v cargo-audit &> /dev/null; then
    echo "   Installing cargo-audit..."
    cargo install cargo-audit 2>/dev/null || echo "   Failed to install cargo-audit"
fi
if command -v cargo-audit &> /dev/null; then
    cd programs/otc
    cargo audit 2>&1 || echo "   ⚠️  Audit found issues or errored"
    cd ../..
fi

# 2. Clippy with extra lints
echo ""
echo "2️⃣ Running clippy with security lints..."
cd programs/otc
cargo clippy -- \
    -D warnings \
    -W clippy::cast_possible_truncation \
    -W clippy::cast_sign_loss \
    -W clippy::checked_conversions \
    -W clippy::integer_arithmetic \
    2>&1 | head -100 || echo "   ⚠️  Clippy found warnings"
cd ../..

# 3. Check for common Solana vulnerabilities
echo ""
echo "3️⃣ Checking for common Solana vulnerabilities..."

echo "   Checking for missing signer validations..."
grep -n "UncheckedAccount" programs/otc/src/lib.rs | head -20 || echo "   No UncheckedAccount found"

echo ""
echo "   Checking for potential overflow points..."
grep -n "as u64\|as u128\|as i64" programs/otc/src/lib.rs | head -20 || echo "   No casts found"

echo ""
echo "   Checking for checked_* arithmetic usage..."
CHECKED_COUNT=$(grep -c "checked_" programs/otc/src/lib.rs || echo "0")
echo "   Found $CHECKED_COUNT checked_* operations"

# 4. Build with overflow checks
echo ""
echo "4️⃣ Building with overflow checks..."
cargo build --release 2>&1 | tail -10

# 5. Generate security report
echo ""
echo "5️⃣ Generating security summary..."
echo ""
echo "================================="
echo "📊 SECURITY SUMMARY"
echo "================================="
echo ""
echo "Program: OTC (Over-The-Counter Trading)"
echo "File: programs/otc/src/lib.rs"
echo ""
echo "Known Issues to Address:"
echo "------------------------"
echo "❗ CRITICAL: Missing offer.desk == desk.key() validation in:"
echo "   - ApproveOffer"
echo "   - CancelOffer"  
echo "   - FulfillOfferUsdc"
echo "   - FulfillOfferSol"
echo "   - Claim"
echo ""
echo "❗ CRITICAL: Missing token_registry.desk == desk.key() validation in:"
echo "   - CreateOffer"
echo "   - CreateOfferFromConsignment"
echo "   - DepositTokens"
echo ""
echo "❗ HIGH: Pool price oracle vulnerable to manipulation"
echo "   - No TWAP protection"
echo "   - No AMM program verification"
echo "   - Permissionless pool_address registration"
echo ""
echo "❗ HIGH: No inventory reservation system"
echo "   - Multiple offers can overbook same tokens"
echo "   - Race condition between fulfill and claim"
echo ""
echo "⚠️  MEDIUM: Deprecated token_reserved used in emergency_refund"
echo "⚠️  MEDIUM: Missing treasury owner validation in Claim"
echo "⚠️  MEDIUM: Missing beneficiary ATA owner validation"
echo ""
echo "================================="
echo "📚 RECOMMENDED TOOLS"
echo "================================="
echo ""
echo "1. Trident (Fuzzing) - https://github.com/Ackee-Blockchain/trident"
echo "   cargo install trident-cli"
echo "   trident init"
echo "   trident fuzz run"
echo ""
echo "2. Soteria (Static Analysis)"
echo "   cargo install soteria-cli"
echo "   soteria analyze ./programs/otc"
echo ""
echo "3. Sec3 X-ray (Commercial)"
echo "   https://www.sec3.dev/"
echo ""
echo "4. Semgrep Solana Rules"
echo "   pip install semgrep"
echo "   semgrep --config p/solana ."
echo ""
echo "5. Bankrun (Time Manipulation Testing)"
echo "   bun add solana-bankrun"
echo ""
echo "================================="
echo ""
echo "Run 'anchor test' to execute security tests after starting validator"

