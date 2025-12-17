#include "...utils/entity.hpp"
#include ".services/cdp.service.hpp"
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



  // Optional chain parameter - if not provided, fetches all chains
  
      // Check if CDP service is available

      // Read parameters from state (extracted by multiStepDecisionTemplate)
      
      // Extract chain parameter if provided
      
      // Store input parameters for return

      // Validate chain parameter if provided

      // Get entity information for context
          
          // Try to get displayName from agent entity first
          
          // If not found, try to get the actual user entity (via author_id) which has the displayName
                // Use user entity ID for consistency
          
          // Final fallback to agent entity names

      // Get CDP service

      // Fetch comprehensive wallet info (always fresh data)

      // Pass wallet address to avoid CDP account lookup (prevents "account not initialized" errors)

      // Format the response

      // Token summary
        
        // Group tokens by chain

          // Sort by USD value (highest first) and show top 5 per chain

      // NFT summary
      
        // Group NFTs by chain

          // Show first 3 NFTs per chain


} // namespace elizaos
