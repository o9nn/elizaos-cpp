#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/// <reference types="cypress" />
/// <reference types="@testing-library/cypress" />

// ***********************************************************
// This example support/component.ts is processed and
// loaded automatically before your component test files.

// Import commands.js using ES2015 syntax:

// 
// 

// Import polyfills

// @ts-ignore

// Mock environment variables to prevent errors

// Add Buffer polyfill globally

// Fix for Radix UI components - they need proper React import

// Ensure React is properly available globally

// Remove duplicate declarations - they're already in types.d.ts

// Create a default query client for tests

// Mount with all common providers
void mountWithProviders(ReactNode component, auto options = {});

// Mount with just router (for simpler components)
void mountWithRouter(ReactNode component, auto options = {});

// Mount specifically for Radix UI components with DirectionProvider
void mountRadix(ReactNode component, auto options = {});

// Add commands

// Example of how to use:
// cy.mount(<Button onClick={handleClick}>Click me</Button>);
// cy.mountWithRouter(<Navigation />);
// cy.mountRadix(<DropdownMenu />);

} // namespace elizaos
