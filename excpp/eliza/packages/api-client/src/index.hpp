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

// Main client
{ ElizaClient } from './client';

// Base types
* from './types/base';

// Domain types
* from './types/agents';
* from './types/messaging';
* from './types/memory';
* from './types/audio';
* from './types/media';
* from './types/server';
* from './types/system';

// Services (for advanced usage)
{ AgentsService } from './services/agents';
{ MessagingService } from './services/messaging';
{ MemoryService } from './services/memory';
{ AudioService } from './services/audio';
{ MediaService } from './services/media';
{ ServerService } from './services/server';
{ SystemService } from './services/system';

// Base client and error
{ BaseApiClient, ApiError } from './lib/base-client';

} // namespace elizaos
