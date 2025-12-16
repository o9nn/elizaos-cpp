#include "markdown.hpp"
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

'use client';

;
;
;

struct AnimatedMarkdownProps {
    std::string children;
    std::optional<std::string> className;
    std::optional<'user' | 'agent'> variant;
    std::optional<bool> shouldAnimate;
    std::optional<std::string> messageId;
};


const AnimatedMarkdown: React.FC<AnimatedMarkdownProps> = ({
  children,
  className,
  variant = 'agent',
  shouldAnimate = false,
  messageId,
}) => {
  const [animationComplete, setAnimationComplete] = React.useState(!shouldAnimate);

  // Reset animation state when message changes
  React.useEffect(() => {
    if (shouldAnimate) {
      setAnimationComplete(false);
      // Estimate animation time based on text length (50ms per character roughly)
      const estimatedTime = Math.min(children.length * 50, 3000);
      const timer = setTimeout(() => {
        setAnimationComplete(true);
      }, estimatedTime);

      return () => clearTimeout(timer);
    } else {
      setAnimationComplete(true);
    }
  }, [children, shouldAnimate, messageId]);

  // If not animating or animation is complete, render markdown
  if (!shouldAnimate || animationComplete) {
    return (
      <Markdown className={className} variant={variant}>
        {children}
      </Markdown>
    );
  }

  // During animation, show AIWriter with plain text
  return (
    <div className={className}>
      <AIWriter>{children}</AIWriter>
    </div>
  );
};

} // namespace elizaos
