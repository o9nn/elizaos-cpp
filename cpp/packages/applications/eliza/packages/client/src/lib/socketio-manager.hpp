#include "elizaos/core.hpp"
#include "logger.hpp"
#include "utils.hpp"
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



// Define types for the events
using MessageBroadcastData = {

using MessageCompleteData = {

// Define type for control messages
using ControlMessageData = std::variant<{

// Define type for message deletion events
using MessageDeletedData = {

// Define type for channel cleared events
using ChannelClearedData = {

// Define type for channel deleted events
using ChannelDeletedData = {

// Define type for log stream messages
using LogStreamData = {

// A simple class that provides EventEmitter-like interface using Evt internally
class EventAdapter {
  private events: Record<string, Evt<any>> = {};

    // Initialize common events

  // For checking if EventEmitter has listeners

  // Used only for internal access to the Evt instances

/**
 * SocketIOManager handles real-time communication between the client and server
 * using Socket.io. It maintains a single connection to the server and allows
 * joining and messaging in multiple rooms.
 */
class SocketIOManager extends EventAdapter {
  private static instance: SocketIOManager | null = null;
  private socket: Socket | null = null;
  private isConnected = false;
  private connectPromise: Promise<void> | null = null;
  private resolveConnect: (() => void) | null = null;
  private activeChannelIds: Set<string> = new Set();
  private clientEntityId: string | null = null;
  private logStreamSubscribed: boolean = false;

  // Public accessor for EVT instances (for advanced usage)
  public get evtMessageBroadcast() {
    return this._getEvt('messageBroadcast') as Evt<MessageBroadcastData>;
  }

  /**
   * Initialize the Socket.io connection to the server
   * @param clientEntityId The client entity ID (central user ID)
   */

    // Create a single socket connection

    // Set up connection promise for async operations that depend on connection

      // Add debug listener for all incoming events

      // CRITICAL: Ensure this loop remains commented out or removed.
      // this.activeChannelIds.forEach((channelId) => {
      //   clientLogger.info(`[SocketIO] 'connect' event: Attempting to re-join active channel ${channelId} (THIS SHOULD NOT HAPPEN AUTOMATICALLY)`);
      //   this.joinChannel(channelId);
      // });

      // Log the full data structure to understand formats

      // Check if this is a message for one of our active channels
        // Post the message to the event for UI updates

    // Listen for control messages

      // Check if this is for one of our active channels

        // Emit the control message event

    // Listen for message deletion events

      // Check if this is for one of our active channels

        // Emit the message deleted event

    // Listen for channel cleared events

      // Check if this is for one of our active channels

        // Emit the channel cleared event

    // Listen for channel deleted events

      // Check if this is for one of our active channels

        // Emit the channel deleted event (same as cleared for now)

      // Reset connect promise for next connection

    // Handle log stream events

  /**
   * Join a channel to receive messages from it
   * @param channelId Channel ID to join
   */

    // Wait for connection if needed

  /**
   * @deprecated Use joinChannel instead
   */

  /**
   * Leave a channel to stop receiving messages from it
   * @param channelId Channel ID to leave
   */

    // No server-side message for leaving a room in this client's protocol,
    // client just stops listening / tracking.

  /**
   * @deprecated Use leaveChannel instead
   */

  /**
   * Send a message to a specific channel
   * @param message Message text to send
   * @param channelId Channel ID to send the message to
   * @param serverId Server ID to send the message to
   * @param source Source identifier (e.g., 'client_chat')
   * @param attachments Optional media attachments
   * @param messageId Optional message ID for tracking optimistic updates
   */

    // Wait for connection if needed

    // Use provided messageId or generate a new one

    // Emit message to server

    // Note: We no longer broadcast locally - the server will send the message back with the proper ID

  /**
   * Subscribe to log streaming
   */

    // Wait for connection if needed

  /**
   * Unsubscribe from log streaming
   */

    // Wait for connection if needed

  /**
   * Update log stream filters
   */

    // Wait for connection if needed

  /**
   * Check if subscribed to log streaming
   */

  /**
   * Disconnect from the server
   */


} // namespace elizaos
