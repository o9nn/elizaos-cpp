#include ".types.hpp"
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
import type { CreateOptions } from '../types';
;

/**
 * Project name validation schema
 */
const ProjectNameSchema = z
  .string()
  .min(1, 'Invalid project name: cannot be empty')
  .regex(
    /^[a-z0-9-_]+$/,
    'Invalid project name: must contain only lowercase letters, numbers, hyphens, and underscores'
  )
  .refine(
    (name) => !name.startsWith('-') && !name.endsWith('-'),
    'Invalid project name: cannot start or end with a hyphen'
  )
  .refine(
    (name) => !name.startsWith('_') && !name.endsWith('_'),
    'Invalid project name: cannot start or end with an underscore'
  );

/**
 * Plugin name validation schema
 */
const PluginNameSchema = z
  .string()
  .min(1, 'Plugin name cannot be empty')
  .regex(
    /^[a-z0-9-_]+$/,
    'Plugin name must contain only lowercase letters, numbers, hyphens, and underscores'
  )
  .refine(
    (name) => !name.startsWith('-') && !name.endsWith('-'),
    'Plugin name cannot start or end with a hyphen'
  )
  .refine(
    (name) => !name.startsWith('_') && !name.endsWith('_'),
    'Plugin name cannot start or end with an underscore'
  );

/**
 * Validates create command options using Zod schema
 */
 catch (error) {
    if (error instanceof z.ZodError) {
      const typeError = error.errors.find(
        (e) => e.path.includes('type') && e.code === 'invalid_enum_value'
      );
      if (typeError && 'received' in typeError) {
        const enumError = typeError as z.ZodInvalidEnumValueIssue;
        throw new Error(
          `Invalid type '${enumError.received}'. Expected: ${enumError.options?.join(', ')}`
        );
      }
    }
    throw error;
  }
}

/**
 * Validates a project name according to npm package naming rules.
 * Special case: "." is allowed for current directory projects.
 */
 {
  // Special case for current directory
  if (name === '.') {
    return { isValid: true };
  }

  try {
    ProjectNameSchema.parse(name);
    return { isValid: true };
  } catch (error) {
    if (error instanceof z.ZodError) {
      return { isValid: false, error: error.errors[0].message };
    }
    return { isValid: false, error: 'Invalid project name' };
  }
}

/**
 * Processes and validates a plugin name.
 */
 {
  try {
    // Remove common prefixes and suffixes
    let processedName = name
      .replace(/^(eliza-?|elizaos-?|plugin-?)/i, '')
      .replace(/(-?plugin|-?eliza|-?elizaos)$/i, '')
      .toLowerCase()
      .replace(/[^a-z0-9-_]/g, '-')
      .replace(/-+/g, '-')
      .replace(/^-+|-+$/g, '');

    if (!processedName) {
      return { isValid: false, error: 'Plugin name cannot be empty after processing' };
    }

    PluginNameSchema.parse(processedName);
    return { isValid: true, processedName };
  } catch (error) {
    if (error instanceof z.ZodError) {
      return { isValid: false, error: error.errors[0].message };
    }
    return { isValid: false, error: 'Invalid plugin name' };
  }
}

/**
 * Validates that the target directory is empty or doesn't exist.
 */
std::future<{ isValid: boolean; error?: string }> validateTargetDirectory(const std::string& targetDir);;
    }

    const entries = await fs.readdir(targetDir);
    if (entries.length > 0) {
      return {
        isValid: false,
        error: `Directory ${targetDir} already exists and is not empty. Please choose an empty directory or a new name.`,
      };
    }

    return { isValid: true };
  } catch (error) {
    return {
      isValid: false,
      error: `Failed to validate directory: ${error instanceof Error ? error.message : 'Unknown error'}`,
    };
  }
}

} // namespace elizaos
