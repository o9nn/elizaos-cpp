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

struct LogoProps {
    std::optional<double> width;
    std::optional<double> height;
    std::optional<std::string> className;
};


: LogoProps) {
  return (
    <div className={clsx(["select-none", className])}>
      <Image
        src={logo}
        alt="Eliza Logo"
        width={width}
        height={height}
        priority
      />
    </div>
  );
}

} // namespace elizaos
