#include "maintenance-provider.hpp"
#include "sol-price-provider.hpp"
#include "tos-provider.hpp"
#include "wallet.hpp"
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
import "react-tooltip/dist/react-tooltip.css";
;
;
;
;

: PropsWithChildren) {
  return (
    <Fragment>
      <TosProvider />
      <MainentenaceProvider />
      <Wallet>
        <SolPriceProvider>{children}</SolPriceProvider>
      </Wallet>
    </Fragment>
  );
}

} // namespace elizaos
