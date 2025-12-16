#include "types.hpp"
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
import type { MinimatchOptions } from "minimatch";
;

/**
 * Escapes special characters in a string for use in a regular expression.
 *
 * @param str The string to escape.
 * @returns The escaped string.
 */
()|[\]\\]/g, "\\$&"); // $& means the whole matched string
}

/**
 * Performs simple string matching.
 *
 * @param pattern The string pattern to search for.
 * @param content The content string to search within.
 * @param options Matching options.
 * @returns True if the pattern is found in the content according to options, false otherwise.
 */
\\b`,
        caseSensitive ? "" : "i",
      );
      return wordBoundaryRegex.test(processedContent);
    } catch (err) {
      console.error(
        `Error creating word boundary regex for pattern "${pattern}":`,
        err,
      );
      return false; // Treat regex error as no match
    }
  } else {
    // Simple substring check
    return processedContent.includes(processedPattern);
  }
}

/**
 * Performs regular expression matching.
 *
 * @param pattern The regex pattern string.
 * @param content The content string to test against the regex.
 * @param options Matching options (caseSensitive, wordBoundary).
 * @returns True if the content matches the regex according to options, false otherwise.
 */
\\b`;
  }

  try {
    const regex = new RegExp(finalPattern, flags);
    return regex.test(content);
  } catch (err) {
    const errorMessage = err instanceof Error ? err.message : String(err);
    console.error(
      `Invalid regex pattern "${pattern}" (final: "${finalPattern}"): ${errorMessage}`,
    );
    return false; // Treat invalid regex as no match
  }
}

/**
 * Performs glob pattern matching (typically for file paths).
 *
 * @param pattern The glob pattern string.
 * @param content The content string (usually a file path) to match against the glob.
 * @param options Matching options (caseSensitive). Word boundary is ignored.
 * @returns True if the content matches the glob pattern according to options, false otherwise.
 */
"`,
    );
  }

  const matchOptions: MinimatchOptions = {
    nocase: !caseSensitive,
    matchBase: false, // Require full path match by default
    dot: true, // Match dotfiles
    noglobstar: false, // Enable ** for matching across directories
    nonegate: true, // Disable negation in glob pattern itself (handled separately)
  };

  try {
    // Content should already be preprocessed (normalized path separators)
    return minimatch(content, pattern, matchOptions);
  } catch (err: unknown) {
    console.error(
      `Error matching glob pattern "${pattern}" against "${content}":`,
      err,
    );
    return false; // Treat glob error as no match
  }
}

} // namespace elizaos
