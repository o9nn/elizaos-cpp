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

using ChatContainerRootProps = {
  children: React.ReactNode;
  className?: string;
} & React.HTMLAttributes<HTMLDivElement>;

using ChatContainerContentProps = {
  children: React.ReactNode;
  className?: string;
} & React.HTMLAttributes<HTMLDivElement>;

using ChatContainerScrollAnchorProps = {
  className?: string;
  ref?: React.RefObject<HTMLDivElement>;
} & React.HTMLAttributes<HTMLDivElement>;

: ChatContainerRootProps) {
  return (
    <StickToBottom
      className={cn('flex overflow-y-auto', className)}
      resize="smooth"
      initial="smooth"
      role="log"
      {...props}
    >
      {children}
    </StickToBottom>
  );
}

: ChatContainerContentProps) {
  return (
    <StickToBottom.Content className={cn('flex w-full flex-col', className)} {...props}>
      {children}
    </StickToBottom.Content>
  );
}

: ChatContainerScrollAnchorProps) {
  return (
    <div
      className={cn('h-px w-full shrink-0 scroll-mt-4', className)}
      aria-hidden="true"
      {...props}
    />
  );
}

{ ChatContainerRoot, ChatContainerContent, ChatContainerScrollAnchor };

} // namespace elizaos
