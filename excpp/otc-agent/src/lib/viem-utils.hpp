#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Type-safe viem utilities for contract interactions
 *
 * viem 2.x has very strict generic types for readContract that require
 * exact ABI type inference. When using dynamic ABIs (loaded from JSON artifacts),
 * this causes type errors. This module provides type-safe wrappers.
 */

/**
 * Parameters for reading a contract with a dynamic ABI
 */
struct ReadContractParams {
    Address address;
    Abi abi;
    std::string functionName;
    std::optional<std::vector<readonly unknown>> args;
};

/**
 * Type-safe wrapper for readContract that works with dynamic ABIs.
 *
 * Use this when the ABI is loaded from a JSON artifact at runtime,
 * which prevents TypeScript from inferring the exact return type.
 *
 * @example
 * ```ts
 *   address: contractAddress,
 *   abi: artifact.abi as Abi,
 *   functionName: "balanceOf",
 *   args: [userAddress],
 * });
 * ```
 */
  // The cast is necessary because viem's readContract has strict generics
  // that require compile-time ABI type inference. With dynamic ABIs,
  // we must bypass this and rely on runtime behavior.
  // Uses unknown cast to bypass viem's strict authorizationList requirement

/**
 * ERC-20 ABI for common token operations
 * Defined as const for full type inference
 */

/**
 * Type-safe ERC-20 token info reader
 */

/**
 * Type-safe ERC-20 balance reader
 */
std::future<bigint> readERC20Balance(PublicClient client, Address tokenAddress, Address account);

} // namespace elizaos
