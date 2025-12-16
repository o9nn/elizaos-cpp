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

// ***********************************************************
// This file is processed and loaded automatically before your test files.
// You can change the location of this file or turn off processing using the
// 'supportFile' config option.
// ***********************************************************

// Import commands.js using ES2015 syntax:
import './commands';

// Import Testing Library Cypress commands
import '@testing-library/cypress/add-commands';

// Import styles
import '../../src/frontend/index.css';

// Add custom TypeScript types
declare global {
  namespace Cypress {
    struct Chainable {
};

  }
}

// Import React mount function
;

// Make mount available globally
Cypress.Commands.add('mount', mount);

} // namespace elizaos
