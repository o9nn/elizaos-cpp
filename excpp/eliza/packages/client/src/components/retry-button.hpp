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

struct RetryButtonProps {
    () => void onClick;
    std::optional<std::string> className;
};


default : RetryButtonProps) {
  return (
    <Tooltip>
      <TooltipTrigger asChild>
        <Button
          variant="ghost"
          size="icon"
          className={`text-muted-foreground ${className}`}
          onClick={onClick}
        >
          <RotateCcw className="size-3" />
        </Button>
      </TooltipTrigger>
      <TooltipContent side="bottom">
        <p>Retry message</p>
      </TooltipContent>
    </Tooltip>
  );
}

} // namespace elizaos
