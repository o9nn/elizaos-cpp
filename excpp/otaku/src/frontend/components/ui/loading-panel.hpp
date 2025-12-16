#include "bullet.hpp"
#include "button.hpp"
#include "card.hpp"
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
;
;

struct LoadingPanelProps {
    std::string title;
    std::vector<string | string> messages;
    std::optional<'loading' | 'success' | 'error'> type;
    std::optional<() => void> onClose;
};


: LoadingPanelProps) {
  const messageArray = Array.isArray(messages) ? messages : [messages];

  return (
    <div className="fixed inset-0 z-[9999] flex items-center justify-center bg-black/80 ">
      <Card className="w-full max-w-md mx-4 bg-background">
        <CardHeader className="flex items-center justify-between pl-3 pr-1">
          <CardTitle className="flex items-center gap-2.5 text-sm font-medium uppercase">
            <Bullet />
            {title}
          </CardTitle>
        </CardHeader>
        <CardContent className="bg-pop">
          <div className="flex flex-col items-center justify-center py-8 space-y-4">
            {/* Icon based on type */}
            {type === 'loading' && (
              <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-[#0052FF]"></div>
            )}
            
            {type === 'success' && (
              <div className="h-12 w-12 rounded-full bg-green-500/20 flex items-center justify-center">
                <Check className="h-8 w-8 text-green-500" />
              </div>
            )}
            
            {type === 'error' && (
              <div className="h-12 w-12 rounded-full bg-destructive/20 flex items-center justify-center">
                <X className="h-8 w-8 text-destructive" />
              </div>
            )}

            {/* Messages */}
            {messageArray.map((message, index) => (
              <p key={index} className="text-sm text-muted-foreground text-center">
                {message}
              </p>
            ))}

            {/* Close button for success/error states */}
            {type !== 'loading' && onClose && (
              <Button onClick={onClose} className="mt-2 w-full" variant={type === 'error' ? 'destructive' : 'default'}>
                Close
              </Button>
            )}
          </div>
        </CardContent>
      </Card>
    </div>
  );
}


} // namespace elizaos
