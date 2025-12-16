#include "elizaos/core.hpp"
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

// Define the hello world table schema
const helloWorldTable = pgTable('hello_world', {
  id: uuid('id').defaultRandom().primaryKey(),
  message: varchar('message', { length: 255 }).notNull(),
  author: varchar('author', { length: 100 }),
  createdAt: timestamp('created_at').defaultNow().notNull(),
});

// Define another test table to verify multiple tables work
const greetingsTable = pgTable('greetings', {
  id: uuid('id').defaultRandom().primaryKey(),
  greeting: varchar('greeting', { length: 100 }).notNull(),
  language: varchar('language', { length: 20 }).notNull().default('en'),
  isActive: varchar('is_active', { length: 10 }).notNull().default('true'),
  updatedAt: timestamp('updated_at').defaultNow().notNull(),
});

// Export the schema
const helloWorldSchema = {
  helloWorldTable,
  greetingsTable,
};

// Create the test plugin
const helloWorldPlugin: Plugin = {
  name: 'test-hello-world',
  description: 'Test plugin for dynamic migration testing',
  schema: helloWorldSchema,
  priority: 100, // Load after SQL plugin
  dependencies: ['@elizaos/plugin-sql'],

  init: async (_config, _runtime) => {
    console.log('Hello World Plugin initialized!');
  },
};

default helloWorldPlugin;

} // namespace elizaos
