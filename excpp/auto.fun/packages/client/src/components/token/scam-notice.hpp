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

default : { isHidden?: boolean }) {
  if (!isHidden) return null;
  return (
    <div
      className=" border-l-4 border-red-500 text-red-700 p-4 rounded-md mb-4 -mt-8"
      role="alert"
    >
      <div className="flex items-center">
        <AlertTriangle className="w-6 h-6 mr-2 flex-shrink-0" />
        <span className="font-semibold text-red-500">High-Risk Warning</span>
      </div>
      <p className="mt-2 text-sm">
        This token has been flagged as potentially fraudulent or part of a scam.
        Trading it carries a <strong>very high risk of loss</strong>. Proceed
        with extreme caution, and make sure to do your own thorough research
        before buying.
      </p>
    </div>
  );
}

} // namespace elizaos
