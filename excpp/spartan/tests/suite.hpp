#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Test suite for the Investment Manager agent
 * Tests portfolio management and financial operations
 */
class SpartanTestSuite {
public:
    SpartanTestSuite(double completionTimeout = 15000);

private:
    std::any scenarioService_;
    double completionTimeout_;
};


} // namespace elizaos
