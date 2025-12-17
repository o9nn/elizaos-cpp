#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> messageReply(auto runtime, auto message, auto reply, auto responses);

void takeItPrivate(auto runtime, auto message, auto reply);

} // namespace elizaos
