#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// this just imported dotenv, settings will handle wrapping this

* from './messages';
//* from "./models";
* from './posts';
//* from "./providers";
//* from "./relationships";
* from './runtime';

/*
*/

// Core types
* from './types';

// Adapters created for v1 -> v2 compatibility
// Export only the adapter functions and V1 types to avoid conflicts

// Existing exports
* from './messages';
* from './posts';
* from './runtime';

// TODO: Implement the remaining adapters:
// - action/handler
// - database
// - knowledge / memory
// - relationships

} // namespace elizaos
