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

default : {
  className?: string;
  isFullscreen?: boolean;
}) {
  return (
    <div
      className={twMerge([
        "flex items-center justify-center h-[50vh]",
        isFullscreen
          ? "fixed top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2"
          : "",
        className ? className : "",
      ])}
    >
      <img
        className="h-[60px] w-auto select-none animate-wiggle animate-infinite animate-duration-[400ms] animate-ease-linear"
        alt="logo"
        src="/dice.svg"
      />
    </div>
  );
}

} // namespace elizaos
