#include "response.hpp"
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

struct AnimatedResponseProps {
    std::string children;
    std::optional<std::string> className;
    std::optional<bool> shouldAnimate;
    std::optional<std::string> messageId;
    std::optional<double> maxDurationMs;
    std::optional<() => void> onTextUpdate;
};


const AnimatedResponse: React.FC<AnimatedResponseProps> = ({
  children,
  className,
  shouldAnimate = false,
  messageId,
  maxDurationMs = 10000,
  onTextUpdate,
}) => {
  const [visibleText, setVisibleText] = React.useState(shouldAnimate ? '' : children);

  React.useEffect(() => {
    if (!shouldAnimate || !children.trim()) {
      setVisibleText(children);
      return;
    }

    const safeDuration = Math.max(1000, maxDurationMs);

    setVisibleText('');

    const TYPING_INTERVAL = 20;
    const totalChars = children.length;
    const totalSteps = Math.ceil(safeDuration / TYPING_INTERVAL);
    const charsPerStep = Math.max(1, Math.ceil(totalChars / totalSteps));

    let visibleCharCount = 0;
    const interval = setInterval(() => {
      visibleCharCount += charsPerStep;
      if (visibleCharCount >= totalChars) {
        setVisibleText(children);
        clearInterval(interval);
      } else {
        setVisibleText(children.slice(0, visibleCharCount));
      }
      // Notify parent that text was updated so it can handle scrolling
      onTextUpdate?.();
    }, TYPING_INTERVAL);

    return () => clearInterval(interval);
  }, [children, shouldAnimate, messageId, maxDurationMs, onTextUpdate]);

  return (
    <Response className={className}>
      {visibleText}
    </Response>
  );
};


} // namespace elizaos
