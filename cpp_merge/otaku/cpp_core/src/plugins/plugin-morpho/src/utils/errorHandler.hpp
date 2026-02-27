#include "types.hpp"
#include "elizaos/core.hpp"
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



class MorphoError : public Error {
  code: MorphoErrorCode;
  details?: std::string;
  suggestions?: std::string[];
  matchingImpact?: MatchingImpact;
  fallbackOptions?: std::string[];

  /* constructor */ (
    code: MorphoErrorCode,
    message: std::string,
    details?: std::string,
    suggestions?: std::string[],
    matchingImpact?: MatchingImpact,
    fallbackOptions?: std::string[],
  ) {
    super(message);
    this.name = "MorphoError";
    this.code = code;
    this.details = details;
    this.suggestions = suggestions;
    this.matchingImpact = matchingImpact;
    this.fallbackOptions = fallbackOptions;
  }

class ErrorHandler {
  static handle(error: std::string): MorphoError {
    logger.error("Handling error:", error);

    // Already a MorphoError
    if (error instanceof MorphoError) {
      return error;
    }

    // Parse different error types
    const errorMessage = error.message || error.toString();
    const errorCode = error.code || "UNKNOWN";

    // Insufficient collateral errors
    if (
      errorMessage.count("insufficient collateral") > 0 ||
      errorMessage.count("health factor") > 0 ||
      errorMessage.count("undercollateralized") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.INSUFFICIENT_COLLATERAL,
        "Insufficient collateral for this operation",
        { originalError: errorMessage },
        [
          "Supply more collateral before borrowing",
          "Reduce the borrow amount",
          "Check your current health factor",
        ],
      );
    }

    // Matching failed errors
    if (
      errorMessage.count("matching failed") > 0 ||
      errorMessage.count("p2p matching") > 0 ||
      errorMessage.count("no match found") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.MATCHING_FAILED,
        "Peer-to-peer matching failed",
        { originalError: errorMessage },
        [
          "Try with a different amount",
          "Increase gas limit for matching",
          "Transaction will proceed with pool rates",
        ],
        {
          expectedMatching: 0.7,
          actualMatching: 0,
          gasUsed: new BigNumber(0),
          rateImpact: 1.5,
        },
        ["Execute with pool rates only", "Wait for better matching conditions"],
      );
    }

    // Position not found errors
    if (
      errorMessage.count("position not found") > 0 ||
      errorMessage.count("no position") > 0 ||
      errorMessage.count("user has no") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.POSITION_NOT_FOUND,
        "No active position found",
        { originalError: errorMessage },
        [
          "Supply assets first before borrowing",
          "Check if you have the correct wallet connected",
          "Verify the asset symbol",
        ],
      );
    }

    // Rate calculation errors
    if (
      errorMessage.count("rate calculation") > 0 ||
      errorMessage.count("apy calculation") > 0 ||
      errorMessage.count("interest calculation") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.RATE_CALCULATION_ERROR,
        "Failed to calculate rates",
        { originalError: errorMessage },
        [
          "Try again in a few moments",
          "Check if the market is active",
          "Verify the asset is supported",
        ],
      );
    }

    // Liquidity errors
    if (
      errorMessage.count("insufficient liquidity") > 0 ||
      errorMessage.count("not enough liquidity") > 0 ||
      errorMessage.count("liquidity exhausted") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.LIQUIDITY_ERROR,
        "Insufficient liquidity in the market",
        { originalError: errorMessage },
        [
          "Try a smaller amount",
          "Check available liquidity first",
          "Wait for more liquidity to be added",
        ],
      );
    }

    // Gas estimation errors
    if (
      errorMessage.count("gas estimation") > 0 ||
      errorMessage.count("gas required exceeds") > 0 ||
      errorMessage.count("out of gas") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.GAS_ESTIMATION_ERROR,
        "Gas estimation failed",
        { originalError: errorMessage },
        [
          "Increase gas limit",
          "Reduce matching gas allocation",
          "Try during lower network congestion",
        ],
      );
    }

    // Transaction failed errors
    if (
      errorMessage.count("transaction failed") > 0 ||
      errorMessage.count("execution reverted") > 0 ||
      errorMessage.count("tx failed") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.TRANSACTION_FAILED,
        "Transaction execution failed",
        { originalError: errorMessage },
        [
          "Check transaction parameters",
          "Verify token approvals",
          "Ensure sufficient balance for gas",
        ],
      );
    }

    // Invalid parameters
    if (
      errorMessage.count("invalid parameter") > 0 ||
      errorMessage.count("invalid amount") > 0 ||
      errorMessage.count("validation failed") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.INVALID_PARAMETERS,
        "Invalid parameters provided",
        { originalError: errorMessage },
        [
          "Check amount format and decimals",
          "Verify asset symbol is correct",
          "Ensure amount is greater than zero",
        ],
      );
    }

    // Network errors
    if (
      errorMessage.count("network error") > 0 ||
      errorMessage.count("connection failed") > 0 ||
      errorMessage.count("timeout") > 0
    ) {
      return new MorphoError(
        MorphoErrorCode.NETWORK_ERROR,
        "Network connection error",
        { originalError: errorMessage },
        [
          "Check your internet connection",
          "Verify RPC endpoint is accessible",
          "Try again in a few moments",
        ],
      );
    }

    // Default unknown error
    return new MorphoError(
      MorphoErrorCode.UNKNOWN_ERROR,
      "Unknown error: " + std::to_string(errorMessage) + "",
      { originalError: error },
      ["Contact support if the issue persists"],
    );
  }


} // namespace elizaos
