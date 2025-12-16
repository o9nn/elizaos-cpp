#include ".lib/base-client.hpp"
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
 * Service for JWT authentication endpoints
 */
class AuthService extends BaseApiClient {
  /**
   * Login with email and get JWT token
   * 
   * @param request Login credentials
   * @returns JWT token and user ID
   */
  async login(request: LoginRequest): Promise<LoginResponse> {
    const response = await this.post<LoginResponse>('/api/auth/login', request);
    return response;
  }
  
  /**
   * Refresh authentication token
   * Extends token expiration without requiring re-authentication
   * 
   * @returns New JWT token with extended expiration
   */
  
  /**
   * Get current authenticated user info
   * Useful for validating tokens and getting user details
   * 
   * @returns Current user information
   */


} // namespace elizaos
