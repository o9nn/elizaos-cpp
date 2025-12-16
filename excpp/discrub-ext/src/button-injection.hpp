#include "containers/discrub-button/discrub-button.tsx.hpp"
#include "theme.ts.hpp"
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

const injectionButton = document.getElementById("button_injection");
if (injectionButton) {
  ReactDOM.createRoot(injectionButton).render(
    <ThemeProvider theme={theme}>
      <DiscrubButton />
    </ThemeProvider>
  );
}

} // namespace elizaos
