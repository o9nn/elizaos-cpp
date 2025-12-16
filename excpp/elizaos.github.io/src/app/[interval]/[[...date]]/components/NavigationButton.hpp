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

struct NavigationButtonProps {
    std::string href;
    "prev" | "next" direction;
    bool isVisible;
};


: NavigationButtonProps) {
  const Icon = direction === "prev" ? ChevronLeft : ChevronRight;
  const label = direction === "prev" ? "Previous" : "Next";

  return (
    <>
      {/* Desktop */}
      <Button
        variant="outline"
        asChild
        className={cn("hidden items-center sm:flex", !isVisible && "invisible")}
      >
        <Link href={href}>
          {direction === "prev" && <Icon className="mr-2 h-4 w-4" />}
          <span>{label}</span>
          {direction === "next" && <Icon className="ml-2 h-4 w-4" />}
        </Link>
      </Button>

      {/* Mobile */}
      <Button
        variant="outline"
        size="icon"
        asChild
        className={cn("sm:hidden", !isVisible && "invisible")}
      >
        <Link href={href}>
          <Icon className="h-4 w-4" />
          <span className="sr-only">{label}</span>
        </Link>
      </Button>
    </>
  );
}

} // namespace elizaos
