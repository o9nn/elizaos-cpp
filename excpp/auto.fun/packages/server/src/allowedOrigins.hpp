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

const envAllowedOrigins = process.env.ALLOWED_ORIGINS
  ? process.env.ALLOWED_ORIGINS.split(",").map((origin) => origin.trim())
  : null;

console.log("envAllowedOrigins", envAllowedOrigins);

const allowedOrigins = envAllowedOrigins || ["http://localhost:3000"];


} // namespace elizaos
