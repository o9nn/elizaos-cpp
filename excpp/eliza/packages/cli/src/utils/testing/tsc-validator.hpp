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

struct TypeCheckResult {
    bool success;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};


std::future<TypeCheckResult> runTypeCheck(const std::string& projectPath, boolean = true strict);`],
      warnings: [],
    };
  }

  try {
    const args = ['--noEmit'];
    if (strict) {
      args.push('--strict');
    }

    const { stdout, stderr } = await execa('tsc', args, {
      cwd: projectPath,
      reject: false,
    });

    const hasErrors = stderr.includes('error TS') || stdout.includes('error TS');

    return {
      success: !hasErrors,
      errors: hasErrors ? [stderr || stdout] : [],
      warnings: stderr.includes('warning') ? [stderr] : [],
    };
  } catch (error: any) {
    logger.error('TypeScript validation failed:', error);
    return {
      success: false,
      errors: [`TypeScript validation error: ${error.message}`],
      warnings: [],
    };
  }
}

} // namespace elizaos
