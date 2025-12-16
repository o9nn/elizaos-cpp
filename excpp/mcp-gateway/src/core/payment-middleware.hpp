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



struct PaymentVerificationResult {
    bool authorized;
    std::optional<std::string> error;
    std::optional<{> pricing;
    std::string amount;
    'x402' | 'apiKey' | 'free' method;
};

struct PaymentRequirements {
    double x402Version;
    Array<{ accepts;
    std::string scheme;
    std::string network;
    std::string maxAmountRequired;
    std::string resource;
    std::string description;
    std::optional<std::string> mimeType;
    std::string payTo;
    double maxTimeoutSeconds;
    std::string asset;
    std::optional<{> extra;
    std::string name;
    std::string version;
    std::optional<std::string> error;
};

/**
 * Middleware for handling payment verification for MCP tools
 * Supports both x402 blockchain payments and ELIZA_API_KEY authentication
 */
class PaymentMiddleware {
  private config: GatewayConfig;
  private apiKeyCache = new Map<string, ApiKeyConfig>();
  private logger: Console;
  private verifyPaymentFunc: (payment: PaymentPayload, requirements: any) => Promise<any>;
  private settlePaymentFunc: (payment: PaymentPayload, requirements: any) => Promise<any>;

  // USDC contract addresses by network
  private static readonly USDC_ADDRESSES: Record<string, string> = {
    'base-sepolia': '0x036CbD53842c5426634e7929541eC2318f3dCF7e',
    'base': '0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913',
    'ethereum': '0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48',
    'optimism': '0x0b2C639c533813f4Aa9D7837CAf62653d097Ff85',
    'polygon': '0x3c499c542cEF5E3811e1192ce70d8cC03d5c3359'
  };

    // Initialize x402 facilitator client

    // Build API key lookup cache

  /**
   * Verify if a payment is valid for accessing a tool
   */

    // Get tool pricing configuration

    // If no pricing is configured, tool is free

    // Check if tool is explicitly marked as free

    // Try API key authentication first (faster than x402)

    // Try x402 payment verification

    // No valid payment found

  /**
   * Verify API key authentication
   */

    // Check for API key in multiple header formats

    // Check if this tier gets free or discounted access

    // API key valid but no tier pricing configured

  /**
   * Verify x402 blockchain payment
   */

      // Decode payment using x402 SDK

      // Build payment requirements using x402 SDK
      
      // Use x402 SDK to process price to atomic amount

      // Construct requirements exactly as x402 SDK expects

      // Add EIP-712 info if available

      // Verify with facilitator using x402 SDK

      // Optionally settle the payment
        // Continue anyway - payment was verified even if settlement is delayed

  /**
   * Get pricing configuration for a specific tool
   */

    // Check for tool-specific pricing

    // Fall back to server default pricing

  /**
   * Generate 402 Payment Required response
   */
    
    // x402 requires a full URL for the resource field
    
    // Get checksummed address (required by x402)

    // Use x402 SDK to process price to atomic amount

    // Add EIP-712 info if available

  /**
   * Convert dollar amount to atomic units (USDC has 6 decimals)
   */

    // USDC has 6 decimals

  /**
   * Get USDC contract address for network
   */

  /**
   * Get payment mode for a server
   */

  /**
   * Calculate markup amount based on downstream price
   * @param downstreamPrice - Price from downstream API (e.g., "$0.01")
   * @param markup - Markup configuration (e.g., "20%" or "$0.005")
   * @returns Final price with markup applied
   */

      // Percentage markup
      // Fixed amount markup

  /**
   * Extract payment headers for forwarding (passthrough mode)
   */

    // Forward x402 payment headers

    // Forward API key headers

  /**
   * Get payment statistics
   */
} // namespace elizaos
