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

{ createDatabaseConnection, getDatabase, closeDatabase } from './connection';
{ runMigrations } from './migrations';
{ MessageDatabaseAdapter } from './adapter';
* from './schema';

} // namespace elizaos
