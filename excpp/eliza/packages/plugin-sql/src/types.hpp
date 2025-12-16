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

import type { NodePgDatabase } from 'drizzle-orm/node-postgres';
import type { PgliteDatabase } from 'drizzle-orm/pglite';

/**
 * Represents a type that can be either a NodePgDatabase or a PgliteDatabase.
 */
using DrizzleDatabase = std::variant<NodePgDatabase, PgliteDatabase>;

/**
 * Interface for managing a database client.
 * @template T - The type of the database connection object.
 */
interface IDatabaseClientManager<T> {
  initialize(): Promise<void>;
  getConnection(): T;
  close(): Promise<void>;
}

} // namespace elizaos
