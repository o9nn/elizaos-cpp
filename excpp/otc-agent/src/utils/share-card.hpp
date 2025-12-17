#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Theme colors for canvas drawing

// Load and cache the business image

std::future<void*> loadBusinessImage();

// Generate a share image for a completed deal (purchase)

  // Background

  // Accent gradient
  // const grad = ctx.createLinearGradient(0, 0, width, height);
  // grad.addColorStop(0, "#00ff87");
  // grad.addColorStop(1, "#00a3ff");
  // ctx.fillStyle = grad;
  // ctx.globalAlpha = 0.12;
  // ctx.fillRect(0, 0, width, height);
  // ctx.globalAlpha = 1;

  // Load and draw business image at full height, offset 20% off right edge
  // Position so 20% extends beyond the right edge

  // Title

  // Main numbers - split amount and ticker for different colors

  // Measure the number text to position the ticker next to it

  // Discount in brand orange


} // namespace elizaos
