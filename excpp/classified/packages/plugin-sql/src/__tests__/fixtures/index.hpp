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

import type { Plugin, Character } from '@elizaos/core';
;
;

const mockCharacter: Character = {
  name: 'Test Agent',
  bio: ['A test agent for running tests.'],
};

const helloWorldSchema = {
  helloWorldTable: pgTable('hello_world', {
    id: uuid('id').primaryKey().defaultRandom(),
    message: varchar('message', { length: 255 }).notNull(),
    author: varchar('author', { length: 100 }),
    createdAt: timestamp('created_at')
      .default(sql`now()`)
      .notNull(),
  }),
  greetingsTable: pgTable('greetings', {
    id: uuid('id').primaryKey().defaultRandom(),
    greeting: varchar('greeting', { length: 100 }).notNull(),
    language: varchar('language', { length: 20 }).default('en').notNull(),
    isActive: varchar('is_active', { length: 10 }).default('true').notNull(),
    updatedAt: timestamp('updated_at')
      .default(sql`now()`)
      .notNull(),
  }),
};

const helloWorldPlugin: Plugin = {
  name: 'test-hello-world',
  description: 'A simple test plugin with a database schema.',
  schema: helloWorldSchema,
  init: async (_runtime) => {
    console.log('Hello World Plugin initialized!');
  },
};

} // namespace elizaos
