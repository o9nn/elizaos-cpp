#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * OTC Flow Validation Script
 * 
 * Validates ALL OTC flows end-to-end with real on-chain verification:
 * 1. Consignment creation (listing tokens)
 * 2. Negotiable vs Fixed terms
 * 3. Offer creation
 * 4. Price validation and pool discovery
 * 5. Backend approval
 * 6. Payment/fulfillment
 * 7. Withdrawal
 * 
 * Run: bun scripts/validate-otc-flows.ts
 * With real transactions: EXECUTE_TX=true bun scripts/validate-otc-flows.ts
 */

// CONFIGURATION

// Load deployment config

// ABI (Load from artifact or use parseAbi)

// UTILITIES

void log(const std::string& category, const std::string& message, std::optional<std::unordered_map<std::string, unknown>> data);

void section(const std::string& title);

// EVM VALIDATION

std::future<void> validateEVM();

// SOLANA VALIDATION

std::future<void> validateSolana();

// FLOW VALIDATION

std::future<void> validateFlows();

// EXECUTE REAL TRANSACTIONS (Optional)

std::future<void> executeRealTransactions();

// MAIN

std::future<void> main();


} // namespace elizaos
