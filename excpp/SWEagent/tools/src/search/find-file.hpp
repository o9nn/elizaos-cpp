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

#!/usr/bin/env node
/**
 * Find file tool
 * Find all files with a given name or pattern in a directory
 * Converted from tools/search/bin/find_file
 */

;
;
;
;
;

 not found`);
    process.exit(1);
  }

  const absDir = path.resolve(dir);

  try {
    // Use glob for pattern matching or find for exact names
    let matches: string[] = [];
    
    if (fileName.includes('*') || fileName.includes('?') || fileName.includes('[')) {
      // Use glob for patterns
      matches = glob.sync(`**/${fileName}`, {
        cwd: absDir,
        absolute: true,
        nodir: true,
        ignore: ['**/node_modules/**', '**/.git/**']
      });
    } else {
      // Use find command for exact names
      try {
        const findCmd = `find "${absDir}" -type f -name "${fileName}" 2>/dev/null`;
        const result = execSync(findCmd, { encoding: 'utf-8' });
        matches = result.trim().split('\n').filter(line => line.length > 0);
      } catch (error: unknown) {
        // find might return non-zero if no matches
        matches = [];
      }
    }

    if (matches.length === 0) {
      console.log(`No matches found for "${fileName}" in ${absDir}`);
      return;
    }

    // Print results
    console.log(`Found ${matches.length} matches for "${fileName}" in ${absDir}:`);
    matches.forEach(file => {
      // Show relative path for readability
      const relPath = path.relative(process.cwd(), file);
      console.log(relPath);
    });

  } catch (error) {
    console.error(`Error finding files: ${error}`);
    process.exit(1);
  }
}

// CLI if run directly
// CLI setup
);

  program.parse(process.argv);
}

// Run CLI if called directly or from bin script
if (require.main === module || require.main?.filename?.endsWith('/bin/find_file')) {
  setupCLI();
}

{ findFile };

} // namespace elizaos
