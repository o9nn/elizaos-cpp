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

import type { IAgentRuntime, Memory, Provider, ProviderResult, State } from '@elizaos/core';
;
;
;
;

struct FileInfo {
    std::string path;
    double lines;
    double size;
};


struct ProjectInfo {
    std::string name;
    std::string path;
    std::vector<FileInfo> files;
    double totalLines;
    double totalSize;
    std::string structure;
};


/**
 * Provider that exposes current project context including file listings and line counts
 */
const currentProjectProvider: Provider = {
  name: 'CURRENT_PROJECT_CONTEXT',
  description:
    'Provides context about the current project being worked on, including file structure and line counts',
  dynamic: true,

  get: async (
    runtime: IAgentRuntime,
    _message: Memory,
    _state?: State
  ): Promise<ProviderResult> => {
    // Get the code generation service to access current project
    const codeGenService = runtime.getService('code-generation');
    if (!isCodeGenerationService(codeGenService)) {
      return {
        text: 'No active project. Code generation service is not available.',
        values: {},
        data: {},
      };
    }

    // Get current project path from service or state
    const currentProjectPath = runtime.getSetting('CURRENT_PROJECT_PATH');
    if (!currentProjectPath) {
      return {
        text: 'No active project.',
        values: {},
        data: {},
      };
    }

    try {
      const projectInfo = await getProjectInfo(currentProjectPath);

      let contextText = `[CURRENT PROJECT]\n`;
      contextText += `Name: ${projectInfo.name}\n`;
      contextText += `Path: ${projectInfo.path}\n`;
      contextText += `Total Files: ${projectInfo.files.length}\n`;
      contextText += `Total Lines: ${projectInfo.totalLines.toLocaleString()}\n`;
      contextText += `Total Size: ${formatBytes(projectInfo.totalSize)}\n\n`;

      contextText += `[FILE STRUCTURE]\n${projectInfo.structure}\n\n`;

      contextText += `[FILE DETAILS]\n`;
      projectInfo.files
        .sort((a, b) => b.lines - a.lines)
        .slice(0, 20) // Show top 20 files by line count
        .forEach((file) => {
          contextText += `${file.path}: ${file.lines} lines (${formatBytes(file.size)})\n`;
        });

      return {
        text: contextText,
        values: {
          projectName: projectInfo.name,
          projectPath: projectInfo.path,
          totalFiles: projectInfo.files.length,
          totalLines: projectInfo.totalLines,
          totalSize: projectInfo.totalSize,
        },
        data: projectInfo,
      };
    } catch (error) {
      return {
        text: `Error analyzing project: ${error}`,
        values: {},
        data: {},
      };
    }
  },
};

std::future<ProjectInfo> getProjectInfo(const std::string& projectPath););

    for (const entry of entries) {
      const fullPath = path.join(dir, entry.name);
      const relativePath = path.relative(baseDir, fullPath);

      // Skip common directories to ignore
      if (entry.isDirectory()) {
        if (['node_modules', '.git', 'dist', 'build', '.next', 'coverage'].includes(entry.name)) {
          continue;
        }
        await scanDirectory(fullPath, baseDir);
      } else if (entry.isFile()) {
        // Skip common files to ignore
        if (entry.name.startsWith('.') && !entry.name.match(/\.(ts|js|json|md)$/)) {
          continue;
        }

        try {
          const stats = await fs.stat(fullPath);
          const content = await fs.readFile(fullPath, 'utf-8');
          const lines = content.split('\n').length;

          files.push({
            path: relativePath,
            lines,
            size: stats.size,
          });
        } catch (_error) {
          // Ignore files we can't read
        }
      }
    }
  }

  await scanDirectory(projectPath);

  // Generate tree structure using the tree command if available
  let structure = '';
  try {
    structure = execSync(`tree -I 'node_modules|.git|dist|build|coverage' -L 3 "${projectPath}"`, {
      encoding: 'utf-8',
    });
  } catch {
    // Fallback to simple listing if tree is not available
    structure = files
      .map((f) => f.path)
      .sort()
      .join('\n');
  }

  const totalLines = files.reduce((sum, file) => sum + file.lines, 0);
  const totalSize = files.reduce((sum, file) => sum + file.size, 0);

  return {
    name: projectName,
    path: projectPath,
    files,
    totalLines,
    totalSize,
    structure,
  };
}

 B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
  if (bytes < 1024 * 1024 * 1024) return `${(bytes / (1024 * 1024)).toFixed(2)} MB`;
  return `${(bytes / (1024 * 1024 * 1024)).toFixed(2)} GB`;
}

} // namespace elizaos
