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
 * Represents the schema for Solana environment variables.
 * * @type {import("zod").ZodIntersection<
 * import("zod").ZodObject<{
 * WALLET_SECRET_SALT?: import("zod").ZodString | undefined;
 * }, "strip">,
 * import("zod").ZodUnion<
 * import("zod").ZodObject<{
 * WALLET_SECRET_KEY: import("zod").ZodString;
 * WALLET_PUBLIC_KEY: import("zod").ZodString;
 * }, "strip"> |
 * import("zod").ZodObject<{
 * WALLET_SECRET_SALT: import("zod").ZodString;
 * }, "strip">, "strict"> & import("zod").ZodObject<{
 * SOL_ADDRESS: import("zod").ZodString;
 * SLIPPAGE: import("zod").ZodString;
 * SOLANA_RPC_URL: import("zod").ZodString;
 * HELIUS_API_KEY: import("zod").ZodString;
 * BIRDEYE_API_KEY: import("zod").ZodString;
 * }, "strict">;
 */

/**
 * Type definition for the configuration of a Solana environment.
 */
using SolanaConfig = z::infer<typeof solanaEnvSchema>;

/**
 * Validates the Solana configuration by retrieving settings from the runtime or environment variables,
 * checking if they are present, and returning a validated SolanaConfig object.
 *
 * @param {IAgentRuntime} runtime - The agent runtime object used to retrieve settings.
 * @returns {Promise<SolanaConfig>} - A promise that resolves with the validated SolanaConfig object.
 * @throws {Error} - If the Solana configuration validation fails.
 */
std::future<SolanaConfig> validateSolanaConfig(IAgentRuntime runtime);

} // namespace elizaos
