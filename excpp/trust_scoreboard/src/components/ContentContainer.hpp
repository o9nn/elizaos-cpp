#include "ContentContainer.module.css.hpp"
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




struct Props {
    React.ReactNode children;
};


const ContentContainer: FC<{ children: ReactNode }> = ({ children }) => {
  return (
    <div className={`container mx-auto flex-1 ${styles.container}`}>
      <div className="flex w-full flex-col justify-center">{children}</div>
    </div>
  )
}

} // namespace elizaos
