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

const directionMap = {
  up: "clip-path-triangle-up",
  down: "clip-path-triangle-down",
  left: "clip-path-triangle-left",
  right: "clip-path-triangle-right",
};

const Triangle = ({
  size = "size-4",
  color = "bg-black",
  direction = "up",
}) => {
  return (
    <div
      // @ts-ignore
      className={clsx(size, color, "triangle", directionMap[direction])}
    ></div>
  );
};

default Triangle;

} // namespace elizaos
