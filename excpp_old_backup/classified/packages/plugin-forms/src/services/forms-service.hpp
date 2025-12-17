#include ".types.hpp"
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



// Zod schemas for validation

// Raw schema for database rows (before decryption)

// Schema for validating decrypted field values

/**
 * FormsService manages form lifecycle and state.
 * It provides methods to create, update, list, and cancel forms.
 */
class FormsService extends Service {
  static serviceName = 'forms';
  static serviceType = FormsServiceType.FORMS as ServiceTypeName;

  private forms: Map<UUID, Form> = new Map();
  private templates: Map<string, FormTemplate> = new Map();
  private persistenceTimer?: NodeJS.Timeout;
  private cleanupTimer?: NodeJS.Timeout;
  private tablesChecked = false;
  private tablesExist = false;

  capabilityDescription = 'Form management service for collecting structured data from users';

  constructor(runtime: IAgentRuntime) {
    super(runtime);
    this.initialize(runtime);
  }

    // Check if database tables exist

    // Restore persisted forms only if tables exist

    // Set up auto-persistence with batch processing

    // Set up cleanup of completed/expired forms

    // Basic contact form template

  /**
   * Force immediate persistence of all forms
   * Uses batch transaction for efficiency and safety
   */

  /**
   * Wait for database tables to be available
   * This can be called by plugins that depend on forms persistence
   */

  /**
   * Check if persistence is available
   */

  /**
   * Create a new form from a template or custom definition
   */

      // Create from template

      // Create from custom form

  /**
   * Update form with extracted values from user message
   */

    // Extract values from message using LLM
        // Check if field already has a valid value

        // For optional fields, only process if mentioned in message
          // More flexible matching for optional fields

        // Required fields should always be processed

    // Update field values
        // Accept all values including falsy ones (false, 0, empty string)
          // Additional type validation
            // Encrypt secret fields before storing

    // Check if all required fields in current step are filled

      // Execute step callback if defined

      // Move to next step or complete form
        // All steps completed

        // Execute form completion callback if defined
      // Prepare message about missing fields

  /**
   * Extract form values from text using LLM
   */

      // The response is directly a string for TEXT_SMALL model

      // Extract JSON from response

      // Validate JSON parsing with Zod

      // Create dynamic Zod schema based on expected fields

        // Use validated and transformed values

  /**
   * List all forms
   */

    // Directly iterate over the Map instead of converting to array first

  /**
   * Get a specific form
   */

  /**
   * Cancel a form
   */

  /**
   * Register a form template
   */

  /**
   * Get all registered templates
   */

  /**
   * Clean up old completed/cancelled forms
   */

      // Remove completed forms older than 1 hour
      // Remove cancelled or other non-active forms older than specified time

    // Persist forms before stopping using batch method

    // Clear timers

      // Check if forms table exists - try multiple approaches for different databases

        // Try SQLite first (most common in development)
          // Not SQLite, try PostgreSQL
            // Try with plugin namespace for dynamic migration system
              // Last resort: try a simple query on the forms table

        // If tables still not found, check if migration service can create them
            // Don't set tablesExist = false yet, let the migration run first
        // If the query fails, assume tables don't exist

  /**
   * Recheck if database tables exist (call this after migration)
   */

    // If tables are now available, restore persisted forms

      // Skip if tables don't exist

      // Collect all forms to persist

      // Use a single transaction for all forms

        // Batch insert/update all forms

          // Batch insert/update all fields for this form

        // Commit all changes at once
        // Rollback entire batch on any error

        // If batch fails, fall back to individual transactions

    // Original implementation with individual transactions

      // Skip if tables don't exist
        // Periodically recheck in case tables were created
          // Check 10% of the time

      // Persist each form to the database

        // Wrap each form's persistence in a transaction

          // Serialize form data for database storage
                // Don't serialize callbacks

          // Use raw SQL for compatibility with different database adapters

          // Persist form fields

          // Commit the transaction if all operations succeeded
          // Rollback on any error

      // Skip if tables don't exist

      // Restore forms from database

          // Validate and sanitize form data

          // Parse steps from validated form

          // Restore form fields

          // Group fields by step

          // Reconstruct form with validated fields

                  // Decrypt first if needed

                  // Validate decrypted value

          // Only add form if it has at least one valid step with fields

    // Decrypt secret values first

        return Number(processedValue);

        // For text, textarea, choice - accept strings

    // Use the public cleanup method with default 24 hour expiration

} // namespace elizaos
