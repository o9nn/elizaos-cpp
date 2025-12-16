#include "button.hpp"
#include "holders-table.hpp"
#include "swaps-table.hpp"
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
;
;

default : { token: IToken }) {
  const [mode, setMode] = useState<"transactions" | "holders">("transactions");

  return (
    <div className="md:overflow-x-hidden xs:max-w-fit md:max-w-full">
      <div className="flex items-center justify-between py-3">
        <div className="flex items-center">
          <Button
            size="small"
            variant={mode === "transactions" ? "primary" : "outline"}
            onClick={() => setMode("transactions")}
          >
            Trades
          </Button>
          <Button
            size="small"
            variant={mode === "holders" ? "primary" : "outline"}
            onClick={() => setMode("holders")}
          >
            Holders
          </Button>
        </div>
      </div>
      {mode === "transactions" ? (
        <SwapsTable token={token} />
      ) : (
        <HoldersTable token={token} />
      )}
    </div>
  );
}

} // namespace elizaos
