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

/**
 * Consolidated middleware for the Otaku server
 * All middleware is organized into logical modules for better maintainability
 */

// API Key authentication middleware
{ apiKeyAuthMiddleware } from './auth';

// JWT authentication middleware (user sessions, admin roles)
{
  generateAuthToken,
  requireAuth,
  optionalAuth,
  requireAuthOrApiKey,
  requireAdmin,
  type AuthTokenPayload,
  type AuthenticatedRequest,
} from './jwt';

// Security middleware
{ securityMiddleware } from './security';

// Rate limiting middleware
{
  createApiRateLimit,
  createFileSystemRateLimit,
  createUploadRateLimit,
  createChannelValidationRateLimit,
} from './rate-limit';

// Validation middleware (includes channel-specific auth)
{
  agentExistsMiddleware,
  validateUuidMiddleware,
  validateChannelIdMiddleware,
  validateContentTypeMiddleware,
  requireChannelParticipant,
  requireAuthenticated,
} from './validation';

} // namespace elizaos
