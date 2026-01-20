#include ".lib/base-client.hpp"
#include ".types/base.hpp"
#include ".types/messaging.hpp"
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



class MessagingService extends BaseApiClient {
  /**
   * Submit agent replies or system messages
   */
  async submitMessage(params: MessageSubmitParams): Promise<Message> {
    return this.post<Message>('/api/messaging/submit', params);
  }

  /**
   * Notify message completion
   */

  /**
   * Ingest messages from external platforms
   */

  /**
   * Create a new channel
   */

  /**
   * Create a group channel
   */

  /**
   * Find or create a DM channel
   */

  /**
   * Get channel details
   */

  /**
   * Get channel participants
   */

  /**
   * Add agent to channel
   */

  /**
   * Remove agent from channel
   */

  /**
   * Delete a channel
   */

  /**
   * Clear channel history
   */

  /**
   * Post a new message to a channel
   */

  /**
   * Get channel messages
   */

  /**
   * Get a specific message
   */

  /**
   * Delete a message from a channel
   */

  /**
   * Update a message
   */

  /**
   * Search messages
   */

  /**
   * List all message servers
   */

  /**
   * Get server channels
   */

  /**
   * Create a new server
   */

  /**
   * Sync server channels
   */

  /**
   * Delete a server
   */

  /**
   * Update a channel
   */

  /**
   * Generate channel title
   */

  /**
   * Add user to channel participants (implemented via updateChannel)
   */
    // First get current participants

    // Add new user if not already present

  /**
   * Add multiple users to channel participants (implemented via updateChannel)
   */
    // First get current participants

    // Add new users that aren't already present

  /**
   * Remove user from channel participants (implemented via updateChannel)
   */
    // First get current participants

    // Remove user from participants


} // namespace elizaos
