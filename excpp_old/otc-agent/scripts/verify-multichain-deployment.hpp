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
 * Verify Multi-Chain OTC Deployment
 * 
 * This script verifies that:
 * - Base OTC contract is deployed and configured correctly
 * - RegistrationHelper is deployed and can be used
 * - Solana program is deployed and operational
 * - Wallet scanning works on both chains
 * - Oracle discovery works
 */

std::future<void> verifyBaseDeployment();

std::future<void> verifySolanaDeployment();

std::future<void> testWalletScanning();

std::future<void> main();


} // namespace elizaos
