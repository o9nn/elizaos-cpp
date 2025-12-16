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
 * Utility to categorize a work item (PR title or commit message)
 *
 * @param text - The PR title or commit message to categorize
 * @returns The category of the work item
 */


  // Bug fix detection
  if (
    lowercaseText.startsWith("fix") ||
    lowercaseText.includes("fix") ||
    lowercaseText.includes("bug")
  ) {
    return "bugfix";
  }

  // Documentation detection
  if (lowercaseText.startsWith("docs") || lowercaseText.includes("document")) {
    return "docs";
  }

  // Refactoring detection
  if (
    lowercaseText.startsWith("refactor") ||
    lowercaseText.includes("refactor") ||
    lowercaseText.includes("clean") ||
    lowercaseText.includes("cleanup")
  ) {
    return "refactor";
  }

  // Test related detection
  if (lowercaseText.startsWith("test") || lowercaseText.includes("test")) {
    return "tests";
  }

  // Default category
  return "other";
}

/**
 * Utility to extract area from a file path
 */


  let area = parts[0];

  if (area === "packages") {
    area = `${parts[1]}`;
  } else {
    area = `${area}/${parts[1]}`;
  }

  return area;
}

/**
 * Check if a file is a common root configuration file that should be ignored
 */


using WorkItemType = std::variant<, "feature", "bugfix", "refactor", "docs", "tests", "other">;

/**
 * Utility to build a map of focus areas from files
 */
[],
): Map<string, number> {
  const areaMap = new Map<string, number>();

  files.forEach((file) => {
    // Use path or filename depending on which is available
    const filePath = file.path || file.filename || "";
    if (!filePath) return;

    const area = extractAreaFromPath(filePath);
    // Skip null areas (root files or ignored config files)
    if (!area) return;

    const currentCount = areaMap.get(area) || 0;
    areaMap.set(area, currentCount + 1);
  });

  return areaMap;
}

} // namespace elizaos
