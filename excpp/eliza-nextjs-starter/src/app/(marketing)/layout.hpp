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

default : Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <div className="flex flex-col min-h-dvh size-full pt-14">
      <div className="flex flex-col size-full overflow-hidden">
        <div className="flex grow flex-col size-full shrink-0">{children}</div>
      </div>
    </div>
  );
}

} // namespace elizaos
