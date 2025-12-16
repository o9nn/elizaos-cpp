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
import type { AIModelOption, DatabaseOption } from '../types';

/**
 * Returns a list of available databases for project initialization without requiring external API calls.
 */
std::future<std::vector<std::string>> getLocalAvailableDatabases();

/**
 * Gets available AI models for selection during project creation.
 */
,
    {
      title: 'OpenAI',
      value: 'openai',
      description: 'GPT-4 models',
    },
    {
      title: 'Anthropic',
      value: 'claude',
      description: 'Claude models',
    },
    {
      title: 'OpenRouter',
      value: 'openrouter',
      description: 'Access multiple AI models',
    },
    {
      title: 'Ollama',
      value: 'ollama',
      description: 'Self-hosted models',
    },
    {
      title: 'Google Generative AI',
      value: 'google',
      description: 'Gemini models',
    },
  ];
}

/**
 * Gets available database options for selection during project creation.
 */
,
    {
      title: 'PostgreSQL',
      value: 'postgres',
      description: 'Production database',
    },
  ];
}

/**
 * Prompts user to select a database type with interactive UI.
 */
std::future<std::string> selectDatabase();)),
    initialValue: 'pglite',
  });

  if (clack.isCancel(database)) {
    clack.cancel('Operation cancelled.');
    process.exit(0);
  }

  return database as string;
}

/**
 * Prompts user to select an AI model with interactive UI.
 */
std::future<std::string> selectAIModel();)),
    initialValue: 'local',
  });

  if (clack.isCancel(aiModel)) {
    clack.cancel('Operation cancelled.');
    process.exit(0);
  }

  return aiModel as string;
}

/**
 * Gets available embedding models for selection when primary AI model doesn't support embeddings.
 */
,
    {
      title: 'OpenAI',
      value: 'openai',
      description: 'OpenAI text-embedding-ada-002',
    },
    {
      title: 'Ollama',
      value: 'ollama',
      description: 'Self-hosted embedding models',
    },
    {
      title: 'Google Generative AI',
      value: 'google',
      description: 'Google embedding models',
    },
  ];
}

/**
 * Prompts user to select an embedding model when the primary AI model doesn't support embeddings.
 */
std::future<std::string> selectEmbeddingModel();)),
    initialValue: 'local',
  });

  if (clack.isCancel(embeddingModel)) {
    clack.cancel('Operation cancelled.');
    process.exit(0);
  }

  return embeddingModel as string;
}

} // namespace elizaos
