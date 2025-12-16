#include "..middleware.hpp"
#include ".shared/response-utils.hpp"
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
;
;
;
 = req.body;
      
      // Validate email
      if (!email || typeof email !== 'string') {
        return sendError(res, 400, 'INVALID_REQUEST', 'Email is required');
      }
      
      const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
      if (!emailRegex.test(email)) {
        return sendError(res, 400, 'INVALID_EMAIL', 'Invalid email format');
      }
      
      // Validate username
      if (!username || typeof username !== 'string') {
        return sendError(res, 400, 'INVALID_REQUEST', 'Username is required');
      }
      
      // Validate CDP userId
      if (!cdpUserId || typeof cdpUserId !== 'string') {
        return sendError(res, 400, 'INVALID_REQUEST', 'CDP userId is required');
      }
      
      // Validate UUID format (CDP uses UUIDs)
      const uuidRegex = /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i;
      if (!uuidRegex.test(cdpUserId)) {
        return sendError(res, 400, 'INVALID_CDP_USER_ID', 'CDP userId must be a valid UUID');
      }
      
      // Use CDP's userId directly (no generation needed)
      const userId = cdpUserId;
      
      // Generate JWT token with email and username
      const token = generateAuthToken(userId, email, username);
      
      logger.info(`[Auth] User authenticated: ${username} (${email}) (userId: ${userId.substring(0, 8)}...)`);
      
      return sendSuccess(res, {
        token,
        userId,
        username,
        expiresIn: '7d'
      });
    } catch (error: any) {
      logger.error('[Auth] Login error:', error);
      return sendError(res, 500, 'AUTH_ERROR', error.message);
    }
  });
  
  /**
   * POST /api/auth/refresh
   * 
   * Refreshes an existing JWT token (extends expiration)
   * 
   * Headers:
   * - Authorization: Bearer <token>
   * 
   * Response:
   * - token: string (new JWT token)
   * - userId: string
   * - expiresIn: string
   * 
   * This allows extending user sessions without requiring re-authentication
   */
  router.post('/refresh', async (req: AuthenticatedRequest, res) => {
    try {
      const authHeader = req.headers.authorization;
      
      if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return sendError(res, 401, 'UNAUTHORIZED', 'No token provided');
      }
      
      const oldToken = authHeader.substring(7);
      const JWT_SECRET = process.env.JWT_SECRET;
      
      if (!JWT_SECRET) {
        return sendError(res, 500, 'SERVER_MISCONFIGURED', 'JWT_SECRET not configured');
      }
      
      try {
        const decoded = jwt.verify(oldToken, JWT_SECRET) as any;
        
        // Issue new token with extended expiration
        const newToken = generateAuthToken(decoded.userId, decoded.email, decoded.username);
        
        logger.info(`[Auth] Token refreshed for: ${decoded.username} (userId: ${decoded.userId.substring(0, 8)}...)`);
        
        return sendSuccess(res, {
          token: newToken,
          userId: decoded.userId,
          username: decoded.username,
          expiresIn: '7d'
        });
      } catch (error: any) {
        // Token verification failed
        logger.warn(`[Auth] Token refresh failed: ${error.message}`);
        return sendError(res, 401, 'INVALID_TOKEN', 'Invalid or expired token');
      }
    } catch (error: any) {
      logger.error('[Auth] Refresh error:', error);
      return sendError(res, 500, 'REFRESH_ERROR', error.message);
    }
  });
  
  /**
   * GET /api/auth/me
   * 
   * Get current authenticated user info
   * Useful for validating tokens and getting user details
   * 
   * Headers:
   * - Authorization: Bearer <token>
   * 
   * Response:
   * - userId: string
   * - email: string
   */
  router.get('/me', async (req: AuthenticatedRequest, res) => {
    try {
      const authHeader = req.headers.authorization;
      
      if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return sendError(res, 401, 'UNAUTHORIZED', 'No token provided');
      }
      
      const token = authHeader.substring(7);
      const JWT_SECRET = process.env.JWT_SECRET;
      
      if (!JWT_SECRET) {
        return sendError(res, 500, 'SERVER_MISCONFIGURED', 'JWT_SECRET not configured');
      }
      
      try {
        const decoded = jwt.verify(token, JWT_SECRET) as any;
        
        return sendSuccess(res, {
          userId: decoded.userId,
          email: decoded.email,
          username: decoded.username
        });
      } catch (error: any) {
        return sendError(res, 401, 'INVALID_TOKEN', 'Invalid or expired token');
      }
    } catch (error: any) {
      logger.error('[Auth] Get user info error:', error);
      return sendError(res, 500, 'AUTH_ERROR', error.message);
    }
  });
  
  return router;
}


} // namespace elizaos
