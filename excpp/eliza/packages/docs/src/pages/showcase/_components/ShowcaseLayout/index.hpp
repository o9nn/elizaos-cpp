#include "styles.module.css.hpp"
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

// src/pages/showcase/_components/ShowcaseLayout/index.tsx
;
;
;

default : {
  children: React.ReactNode;
  selectedTags: TagType[];
  toggleTag: (tag: TagType) => void;
}): JSX.Element {
  return (
    <div className={styles.layout}>
      <div className={styles.content}>{children}</div>
    </div>
  );
}

} // namespace elizaos
