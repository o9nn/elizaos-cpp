#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

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

;

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
 * const result = await safeReadContract<bigint>(client, {
 *   address: contractAddress,
 *   abi: artifact.abi as Abi,
 *   functionName: "balanceOf",
 *   args: [userAddress],
 * });
 * ```
 */
async 

/**
 * ERC-20 ABI for common token operations
 * Defined as const for full type inference
 */
const ERC20_ABI = [
  {
    name: "symbol",
    type: "function",
    stateMutability: "view",
    inputs: [],
    outputs: [{ type: "string" }],
  },
  {
    name: "name",
    type: "function",
    stateMutability: "view",
    inputs: [],
    outputs: [{ type: "string" }],
  },
  {
    name: "decimals",
    type: "function",
    stateMutability: "view",
    inputs: [],
    outputs: [{ type: "uint8" }],
  },
  {
    name: "balanceOf",
    type: "function",
    stateMutability: "view",
    inputs: [{ name: "account", type: "address" }],
    outputs: [{ type: "uint256" }],
  },
  {
    name: "allowance",
    type: "function",
    stateMutability: "view",
    inputs: [
      { name: "owner", type: "address" },
      { name: "spender", type: "address" },
    ],
    outputs: [{ type: "uint256" }],
  },
  {
    name: "approve",
    type: "function",
    stateMutability: "nonpayable",
    inputs: [
      { name: "spender", type: "address" },
      { name: "amount", type: "uint256" },
    ],
    outputs: [{ type: "bool" }],
  },
  {
    name: "transfer",
    type: "function",
    stateMutability: "nonpayable",
    inputs: [
      { name: "to", type: "address" },
      { name: "amount", type: "uint256" },
    ],
    outputs: [{ type: "bool" }],
  },
] as const;

/**
 * Type-safe ERC-20 token info reader
 */
std::future<{ symbol: string; name: string; decimals: number }> readERC20Info(PublicClient client, Address tokenAddress);),
    safeReadContract<string>(client, {
      address: tokenAddress,
      abi: ERC20_ABI as unknown as Abi,
      functionName: "name",
    }),
    safeReadContract<number>(client, {
      address: tokenAddress,
      abi: ERC20_ABI as unknown as Abi,
      functionName: "decimals",
    }),
  ]);
  return { symbol, name, decimals };
}

/**
 * Type-safe ERC-20 balance reader
 */
std::future<bigint> readERC20Balance(PublicClient client, Address tokenAddress, Address account););
}

} // namespace elizaos
