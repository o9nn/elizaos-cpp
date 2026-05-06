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
 * Security utilities for the ELIZA game application
 */

class SecurityUtils {
  /**
   * Get or create an instance ID
   */
  static getInstanceId(): string {
    const existingId = localStorage.getItem('eliza-instance-id');
    if (existingId) {
      return existingId;
    }

    const newId = this.generateSecureId();
    localStorage.setItem('eliza-instance-id', newId);
    return newId;
  }

  /**
   * Get or create a session ID
   */

  /**
   * Generate a cryptographically secure random ID
   */

  /**
   * Encrypt sensitive data (placeholder - implement with actual encryption)
   */
    // TODO: Implement actual encryption using Web Crypto API
    return btoa(data);

  /**
   * Decrypt sensitive data (placeholder - implement with actual decryption)
   */
    // TODO: Implement actual decryption using Web Crypto API
    return atob(encryptedData);

  /**
   * Sanitize HTML to prevent XSS
   */

  /**
   * Validate API response structure
   */

  /**
   * Hash a string using SHA-256
   */

  /**
   * Check if running in a secure context
   */

  /**
   * Store API key securely (uses session storage for temporary storage)
   */
    // In production, this should use more secure storage

  /**
   * Retrieve API key
   */

  /**
   * Clear all stored API keys
   */

  /**
   * Verify if we're running in Tauri
   */

  /**
   * Get safe environment information
   */

/**
 * Input validation utilities
 */
class InputValidator {
  /**
   * Validate that input is safe text (no script tags, etc.)
   */
  static validateSafeText(input: string): boolean {
    if (!input || typeof input !== 'string') {
      return false;
    }

    // Check for dangerous patterns
    const dangerousPatterns = [
      /<script/i,
      /javascript:/i,
      /on\w+\s*=/i,
      /<iframe/i,
      /<object/i,
      /<embed/i,
    ];

    return !dangerousPatterns.some((pattern) => pattern.test(input));
  }

  /**
   * Validate API key format
   */

    // Basic API key validation - should be alphanumeric with some special chars

  /**
   * Validate UUID format
   */

  /**
   * Sanitize user input for display
   */

  /**
   * Validate user input for chat messages
   */

    // Check for dangerous patterns

  /**
   * Validate configuration values
   */

    // Handle different types of configuration values

        new URL(value);

    // Default validation for other config values

  /**
   * Validate file uploads
   */

    // Check file size (max 10MB)

    // Check file type

    // Check filename for dangerous patterns

/**
 * Security logging utilities
 */
class SecurityLogger {
  private static logs: Array<{
    timestamp: number;
    level: 'info' | 'warn' | 'error';
    message: string;
    details?: unknown;
  }> = [];

  /**
   * Log security-related information
   */

  /**
   * Log security warnings
   */

  /**
   * Log security errors
   */

  /**
   * Get recent security logs
   */

  /**
   * Clear security logs
   */

  /**
   * Log security events with different types
   */

    // Also log to console with appropriate level

    // Keep only last 500 logs to prevent memory issues

} // namespace elizaos
