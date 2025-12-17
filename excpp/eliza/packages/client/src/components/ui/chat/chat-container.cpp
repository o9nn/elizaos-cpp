#include "chat-container.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatContainerRoot(auto { children, auto className, ChatContainerRootProps ...props }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <StickToBottom;
    className={cn('flex overflow-y-auto', className)}
    resize="smooth";
    initial="smooth";
    role="log";
    {...props}
    >;
    {children}
    </StickToBottom>;
    );

}

void ChatContainerContent(auto { children, auto className, ChatContainerContentProps ...props }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <StickToBottom.Content className={cn('flex w-full flex-col', className)} {...props}>;
    {children}
    </StickToBottom.Content>;
    );

}

void ChatContainerScrollAnchor(auto { className, ChatContainerScrollAnchorProps ...props }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    className={cn('h-px w-full shrink-0 scroll-mt-4', className)}
    aria-hidden="true";
    {...props}
    />;
    );

}

} // namespace elizaos
