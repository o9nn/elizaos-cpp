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



/**
 * Test suite for the Investment Manager agent
 * Tests portfolio management and financial operations
 */
class SpartanTestSuite implements TestSuite {
  name = 'investment-manager';
  description = 'Tests for the investment manager agent';
  private scenarioService: any;
  private completionTimeout: number;

  constructor(completionTimeout: number = 15000) {
    this.completionTimeout = completionTimeout;
  }


} // namespace elizaos
