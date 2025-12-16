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



// Socket message types (must match server's SOCKET_MESSAGE_TYPE enum)

class SocketManager {
  private socket: Socket | null = null;
  private userId: string | null = null;
  private activeChannels: Set<string> = new Set();

  connect(userId: string) {
    if (this.socket?.connected) {
      console.log('Socket already connected');
      return this.socket;
    }
    
    this.userId = userId;
    this.socket = io(window.location.origin + '/', {
      autoConnect: true,
      reconnection: true,
      reconnectionDelay: 1000,
      reconnectionAttempts: 5,
    });

    this.socket.on('connect', () => {
      console.log(' Connected to Eliza server');
    });

    this.socket.on('disconnect', (reason) => {
      console.log(' Disconnected from Eliza server:', reason);
    });

    this.socket.on('connect_error', (error) => {
      console.error('Connection error:', error);
    });

    return this.socket;
  }


} // namespace elizaos
