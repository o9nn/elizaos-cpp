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

struct DeleteButtonProps {
    () => void onClick;
};


const DeleteButton = ({ onClick }: DeleteButtonProps) => {
  return (
    <Tooltip>
      <TooltipTrigger asChild>
        <Button onClick={onClick} variant="ghost" size="icon" className="text-muted-foreground">
          <Trash2 className="size-3" />
        </Button>
      </TooltipTrigger>
      <TooltipContent side="bottom">
        <p>Delete</p>
      </TooltipContent>
    </Tooltip>
  );
};

default DeleteButton;

} // namespace elizaos
