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

// ***********************************************
// This file is where you can create custom Cypress commands
// and overwrite existing commands.
//
// For comprehensive examples, visit:
// https://on.cypress.io/custom-commands
// ***********************************************

// Example custom command
// Cypress.Commands.add('login', (email, password) => { ... })

// Extend Window interface
declare global {
  struct Window {
    std::optional<{> ELIZA_CONFIG;
    std::string agentId;
    std::string apiBase;
};

}

// Custom command to check if element is in dark mode
Cypress.Commands.add('shouldBeDarkMode', () => {
  cy.get('html').should('have.class', 'dark');
});

// Custom command to set ELIZA_CONFIG
Cypress.Commands.add('setElizaConfig', (config) => {
  cy.window().then((win) => {
    win.ELIZA_CONFIG = config;
  });
});

// TypeScript definitions
declare global {
  namespace Cypress {
    struct Chainable {
};

  }
}

{};

} // namespace elizaos
