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

import type { Agent } from '@elizaos/core';

struct ExportResult {
    bool success;
    std::optional<std::string> filename;
    std::optional<std::string> error;
};


struct ToastFunction {
};


/**
 * Sanitizes a filename by replacing non-alphanumeric characters with dashes
 * and cleaning up multiple consecutive dashes
 */


/**
 * Converts an agent to character JSON data, excluding sensitive information
 */
 : undefined,
    style: agent.style,
  };

  // Remove secrets from settings if they exist
  if (characterData.settings && 'secrets' in characterData.settings) {
    const { secrets, ...settingsWithoutSecrets } = characterData.settings;
    characterData.settings = settingsWithoutSecrets;
  }

  // Remove undefined/null fields to keep JSON clean
  return Object.fromEntries(
    Object.entries(characterData).filter(([_, value]) => value !== undefined && value !== null)
  );
}

/**
 * Generates a filename for the exported character
 */
-${date}.json`;
}

/**
 * Creates and triggers a download of a JSON file
 */
);
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = filename;
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
  URL.revokeObjectURL(url);
}

/**
 * Exports an agent's character data as a JSON file download
 * @param agent - The agent containing the character data to * @param toast - Optional toast 's character data has been downloaded as ${filename}`,
      });
    }

    return { success: true, filename };
  } catch (error) {
    console.error('Failed to character:', error);

    // Error notification
    if (toast) {
      toast({
        title: 'Export Failed',
        description: 'Failed to character data. Please try again.',
        variant: 'destructive',
      });
    }

    return {
      success: false,
      error: error instanceof Error ? error.message : 'Unknown error',
    };
  }
}

} // namespace elizaos
