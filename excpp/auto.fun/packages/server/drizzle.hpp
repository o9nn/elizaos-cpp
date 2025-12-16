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
;

dotenv.config({ path: '../../.env' });

default defineConfig({
   out: './drizzle',
   schema: './src/db.ts',
   dialect: 'postgresql',
   migrations: {
      schema: 'public',
   },
   dbCredentials: {
      url: process.env.DATABASE_URL || '',
   },
});

} // namespace elizaos
