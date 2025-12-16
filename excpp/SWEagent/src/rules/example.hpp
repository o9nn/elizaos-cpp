#include "index.hpp"
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
 * Example usage of the SWE-agent rules module
 */

;

// Example 1: Validate a Python file
async 

// Example 2: Validate a TypeScript file
async 
`;

  const validator = new TypeScriptValidator();
  const result = validator.validate(tsCode, 'example.ts');

  console.log('Valid:', result.valid);
  console.log('Violations:', result.violations);
  console.log('Warnings:', result.warnings);
}

// Example 3: Get project structure information
: ${entry.description}`);
  });

  console.log('\nInspectors:');
  PROJECT_STRUCTURE.inspectors.forEach((inspector) => {
    console.log(`  - ${inspector.name} (${inspector.type}): ${inspector.path}`);
  });
}

// Example 4: Get applicable rules for different file types
: ${rule.rule}`);
  });

  console.log('\nTypeScript rules:');
  const tsRules = getApplicableRules('example.ts');
  tsRules.forEach((rule) => {
    console.log(`  - ${rule.id}: ${rule.rule}`);
  });
}

// Example 5: Export rules to Cursor IDE format
 (${cursorRules[filename].length} bytes)`);
  });
}

// Run all examples
async 

// Execute if run directly
if (require.main === module) {
  runExamples().catch(console.error);
}

{ runExamples };

} // namespace elizaos
