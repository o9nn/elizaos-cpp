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



// Socket message types from ElizaOS core
enum SOCKET_MESSAGE_TYPE {
  ROOM_JOINING = 1,
  SEND_MESSAGE = 2,
  MESSAGE = 3,
  ACK = 4,
  THINKING = 5,
  CONTROL = 6,
}

// Direct connection to ElizaOS server for Socket.IO (proxying doesn't work for WebSocket)

// Enhanced types for ElizaOS Socket.IO events (matching official client)
using MessageBroadcastData = {

using MessageCompleteData = {

using ControlMessageData = std::variant<{

using MessageDeletedData = {

using ChannelClearedData = {

using ChannelDeletedData = {

using LogStreamData = {

// A simple class that provides EventEmitter-like interface using Evt internally
class EventAdapter {
  private events: Record<string, Evt<any>> = {};

    // Initialize common events

  // For checking if EventEmitter has listeners

  // Used only for internal access to the Evt instances

/**
 * SocketIOManager handles real-time communication between the client and server
 * using Socket.io. Based on the official ElizaOS client implementation.
 */
class SocketIOManager extends EventAdapter {
  private static instance: SocketIOManager | null = null;
  private socket: Socket | null = null;
  private isConnected = false;
  private connectPromise: Promise<void> | null = null;
  private resolveConnect: (() => void) | null = null;
  private activeChannels: Set<string> = new Set();
  private activeRooms: Set<string> = new Set(); // For backward compatibility
  private activeSessionChannelId: string | null = null; // Current session for message filtering
  private entityId: string | null = null;
  private serverId: string | null = null;

  // Public accessor for EVT instances (for advanced usage)
  public get evtMessageBroadcast() {
    return this._getEvt("messageBroadcast") as Evt<MessageBroadcastData>;
  }

  /**
   * Initialize the Socket.io connection to the server
   * @param entityId The client entity ID
   * @param serverId Server ID for channel-based messaging
   */

    // Create a single socket connection

    // Set up connection promise for async operations that depend on connection

      // Rejoin any active channels after reconnection

      // Rejoin any active rooms after reconnection (backward compatibility)

      // Check if this message is for our active session

      // Also check if it's for any of our joined channels (for backward compatibility)

        // Post the message to the event

      // Reset connect promise for next connection

  /**
   * Join a channel to receive messages from it
   * @param channelId Channel ID to join
   * @param serverId Optional server ID for the channel
   */

    // Wait for connection if needed

  /**
   * Join a room to receive messages from it (backward compatibility)
   * @param roomId Room/Agent ID to join
   */

    // Wait for connection if needed

  /**
   * Leave a channel to stop receiving messages from it
   * @param channelId Channel ID to leave
   */

  /**
   * Leave a room to stop receiving messages from it (backward compatibility)
   * @param roomId Room/Agent ID to leave
   */

  /**
   * Send a message to a specific channel
   * @param message Message text to send
   * @param channelId Channel ID to send the message to (usually central bus)
   * @param source Source identifier (e.g., 'client_chat')
   * @param sessionChannelId Optional session channel ID for filtering (following official client pattern)
   * @param serverId Optional server ID
   */

    // Wait for connection if needed

    // Emit message to server - always send to central bus but tag with session channel ID

    // Immediately broadcast message locally so UI updates instantly

  /**
   * Send a message to a specific room (backward compatibility)
   * @param message Message text to send
   * @param roomId Room/Agent ID to send the message to
   * @param source Source identifier (e.g., 'client_chat')
   */

    // Wait for connection if needed

    // Emit message to server

    // Immediately broadcast message locally so UI updates instantly

  /**
   * Subscribe to log streaming
   */

  /**
   * Unsubscribe from log streaming
   */

  /**
   * Update log filters
   */

  /**
   * Get active channels
   */

  /**
   * Get active rooms (backward compatibility)
   */

  /**
   * Check if connected
   */

  /**
   * Get entity ID
   */

  /**
   * Get server ID
   */

  /**
   * Set the active session channel ID for message filtering (following official client pattern)
   * @param sessionChannelId The session channel ID to filter messages by
   */

  /**
   * Get the current active session channel ID
   */

  /**
   * Clear the active session channel ID
   */

  /**
   * Disconnect from the server
   */


} // namespace elizaos
