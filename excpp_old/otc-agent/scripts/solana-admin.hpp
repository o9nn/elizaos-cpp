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
 * Solana OTC Desk Admin CLI
 * 
 * Consolidated admin utilities for managing the Solana OTC desk.
 * 
 * Usage:
 *   bun scripts/solana-admin.ts <command> [options]
 * 
 * Commands:
 *   create-treasury <TOKEN_MINT>           Create desk token treasury (ATA)
 *   register-token <TOKEN_MINT> [PRICE]    Register token on desk with optional price
 *   set-price <TOKEN_MINT> <PRICE_USD>     Set manual token price
 *   status                                 Show desk status and registered tokens
 */

std::future<Connection> getConnection();

std::future<Keypair> getWallet();

std::future<anchor::Program> getProgram(Connection connection, Keypair wallet);

// COMMANDS

std::future<void> createTreasury(const std::string& tokenMintStr);

std::future<void> registerToken(const std::string& tokenMintStr, std::optional<double> priceUsd);

std::future<void> setPrice(const std::string& tokenMintStr, double priceUsd);

std::future<void> showStatus();

// MAIN

void printUsage();

std::future<void> main();


} // namespace elizaos
