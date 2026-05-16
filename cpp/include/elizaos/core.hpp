#pragma once

// This header is a compatibility forwarding wrapper.
// The authoritative public ElizaOS C++ API lives under include/elizaos/.
// Keeping this shim prevents cpp/include from drifting when legacy targets
// still add cpp/include ahead of the root include directory.
#include "../../../include/elizaos/core.hpp"
