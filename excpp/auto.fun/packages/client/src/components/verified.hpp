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

default : { isVerified?: boolean }) {
  if (!isVerified) return null;
  return (
    <Fragment>
      <Tooltip anchorSelect="#verified">
        <span>Verified</span>
      </Tooltip>

      <img
        src="/verified.svg"
        id="verified"
        className="size-4 lg:size-5 select-none"
      />
    </Fragment>
  );
}

} // namespace elizaos
