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

// this just imported dotenv, settings will handle wrapping this

* from './messages';
//* from "./models";
* from './posts';
//* from "./providers";
//* from "./relationships";
* from './runtime';

/*
Core exports for Eliza SDK v1. This is the primary entrypoint for the SDK.
These are the v1 specs, which are a shim over the v2 specs for backwards compatibility.
*/

// Core types
* from './types';

// Adapters created for v1 -> v2 compatibility
// Export only the adapter functions and V1 types to avoid conflicts
{ fromV2State, toV2State } from './state';
type { State } from './state';

{ asUUID, generateUuidFromString } from './uuid';

{
  fromV2ActionExample,
  toV2ActionExample,
  convertContentToV1,
  convertContentToV2,
} from './actionExample';

type { ActionExample } from './actionExample';

{ fromV2Provider, toV2Provider } from './provider';

type { Provider } from './provider';

{ createTemplateFunction, processTemplate, getTemplateValues } from './templates';

type { TemplateType } from './templates';

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
