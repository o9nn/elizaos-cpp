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

interface ChatMessageListProps extends React.HTMLAttributes<HTMLDivElement> {
  scrollRef: React.RefObject<HTMLDivElement | null>;
  contentRef?: React.RefObject<HTMLDivElement | null>; // Optional content ref for StickToBottom
  isAtBottom: boolean;
  scrollToBottom: () => void;
  disableAutoScroll: () => void;
  smooth?: boolean;
}

const ChatMessageList = React.forwardRef<HTMLDivElement, ChatMessageListProps>(
  (
    {
      className,
      children,
      scrollRef,
      contentRef,
      isAtBottom,
      scrollToBottom,
      disableAutoScroll,
      ...props
    },
    _ref
  ) => {
    return (
      <div className="relative w-full h-full overflow-hidden">
        <div
          className={`flex flex-col w-full h-full p-2 overflow-y-auto ${className}`}
          ref={scrollRef}
          onWheel={disableAutoScroll}
          onTouchMove={disableAutoScroll}
          {...props}
        >
          <div className="flex flex-col gap-1" ref={contentRef}>
            {children}
          </div>
        </div>

        {!isAtBottom && (
          <Button
            onClick={() => {
              scrollToBottom();
            }}
            size="icon"
            variant="outline"
            className="absolute bottom-2 left-1/2 transform -translate-x-1/2 inline-flex rounded-full shadow-md"
          >
            <ArrowDown className="h-4 w-4" />
          </Button>
        )}
      </div>
    );
  }
);

ChatMessageList.displayName = 'ChatMessageList';

{ ChatMessageList };

} // namespace elizaos
