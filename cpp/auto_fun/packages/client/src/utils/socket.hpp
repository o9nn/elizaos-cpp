#include "env.hpp"
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



// Custom WebSocket wrapper to maintain Socket.io-like API
class SocketWrapper {
  private ws: WebSocket | null = null;
  private eventHandlers: Record<string, Array<(data: unknown) => void>> = {};
  // Queue for messages that need to be sent when connection is established
  // Store the client ID received from the server

    /**
     * TODO: update as necessary once backend socket implementation is finalized
     */

    // Create a connection promise that will be resolved when the connection is open

      // Reset reconnection state on successful connection

      // Process any queued messages

        // Handle server pings
          // Respond immediately

        // Listen for the clientId event specifically
          // Process queue again in case messages were waiting for clientId
          // Trigger other event handlers

      // Don't attempt to reconnect if disconnect() was called

      // Clear client ID on disconnect

      // Attempt to reconnect unless this was a normal closure

      // Calculate delay with exponential backoff, but cap at maxReconnectInterval

    // Only process queue if connected AND we have a client ID
        // Add clientId to the message before sending

    // Always queue the message first

      // If connected and have clientId, try processing queue immediately
        // Waiting for clientId event
      // Connection is in progress, wait for open and clientId
          // Connection opened, clientId might arrive soon, queue is fine
          // Error connecting, queue remains
      // Socket is closed or closing, attempt reconnect (queue remains)

    // Clear client ID on disconnect

    // Clear any pending reconnection attempts

// Create a type that matches the Socket.io Socket interface
using Socket = SocketWrapper;

    // Get apiUrl from environment and ensure it's defined
    // Pass the guaranteed string to the constructor

} // namespace elizaos
