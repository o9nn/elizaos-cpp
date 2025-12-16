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

/// <reference types="cypress" />
/// <reference types="@testing-library/cypress" />
/// <reference types="@cypress/react" />

;

declare module 'process/browser' {
  const process: NodeJS.Process;
  = process;
}

// Extend Cypress types
declare global {
  namespace Cypress {
    struct Chainable {
    typeof mount mount;
    typeof mountWithRouter mountWithRouter;
    typeof mountRadix mountRadix;
};

  }
}

// Import statements to ensure types are available
;
import type { mountWithRouter, mountRadix } from './component';

{};

} // namespace elizaos
