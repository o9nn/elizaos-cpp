#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..service.hpp"
#include "vision-autonomy.hpp"
#include "vision-basic.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Simple test runner for local e2e testing
std::future<void> runE2ETests();

// Run tests

} // namespace elizaos
