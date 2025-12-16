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

;

default defineConfig({
  out: "./drizzle",
  schema: "./src/lib/data/schema.ts",
  dialect: "sqlite",
  dbCredentials: {
    url: "file:data/db.sqlite",
  },
  verbose: true,
});

} // namespace elizaos
