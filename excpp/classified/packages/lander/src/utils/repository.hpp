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
 * Extract repository information from the current URL
 * Defaults to elizaos/eliza if not running on GitHub Pages
 */
;
  }
  
  // Check for environment variable override (useful for custom domains)
  if (typeof process !== 'undefined' && process.env?.VITE_GITHUB_REPOSITORY) {
    const [owner, repo] = process.env.VITE_GITHUB_REPOSITORY.split('/');
    if (owner && repo) {
      return { owner, repo };
    }
  }
  
  // Default fallback for local development or non-GitHub Pages hosting
  return { owner: 'elizaos', repo: 'eliza' };
}

/**
 * Get the full repository path (owner/repo)
 */
 = getRepositoryInfo();
  return `${owner}/${repo}`;
}

/**
 * Get the GitHub repository URL
 */
`;
}

/**
 * Get the GitHub releases URL
 */
/releases`;
}

/**
 * Get the GitHub API releases URL
 */
/releases`;
}
} // namespace elizaos
