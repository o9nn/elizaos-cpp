#include "ui/tooltip.hpp"
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

const CopyButton = ({ text }: { text: string }) => {
  const [copied, setCopied] = useState(false);

  const handleCopy = () => {
    navigator.clipboard.writeText(text).then(() => {
      setCopied(true);
      setTimeout(() => setCopied(false), 2000); // Reset after 2 seconds
    });
  };

  return (
    <Tooltip>
      <TooltipTrigger asChild>
        <Button
          onClick={handleCopy}
          variant="ghost"
          size="icon"
          className="flex items-center space-x-2 text-muted-foreground"
        >
          {copied ? <Check className="size-3" /> : <Copy className="size-3" />}
        </Button>
      </TooltipTrigger>
      <TooltipContent side="bottom">
        <p>Copy</p>
      </TooltipContent>
    </Tooltip>
  );
};

default CopyButton;

} // namespace elizaos
