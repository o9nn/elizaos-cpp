#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "...utils/entity.hpp"
#include ".services/cdp.service.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * FETCH_WITH_PAYMENT Action
 * 
 * Makes HTTP requests to x402-enabled paid APIs with automatic payment handling.
 * 
 * This action integrates Coinbase's x402 payment protocol to enable agents to:
 * - Make requests to APIs that require onchain payment (HTTP 402)
 * - Automatically detect payment requirements from API responses
 * - Sign and submit USDC payment transactions on Base network
 * - Retry requests with proof of payment
 * - Return both API response data and payment transaction details
 * 
 * Payment Flow:
 * 1. Agent makes initial HTTP request to x402-enabled API
 * 2. API returns 402 Payment Required with payment details
 * 3. Action verifies payment is within maxPayment limit
 * 4. Creates and signs USDC payment transaction using CDP wallet
 * 5. Submits payment onchain and gets transaction hash
 * 6. Retries original request with payment proof header
 * 7. Returns API response + payment receipt to user
 * 
 * @see https://docs.cdp.coinbase.com/x402/quickstart-for-buyers
 * 
 * @example
 * // User: "fetch https://api.example.com/paid-data with payment"
 * // Agent executes: FETCH_WITH_PAYMENT { url: "https://...", maxPayment: 1.0 }
 * 
 * @requires CDP Service - Must be configured with API credentials
 * @requires CDP Wallet - User must have wallet with USDC balance on Base
 * @requires x402-fetch - Payment wrapper library (installed via package.json)
 */

/**
 * Helper function to create standardized error results and invoke callback
 */

/**
 * Helper function to determine status indicator and prefix based on response status
 */

/**
 * Helper function to format and truncate response data
 */
std::string formatResponseData(unknown responseData, double maxLength = 500);

      // Check if CDP service is available

      // Read parameters from state (extracted by multiStepDecisionTemplate)
      
      // Extract and validate parameters

      // Store input parameters for return

      // Validate URL
        return createErrorResult("URL is required", "missing_url", inputParams, callback);

      // Validate URL format
        new URL(url);
        return createErrorResult(`Invalid URL format: ${url}`, "invalid_url", inputParams, callback);

      // Validate method

      // Validate maxPayment

      // Get CDP service

      // Get the viem wallet client (defaults to base network)

      // Ensure account is defined

      // Convert maxPayment from USDC to base units (USDC has 6 decimals)

      // Wrap fetch with payment capability
      // Cast to any to bypass type incompatibility - walletClient is a valid SignerWallet

      // Prepare fetch options

      // Add body for POST requests

      // Make the request

      // Parse response

      // Decode payment response if present

      // Determine if this was actually a paid request
      
      // Determine success based on status code
      
      // Build appropriate status message

      // Only show response body for successful requests or if it's informative


} // namespace elizaos
