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
//import "./config.ts"; // Add this line first

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
{ default as knowledge } from "./knowledge.ts";
* from "./ragknowledge.ts";
* from "./utils.ts";
*/

// This is the entrypoint for the core-plugin-v1 package
// It exports everything needed for v1 plugin compatibility

// Core types
* from './types.ts';

// Adapters created for v1 -> v2 compatibility
// Export only the adapter functions and V1 types to avoid conflicts
{ fromV2State, toV2State, State } from './state.ts';

{ asUUID, generateUuidFromString } from './uuid.ts';

{
  fromV2ActionExample,
  toV2ActionExample,
  ActionExample,
  convertContentToV1,
  convertContentToV2,
} from './actionExample.ts';

{ fromV2Provider, toV2Provider, Provider } from './provider.ts';

{
  createTemplateFunction,
  processTemplate,
  getTemplateValues,
  TemplateType,
} from './templates.ts';

// Existing exports
* from './messages.ts';
* from './posts.ts';
* from './runtime.ts';

// Action/Handler adapters
{ fromV2Action, toV2Action, Action, Handler, Validator } from './action.ts';

// Database adapters
{ fromV2DatabaseAdapter, toV2DatabaseAdapter, IDatabaseAdapter } from './database.ts';

// Knowledge/Memory adapters
{
  knowledgeItemToMemory,
  memoryToKnowledgeItem,
  ragKnowledgeToMemory,
  memoryToRagKnowledge,
  knowledgeItemsToMemories,
  memoriesToKnowledgeItems,
  ragKnowledgeItemsToMemories,
  memoriesToRagKnowledgeItems,
  adaptKnowledgeSearchParams,
  filterKnowledgeByScope,
  createKnowledgeMetadata,
  isSharedKnowledge,
  KNOWLEDGE_TABLE_NAMES,
} from './knowledge.ts';

// Relationship adapters
{
  fromV2Relationship,
  toV2Relationship,
  fromV2Relationships,
  toV2Relationships,
  fromV2RelationshipEnhanced,
  toV2RelationshipEnhanced,
  createV1Relationship,
  areRelationshipsEquivalent,
  filterRelationshipsByStatus,
  getRelationshipsForUser,
  tagsToStatus,
  statusToTags,
  RELATIONSHIP_STATUSES,
  Relationship,
} from './relationship.ts';

} // namespace elizaos
