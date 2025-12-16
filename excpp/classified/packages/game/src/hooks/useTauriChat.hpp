#include ".services/TauriService.hpp"
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

;
;

struct UseTauriChatReturn {
    bool isConnected;
    (content: string) => Promise<void> sendMessage;
    std::vector<TauriMessage> messages;
    string | null error;
    bool isLoading;
};


// Hook for using Tauri-based chat functionality
 else {
              console.log('[AUTO-GREETING] Agent not ready, skipping auto-greeting');
            }
          } catch (error) {
            console.warn('[AUTO-GREETING] Failed to send auto-greeting:', error);
          }
        }, 2000); // Wait 2 seconds for services to be fully ready
      } else {
        setError('Not running in Tauri environment');
      }

      setIsLoading(false);
    };

    initialize();

    // Set up message listener
    const unsubscribe = TauriService.onMessage((message) => {
      setMessages((prev) => [...prev, message]);
    });

    return () => {
      unsubscribe();
    };
  }, []);

  const sendMessage = async (content: string) => {
    await TauriService.sendMessage(content);
  };

  return {
    isConnected,
    sendMessage,
    messages,
    error,
    isLoading,
  };
}

} // namespace elizaos
