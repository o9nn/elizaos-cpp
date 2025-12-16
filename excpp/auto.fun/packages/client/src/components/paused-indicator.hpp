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

default : { show: boolean }) {
  return (
    <div
      className={twMerge([
        show ? "opacity-100" : "opacity-0",
        "flex items-center transition-opacity duration-200 p-1 px-1 gap-0.5 bg-autofun-background-card border text-autofun-text-highlight",
      ])}
    >
      <Pause />
    </div>
  );
}

} // namespace elizaos
