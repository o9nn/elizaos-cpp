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

import React, { useState } from "react";
;
;
;
;

struct StatCardProps {
    std::string title;
    LucideIcon icon;
    std::optional<std::string> bgColor;
    React.ReactNode children;
    std::optional<std::string> className;
    std::optional<React.ReactNode> modalContent;
    std::optional<std::string> modalTitle;
    std::optional<std::string> modalDescription;
};


: StatCardProps) {
  const HeaderContent = (
    <div className="flex w-full items-center justify-between">
      <div className="flex items-center gap-2">
        <Icon className="h-4 w-4" /> {title}
      </div>
      {modalContent && (
        <ChevronRight className="h-4 w-4 text-muted-foreground" />
      )}
    </div>
  );

  const cardInnerContent = (
    <>
      <CardHeader className={cn(bgColor, "w-full py-4 transition-colors")}>
        <CardTitle className="text-sm font-medium">{HeaderContent}</CardTitle>
      </CardHeader>
      <CardContent className="w-full flex-grow p-4">{children}</CardContent>
    </>
  );

  if (modalContent) {
    return (
      <Dialog>
        <DialogTrigger asChild>
          <button
            className={cn(
              "group flex flex-col rounded-md border bg-card text-card-foreground shadow-sm",
              "ring-offset-background transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2",
              className,
            )}
          >
            {cardInnerContent}
          </button>
        </DialogTrigger>
        <DialogContent className="gap-0 p-0 sm:max-w-[525px]">
          <DialogHeader className="flex justify-center border-b border-border px-6 py-4 text-start">
            <DialogTitle>{modalTitle || title}</DialogTitle>
            {modalDescription && (
              <DialogDescription>{modalDescription}</DialogDescription>
            )}
          </DialogHeader>
          {modalContent}
        </DialogContent>
      </Dialog>
    );
  }

  return (
    <Card className={cn("flex flex-col overflow-hidden", className)}>
      {cardInnerContent}
    </Card>
  );
}

} // namespace elizaos
