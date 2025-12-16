#include ".types/shared.hpp"
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

// Block old WebSocket messages


      // Block specific message types or content
      if (message && message.type === 'send_message') {
        const messageText = message.message?.text || message.message?.content || '';
        if (messageText.includes('admin has opened the terminal')) {
          console.error('🚫 BLOCKED problematic message!', message);
          console.trace();
          return; // Don't send it
        }
      }
    } catch (_e) {
      // Not JSON or couldn't parse, let it through
    }

    // Allow other messages
    return originalSend.apply(this, [data]);
  };

  console.log('✅ WebSocket message blocker installed');
}

} // namespace elizaos
