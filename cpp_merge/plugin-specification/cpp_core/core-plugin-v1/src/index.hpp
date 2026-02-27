#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// this just imported dotenv, settings will handle wrapping this

/*
* from "./actions.ts";
* from "./context.ts";
* from "./database.ts";
* from "./embedding.ts";
* from "./evaluators.ts";
* from "./generation.ts";
* from "./goals.ts";
* from "./memory.ts";
*/
* from './messages.ts';
//* from "./models.ts";
* from './posts.ts';
//* from "./providers.ts";
//* from "./relationships.ts";
* from './runtime.ts';
/*
* from "./settings.ts";
* from "./types.ts";
* from "./logger.ts";
* from "./parsing.ts";
* from "./uuid.ts";
* from "./environment.ts";
* from "./cache.ts";
* from "./ragknowledge.ts";
* from "./utils.ts";
*/

// This is the entrypoint for the core-plugin-v1 package
// It exports everything needed for v1 plugin compatibility

// Core types
* from './types.ts';

// Adapters created for v1 -> v2 compatibility
// Export only the adapter functions and V1 types to avoid conflicts

// Existing exports
* from './messages.ts';
* from './posts.ts';
* from './runtime.ts';

// Action/Handler adapters

// Database adapters

// Knowledge/Memory adapters

// Relationship adapters

} // namespace elizaos
