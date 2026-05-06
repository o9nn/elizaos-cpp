/**
 * Type-safe viem utilities for contract interactions
 *
 * viem 2.x has very strict generic types for readContract that require
 * exact ABI type inference. When using dynamic ABIs (loaded from JSON artifacts),
 * this causes type errors. This module provides type-safe wrappers.
 */

import { type PublicClient, type Address, type Abi } from "viem";

/**
 * Parameters for reading a contract with a dynamic ABI
 */
export interface ReadContractParams {
  address: Address;
  abi: Abi;
  functionName: string;
  args?: readonly unknown[];
}

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
export async function safeReadContract<T>(
  client: PublicClient,
  params: ReadContractParams,
): Promise<T> {
  // The cast is necessary because viem's readContract has strict generics
  // that require compile-time ABI type inference. With dynamic ABIs,
  // we must bypass this and rely on runtime behavior.
  // Uses unknown cast to bypass viem's strict authorizationList requirement
  const result = await (
    client.readContract as (params: unknown) => Promise<unknown>
  )(params);
  return result as T;
}

/**
 * ERC-20 ABI for common token operations
 * Defined as const for full type inference
 */
export const ERC20_ABI = [
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
export async function readERC20Info(
  client: PublicClient,
  tokenAddress: Address,
): Promise<{ symbol: string; name: string; decimals: number }> {
  const [symbol, name, decimals] = await Promise.all([
    safeReadContract<string>(client, {
      address: tokenAddress,
      abi: ERC20_ABI as unknown as Abi,
      functionName: "symbol",
    }),
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
export async function readERC20Balance(
  client: PublicClient,
  tokenAddress: Address,
  account: Address,
): Promise<bigint> {
  return safeReadContract<bigint>(client, {
    address: tokenAddress,
    abi: ERC20_ABI as unknown as Abi,
    functionName: "balanceOf",
    args: [account],
  });
}
