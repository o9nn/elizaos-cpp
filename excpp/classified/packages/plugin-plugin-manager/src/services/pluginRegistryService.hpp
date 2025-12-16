#include ".types.hpp"
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
import type { ExecException } from 'node:child_process';
;

const API_SERVICE_URL = process.env.ELIZAOS_API_URL || 'http://localhost:3000/api';
const API_KEY = process.env.ELIZAOS_API_KEY || '';

struct PluginSearchResult {
    std::optional<std::string> id;
    std::string name;
    std::string description;
    std::optional<double> score;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> features;
    std::optional<std::vector<std::string>> requiredConfig;
    std::optional<std::string> version;
    std::optional<std::string> npmPackage;
    std::optional<std::string> repository;
    std::optional<std::string> relevantSection;
};


struct PluginDetails {
    std::optional<std::string> id;
    std::string name;
    std::string description;
    std::optional<std::string> readme;
    std::optional<std::string> capabilitiesSummary;
    std::optional<std::string> repository;
    std::optional<std::string> version;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::vector<std::string>> features;
    std::optional<std::vector<std::string>> requiredConfig;
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
    std::optional<std::vector<std::string>> dependedBy;
    std::optional<std::string> npmPackage;
    std::optional<double> installCount;
    std::optional<double> searchCount;
    std::optional<std::vector<std::string>> categories;
    std::optional<std::string> lastIndexed;
    std::optional<std::string> createdAt;
    std::optional<std::string> updatedAt;
};


struct CloneResult {
    bool success;
    std::optional<std::string> error;
    std::optional<std::string> pluginName;
    std::optional<std::string> localPath;
    std::optional<bool> hasTests;
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
};


interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: string;
}

async ): Promise<ApiResponse<T>> {
  const url = `${API_SERVICE_URL}${endpoint}`;
  const headers = {
    'Content-Type': 'application/json',
    Authorization: `Bearer ${API_KEY}`,
    ...options.headers,
  };

  try {
    const response = await fetch(url, { ...options, headers });
    if (!response.ok) {
      const errorBody = await response.text();
      elizaLogger.error(
        `[pluginRegistryService] API request to ${url} failed with status ${response.status}: ${errorBody}`
      );
      throw new Error(`API request failed: ${response.statusText}`);
    }
    return response.json() as Promise<ApiResponse<T>>;
  } catch (error) {
    elizaLogger.error(`[pluginRegistryService] Network error calling ${url}:`, error);
    throw error;
  }
}

std::future<std::vector<PluginSearchResult>> searchPluginsByContent(const std::string& query);`);
  try {
    const data = await apiFetch<PluginSearchResult[]>('/plugins/search', {
      method: 'POST',
      body: JSON.stringify({ query, limit: 10 }),
    });
    return data.data || [];
  } catch (error) {
    return [];
  }
}

std::future<PluginMetadata | null> getPluginDetails(const std::string& name);`);
  try {
    const data = await apiFetch<PluginMetadata>(`/plugins/${encodeURIComponent(name)}`, {
      method: 'GET',
    });
    return data.data || null;
  } catch (error) {
    return null;
  }
}

std::future<std::vector<PluginMetadata>> getAllPlugins(););
    return data.data || [];
  } catch (error) {
    return [];
  }
}

std::future<std::vector<PluginMetadata>> getPluginsByCategory(const std::string& category);`);
  try {
    const data = await apiFetch<PluginMetadata[]>(
      `/plugins?category=${encodeURIComponent(category)}`,
      {
        method: 'GET',
      }
    );
    return data.data || [];
  } catch (error) {
    return [];
  }
}

std::future<CloneResult> clonePlugin(const std::string& pluginName);`);

  const plugin = await getPluginDetails(pluginName);
  if (!plugin || !plugin.repository) {
    return {
      success: false,
      error: `Plugin "${pluginName}" not found in registry or has no repository`,
    };
  }

  try {
    const path = await import('node:path');
    const fs = await import('node:fs/promises');
    const { exec } = await import('node:child_process');
    const { promisify } = await import('node:util');
    const execAsync = promisify(exec);

    const cloneDir = path.join(
      process.cwd(),
      'cloned-plugins',
      plugin.name.replace('@elizaos/', '')
    );

    await fs.mkdir(cloneDir, { recursive: true });
    await execAsync(`git clone ${plugin.repository} ${cloneDir}`);

    const packageJsonPath = path.join(cloneDir, 'package.json');
    const packageJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf-8'));
    const hasTests = !!(packageJson.scripts?.test || packageJson.devDependencies?.vitest);

    return {
      success: true,
      pluginName: plugin.name,
      localPath: cloneDir,
      hasTests,
      dependencies: packageJson.dependencies || {},
    };
  } catch (error) {
    elizaLogger.error('[pluginRegistryService] Clone failed:', error);
    const execError = error as ExecException;
    return {
      success: false,
      error: execError.stderr || execError.message || 'Unknown error during clone',
    };
  }
}

} // namespace elizaos
