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

default : { isHidden?: boolean }) {
  if (!isHidden) return null;
  return (
    <Fragment>
      <Tooltip anchorSelect="#verified">
        <span>Scam</span>
      </Tooltip>

      <img src="/warning.svg" id="verified" className="size-5 select-none" />
    </Fragment>
  );
}

} // namespace elizaos
