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
 * Client for making outbound x402 payments to downstream APIs
 * Uses x402-axios to automatically handle 402 Payment Required responses
 */
class X402PaymentClient {
  private axiosInstance: AxiosInstance;
  private config: GatewayConfig;
  private logger: Console;
  private walletAddress?: string;

  constructor(config: GatewayConfig, logger: Console = console) {
    this.config = config;
    this.logger = logger;

    // Create base axios instance
    this.axiosInstance = axios.create({
      timeout: 30000,
      headers: {
        'Content-Type': 'application/json'
      }
    });

    // If outbound wallet is configured, wrap with payment interceptor
    if (config.payment?.outboundWallet) {
      try {
        const account = privateKeyToAccount(config.payment.outboundWallet as `0x${string}`);
        this.walletAddress = account.address;
        this.axiosInstance = withPaymentInterceptor(this.axiosInstance, account);
        this.logger.info(`X402 payment client initialized with wallet: ${this.walletAddress}`);
      } catch (error) {
        this.logger.error(`Failed to initialize x402 payment client: ${error}`);
        throw error;
      }
    }
  }

  /**
   * Create a payment-enabled HTTP client for a specific server
   * Supports server-specific wallet override
   */
    // If server has its own wallet, create a new client

    // Otherwise, use the global payment client

  /**
   * Make an HTTP request with automatic x402 payment handling
   */

  /**
   * Forward a request with payment headers (passthrough mode)
   * Inbound payment headers are forwarded to downstream API
   */
    // Merge inbound payment headers with request headers

  /**
   * Get wallet address being used for payments
   */

  /**
   * Check if payment client is enabled
   */

} // namespace elizaos
