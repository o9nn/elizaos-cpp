#include "elizaos/core.hpp"
#include "resolve-utils.hpp"
#include "spinner-utils.hpp"
#include "user-environment.hpp"
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
;
;
;
;
;
// Database config schemas
const postgresConfigSchema = z.object({
  type: z.literal('postgres'),
  config: z.object({
    url: z.string().optional(),
  }),
});

const pgliteConfigSchema = z.object({
  type: z.literal('pglite'),
  config: z.object({
    dataDir: z.string(),
  }),
});

/**
 * Default sample environment variables written to new .env files.
 */
const SAMPLE_ENV_TEMPLATE = `### elizaos Environment Variables ###
# To get started, copy this file to .env, or make a .env and add the settings you'd like to override
# Please read the comments for each of the configurations

## The only thing you ABSOLUTELY NEED to get up and running is one of the model provider keys,
## i.e. OPENAI_API_KEY or ANTHROPIC_API_KEY, or setup the ollama plugin
## Everything else is optional, and most settings and secrets can be configured in your agent or through the GUI
## For multi-agent, each agent will need keys for the various services it is connected to
-------------------------------
## You can use the .env or environment variables generally for shared keys, such as to model providers,
## database, etc, with scoped keys for services such as Telegram, Discord, etc

## MODEL PROVIDER KEYS ##
## Eliza is compatible with a wide array of model providers. Many have OpenAI compatible APIs,
## and you can use them by overriding the base URL

## NOTE: You will need a provider that provides embeddings. So even if you use Claude, you will
## need to get embeddings using another provider, for example openai or ollama

# OpenAI Configuration
OPENAI_API_KEY=
## Use this to override the openai endpoint, for example for using together.ai, fireworks or other providers
## Optional overrides:
--------------------------------
# OPENAI_BASE_URL=
# OPENAI_SMALL_MODEL=gpt-4o-mini
# OPENAI_LARGE_MODEL=gpt-4o
# OPENAI_EMBEDDING_MODEL=text-embedding-3-small
# OPENAI_EMBEDDING_URL=
# OPENAI_EMBEDDING_DIMENSIONS=1536
# OPENAI_IMAGE_DESCRIPTION_MODEL=gpt-4o-mini
# OPENAI_IMAGE_DESCRIPTION_MAX_TOKENS=8192

# Anthropic Configuration
## By default in most of our starter kits, Anthropic will take precedence over OpenAI in handling requests
## Anthropic does not handle embeddings, so you may wish to use OpenAI for that, even while Claude is handling text generation
--------------------------------
ANTHROPIC_API_KEY=
# Optional overrides:
# ANTHROPIC_SMALL_MODEL=claude-3-5-haiku-latest
# ANTHROPIC_LARGE_MODEL=claude-3-5-sonnet-latest


# Ollama Configuration
## Highly recommended to use gemma3:latest for text generation
--------------------------------
# OLLAMA_API_ENDPOINT=http://localhost:11434/api
# OLLAMA_SMALL_MODEL=gemma3:latest
# OLLAMA_MEDIUM_MODEL=gemma3:latest
# OLLAMA_LARGE_MODEL=gemma3:latest


# Highly recommended to use nomic-embed-text for embeddings
# OLLAMA_EMBEDDING_MODEL=nomic-embed-text

### DATABASE ###
# By default, Eliza will use a local pglite instance
# If you fill out POSTGRES_URL, the agent will connect to your postgres instance instead of using the local path

# You can override the pglite data directory
PGLITE_DATA_DIR=

# Fill this out if you want to use Postgres
POSTGRES_URL=

### LOGGING CONFIGURATION ###
# Logging Configuration (supported: fatal, error, warn, info, debug, trace | default: info)
LOG_LEVEL=


# Sentry Configuration
--------------------------------
## DO NOT CHANGE THIS UNLESS YOU KNOW WHAT YOU ARE DOING
--------------------------------
# Sentry is a tool for monitoring and logging errors and exceptions
# It is used to track errors and exceptions in the agent
--------------------------------
# Sentry Configuration
SENTRY_LOGGING=true
SENTRY_DSN=
SENTRY_ENVIRONMENT=
SENTRY_TRACES_SAMPLE_RATE=
SENTRY_SEND_DEFAULT_PII=

`;

/**
 * Validates a Postgres URL format
 * @param url The URL to validate
 * @returns True if the URL appears valid
 */
 catch {
    // Fallback to regex patterns for edge cases
    const patterns = [
      /^postgresql:\/\/[^:]+:[^@]+@[^:]+:\d+\/\w+$/,
      /^postgresql:\/\/[^:]+:[^@]+@[^\/]+\/[^?]+(\?.*)?$/,
      /^postgresql:\/\/.*@.*:\d+\/.*$/,
    ];

    return patterns.some((pattern) => pattern.test(url));
  }
}

/**
 * Retrieves the standard directory paths used by Eliza for configuration and database storage.
 *
 * @returns An object containing the Eliza configuration directory, the Eliza database directory for the current project, and the path to the Eliza `.env` file.
 */
async );

  const defaultElizaDbDir = path.resolve(projectRoot, '.eliza', '.elizadb');
  // Pass targetProjectDir to resolvePgliteDir to ensure it uses the correct base directory
  const elizaDbDir = await resolvePgliteDir(undefined, defaultElizaDbDir, targetProjectDir);

  return { elizaDir, elizaDbDir, envFilePath };
}

/**
 * Generic );
    logger.debug(`Created directory: ${dirPath}`);
  }
}

/**
 * Sets up the .env file by creating it if it doesn't exist or populating it with a hybrid
 * merge of process.env variables and example variables if it's empty
 * @param envFilePath Path to the .env file
 */
std::future<void> setupEnvFile(const std::string& envFilePath);`);
      }
    } else {
      // File exists, check if it's empty
      const content = await fs.readFile(envFilePath, 'utf8');
      const trimmedContent = content.trim();

      if (trimmedContent === '') {
        // File is empty, write the template
        await fs.writeFile(envFilePath, SAMPLE_ENV_TEMPLATE, 'utf8');

        logger.info(
          `[Config] Populated empty .env file with template variables at: ${envFilePath}`
        );
      } else {
        logger.debug(`[Config] .env file already exists and has content at: ${envFilePath}`);
      }
    }
  } catch (error) {
    logger.error('Error setting up .env file:', {
      error: error instanceof Error ? error.message : String(error),
      envFilePath,
    });
    throw error;
  }
}

/**
 * Ensures the Eliza configuration directory exists and returns standard Eliza directory paths.
 *
 * @returns An object containing paths for the Eliza configuration directory, the Eliza database directory, and the `.env` file.
 */
async , null, 2), 'utf8');
    logger.debug(`Created registry cache file: ${registryCachePath}`);
  }

  if (!existsSync(configPath)) {
    await fs.writeFile(configPath, JSON.stringify({ version: '1.0.0' }, null, 2), 'utf8');
    logger.debug(`Created config file: ${configPath}`);
  }

  return dirs;
}

/**
 * Sets up and configures PGLite database
 * @param elizaDbDir The directory for PGLite database
 * @param envFilePath Path to the .env file
 */
std::future<void> setupPgLite(string | undefined dbDir, string | undefined envPath, std::optional<std::string> targetProjectDir); = dirs;

  // Use provided parameters or defaults from dirs
  const targetDbDir = dbDir || elizaDbDir;
  const targetEnvPath = envPath || envFilePath;

  try {
    // Ensure the PGLite database directory exists
    await ensureDir(targetDbDir);
    logger.debug('[PGLite] Created database directory:', targetDbDir);

    // Set up the .env file with the full template first
    await setupEnvFile(targetEnvPath);

    // Then ensure PGLITE_DATA_DIR is properly set in the .env file
    // This handles both new and existing .env files
    await storePgliteDataDir(targetDbDir, targetEnvPath);

    logger.success('PGLite configuration saved');
  } catch (error) {
    logger.error('Error setting up PGLite directory:', {
      error: error instanceof Error ? error.message : String(error),
      elizaDbDir,
      envFilePath,
    });
    throw error;
  }
}

/**
 * Stores the provided Postgres connection URL in the specified `.env` file, replacing any existing entry.
 *
 * Updates the `POSTGRES_URL` environment variable in both the file and the current process.
 *
 * @param url - The Postgres connection URL to store.
 * @param envFilePath - Path to the `.env` file where the URL should be saved.
 *
 * @throws {Error} If reading from or writing to the `.env` file fails.
 */
std::future<void> storePostgresUrl(const std::string& url, const std::string& envFilePath););

    // Read existing content first to avoid duplicates
    let content = '';
    if (existsSync(envFilePath)) {
      content = await fs.readFile(envFilePath, 'utf8');
    }

    // Remove existing POSTGRES_URL line if present
    const lines = content.split('\n').filter((line) => !line.startsWith('POSTGRES_URL='));
    lines.push(`POSTGRES_URL=${url}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');
    process.env.POSTGRES_URL = url;

    logger.success('Postgres URL saved to configuration');
  } catch (error) {
    logger.error('Error saving database configuration:', error);
    throw error; // Re-throw to handle upstream
  }
}

/**
 * Stores the provided PGLite data directory in the specified `.env` file, replacing any existing entry.
 *
 * Updates the `PGLITE_DATA_DIR` environment variable in both the file and the current process.
 *
 * @param dataDir - The PGLite data directory path to store.
 * @param envFilePath - Path to the `.env` file where the directory should be saved.
 *
 * @throws {Error} If reading from or writing to the `.env` file fails.
 */
std::future<void> storePgliteDataDir(const std::string& dataDir, const std::string& envFilePath);

    // Remove existing PGLITE_DATA_DIR line if present
    const lines = content.split('\n').filter((line) => !line.startsWith('PGLITE_DATA_DIR='));
    lines.push(`PGLITE_DATA_DIR=${dataDir}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');
    process.env.PGLITE_DATA_DIR = dataDir;

    logger.success('PGLite data directory saved to configuration');
  } catch (error) {
    logger.error('Error saving PGLite configuration:', error);
    throw error; // Re-throw to handle upstream
  }
}

/**
 * Prompts the user for a Postgres URL, validates it, and stores it
 * @returns The configured Postgres URL or null if user cancels
 */
std::future<string | null> promptAndStorePostgresUrl(const std::string& envFilePath);
      return undefined;
    },
  });

  if (clack.isCancel(response)) {
    clack.cancel('Operation cancelled.');
    return null;
  }

  // Store the URL in the .env file
  const spinner = clack.spinner();
  spinner.start('Saving PostgreSQL configuration...');

  try {
    await storePostgresUrl(response, envFilePath);
    spinner.stop('PostgreSQL configuration saved successfully!');
    clack.outro('\u2713 Database connection configured');
    return response;
  } catch (error) {
    spinner.stop('Failed to save configuration');
    clack.log.error(`Error: ${error instanceof Error ? error.message : String(error)}`);
    return null;
  }
}

/**
 * Validates an OpenAI API key format
 * @param key The API key to validate
 * @returns True if the key appears valid
 */


/**
 * Validates an Anthropic API key format
 * @param key The API key to validate
 * @returns True if the key appears valid
 */


/**
 * Validates a Google Generative AI API key format
 * @param key The API key to validate
 * @returns True if the key appears valid
 */


/**
 * Stores OpenAI API key in the .env file
 * @param key The OpenAI API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeOpenAIKey(const std::string& key, const std::string& envFilePath);

    // Remove existing OPENAI_API_KEY line if present
    const lines = content.split('\n').filter((line) => !line.startsWith('OPENAI_API_KEY='));
    lines.push(`OPENAI_API_KEY=${key}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');
    process.env.OPENAI_API_KEY = key;

    logger.success('OpenAI API key saved to configuration');
  } catch (error) {
    logger.error('Error saving OpenAI API key:', error);
    throw error;
  }
}

/**
 * Stores Google Generative AI API key in the .env file
 * @param key The Google API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeGoogleKey(const std::string& key, const std::string& envFilePath);

    // Remove existing GOOGLE_GENERATIVE_AI_API_KEY line if present
    const lines = content
      .split('\n')
      .filter((line) => !line.startsWith('GOOGLE_GENERATIVE_AI_API_KEY='));
    lines.push(`GOOGLE_GENERATIVE_AI_API_KEY=${key}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');
    process.env.GOOGLE_GENERATIVE_AI_API_KEY = key;

    logger.success('Google Generative AI API key saved to configuration');
  } catch (error) {
    logger.error('Error saving Google API key:', error);
    throw error;
  }
}

/**
 * Stores Anthropic API key in the .env file
 * @param key The Anthropic API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeAnthropicKey(const std::string& key, const std::string& envFilePath);

    // Remove existing ANTHROPIC_API_KEY line if present
    const lines = content.split('\n').filter((line) => !line.startsWith('ANTHROPIC_API_KEY='));
    lines.push(`ANTHROPIC_API_KEY=${key}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');
    process.env.ANTHROPIC_API_KEY = key;

    logger.success('Anthropic API key saved to configuration');
  } catch (error) {
    logger.error('Error saving Anthropic API key:', error);
    throw error;
  }
}

/**
 * Generic configuration for provider prompts
 */
struct ProviderPromptConfig {
    std::string name;
    std::string icon;
    std::string noteText;
    Array<{ inputs;
    std::string key;
    std::string message;
    std::optional<std::string> placeholder;
    std::optional<std::string> initialValue;
    'text' | 'password' type;
    (value: string) => string | undefined validate;
    (config: any, envFilePath: string) => Promise<void> storeFunction;
    std::string successMessage;
};


/**
 * Generic  ${config.name} Configuration`);

  if (config.noteText) {
    clack.note(config.noteText, 'API Key Information');
  }

  const results: any = {};

  // Collect all inputs
  for (const input of config.inputs) {
    const promptFn = input.type === 'password' ? clack.password : clack.text;
    const promptConfig: any = {
      message: input.message,
      validate: input.validate,
    };

    if (input.placeholder) promptConfig.placeholder = input.placeholder;
    if (input.initialValue) promptConfig.initialValue = input.initialValue;

    const response = await promptFn(promptConfig);

    if (clack.isCancel(response)) {
      clack.cancel('Operation cancelled.');
      return null;
    }

    results[input.key] = input.type === 'text' ? response.trim() : response;
  }

  // Store the configuration
  const spinner = clack.spinner();
  spinner.start(`Saving ${config.name} configuration...`);

  try {
    await config.storeFunction(results, envFilePath);
    spinner.stop(`${config.name} configuration saved successfully!`);
    clack.outro(`✓ ${config.successMessage}`);
    return results as T;
  } catch (error) {
    spinner.stop('Failed to save configuration');
    clack.log.error(`Error: ${error instanceof Error ? error.message : String(error)}`);
    return null;
  }
}

/**
 * Prompts the user for an OpenAI API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured OpenAI API key or null if user cancels
 */
std::future<string | null> promptAndStoreOpenAIKey(const std::string& envFilePath);,
      },
    ],
    storeFunction: async (results, envPath) => {
      const isValid = isValidOpenAIKey(results.key);
      if (!isValid) {
        clack.log.warn('Invalid API key format detected. Expected format: sk-...');
        clack.log.warn('The key has been saved but may not work correctly.');
      }
      await storeOpenAIKey(results.key, envPath);
    },
    successMessage: 'OpenAI integration configured',
  };

  const result = await promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
  return result?.key || null;
}

/**
 * Prompts the user for an Anthropic API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured Anthropic API key or null if user cancels
 */
std::future<string | null> promptAndStoreAnthropicKey(const std::string& envFilePath);,
      },
    ],
    storeFunction: async (results, envPath) => {
      const isValid = isValidAnthropicKey(results.key);
      if (!isValid) {
        clack.log.warn('Invalid API key format detected. Expected format: sk-ant-...');
        clack.log.warn('The key has been saved but may not work correctly.');
      }
      await storeAnthropicKey(results.key, envPath);
    },
    successMessage: 'Claude integration configured',
  };

  const result = await promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
  return result?.key || null;
}

/**
 * Validates an Ollama API endpoint format
 * @param endpoint The endpoint URL to validate
 * @returns True if the endpoint appears valid
 */
 catch {
    return false;
  }
}

/**
 * Stores Ollama configuration in the .env file
 * @param config The Ollama configuration to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeOllamaConfig({ endpoint: string; model: string } config, const std::string& envFilePath);

    // Remove existing Ollama lines if present
    const lines = content
      .split('\n')
      .filter(
        (line) => !line.startsWith('OLLAMA_API_ENDPOINT=') && !line.startsWith('OLLAMA_MODEL=')
      );

    // Add new Ollama configuration
    lines.push(`OLLAMA_API_ENDPOINT=${config.endpoint}`);
    lines.push(`OLLAMA_MODEL=${config.model}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');

    // Update process.env
    process.env.OLLAMA_API_ENDPOINT = config.endpoint;
    process.env.OLLAMA_MODEL = config.model;

    logger.success('Ollama configuration saved to configuration');
  } catch (error) {
    logger.error('Error saving Ollama configuration:', error);
    throw error;
  }
}

/**
 * Prompts the user for Ollama embedding model selection
 * @param envFilePath Path to the .env file
 * @returns The configured Ollama embedding settings or null if user cancels
 */
std::future<{ endpoint: string; embeddingModel: string } | null> promptAndStoreOllamaEmbeddingConfig(const std::string& envFilePath);,
      },
      {
        key: 'embeddingModel',
        message: 'Enter your Ollama embedding model:',
        placeholder: 'nomic-embed-text',
        initialValue: 'nomic-embed-text',
        type: 'text',
        validate: (value) => {
          if (value.trim() === '') return 'Embedding model name cannot be empty';
          return undefined;
        },
      },
    ],
    storeFunction: async (results, envPath) => {
      // Store Ollama embedding configuration
      try {
        let content = '';
        if (existsSync(envPath)) {
          content = await fs.readFile(envPath, 'utf8');
        }

        // Only remove embedding-specific lines, preserve general Ollama config
        const lines = content
          .split('\n')
          .filter((line) => !line.startsWith('OLLAMA_EMBEDDING_MODEL='));

        // Check if we need to update the endpoint
        const endpointPattern = /^OLLAMA_API_ENDPOINT=(.*)$/m;
        const existingEndpointMatch = content.match(endpointPattern);

        if (existingEndpointMatch) {
          // Endpoint exists, only update if different
          if (results.endpoint !== existingEndpointMatch[1]) {
            const updatedLines = lines.map((line) => {
              if (line.startsWith('OLLAMA_API_ENDPOINT=')) {
                return `OLLAMA_API_ENDPOINT=${results.endpoint}`;
              }
              return line;
            });
            lines.length = 0;
            lines.push(...updatedLines);
          }
        } else {
          // No existing endpoint, add it
          lines.push(`OLLAMA_API_ENDPOINT=${results.endpoint}`);
        }

        // Add embedding-specific configuration
        lines.push(`OLLAMA_EMBEDDING_MODEL=${results.embeddingModel}`);

        await fs.writeFile(envPath, lines.join('\n'), 'utf8');

        // Update process.env
        process.env.OLLAMA_API_ENDPOINT = results.endpoint;
        process.env.OLLAMA_EMBEDDING_MODEL = results.embeddingModel;

        logger.success('Ollama embedding configuration saved');
      } catch (error) {
        logger.error('Error saving Ollama embedding configuration:', error);
        throw error;
      }
    },
    successMessage: 'Ollama embedding model configured',
  };

  return await promptAndStoreProviderConfig<{ endpoint: string; embeddingModel: string }>(
    config,
    envFilePath
  );
}

/**
 * Prompts the user for Ollama configuration, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured Ollama settings or null if user cancels
 */
std::future<{ endpoint: string; model: string } | null> promptAndStoreOllamaConfig(const std::string& envFilePath);,
      },
      {
        key: 'model',
        message: 'Enter your preferred Ollama model:',
        placeholder: 'llama2',
        initialValue: 'llama2',
        type: 'text',
        validate: (value) => {
          if (value.trim() === '') return 'Model name cannot be empty';
          return undefined;
        },
      },
    ],
    storeFunction: async (results, envPath) => {
      await storeOllamaConfig({ endpoint: results.endpoint, model: results.model }, envPath);
    },
    successMessage: 'Ollama integration configured',
  };

  return await promptAndStoreProviderConfig<{ endpoint: string; model: string }>(
    config,
    envFilePath
  );
}

/**
 * Prompts the user for a Google Generative AI API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured Google API key or null if user cancels
 */
std::future<string | null> promptAndStoreGoogleKey(const std::string& envFilePath);,
      },
    ],
    storeFunction: async (results, envPath) => {
      const isValid = isValidGoogleKey(results.key);
      if (!isValid) {
        clack.log.warn(
          'Invalid API key format detected. Expected format: 39 character alphanumeric key'
        );
        clack.log.warn('The key has been saved but may not work correctly.');
      }
      await storeGoogleKey(results.key, envPath);
    },
    successMessage: 'Google Generative AI integration configured',
  };

  const result = await promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
  return result?.key || null;
}

/**
 * Validates an OpenRouter API key format
 * @param key The API key to validate
 * @returns True if the key appears to be in valid format
 */


/**
 * Stores OpenRouter API key in the .env file
 * @param key The API key to store
 * @param envFilePath Path to the .env file
 */
std::future<void> storeOpenRouterKey(const std::string& key, const std::string& envFilePath);

    // Remove existing OpenRouter API key line if present
    const lines = content.split('\n').filter((line) => !line.startsWith('OPENROUTER_API_KEY='));

    // Add new OpenRouter API key
    lines.push(`OPENROUTER_API_KEY=${key}`);

    await fs.writeFile(envFilePath, lines.join('\n'), 'utf8');

    // Update process.env
    process.env.OPENROUTER_API_KEY = key;

    logger.success('OpenRouter API key saved to configuration');
  } catch (error) {
    logger.error('Error saving OpenRouter API key:', error);
    throw error;
  }
}

/**
 * Prompts the user for an OpenRouter API key, validates it, and stores it
 * @param envFilePath Path to the .env file
 * @returns The configured OpenRouter API key or null if user cancels
 */
std::future<string | null> promptAndStoreOpenRouterKey(const std::string& envFilePath);,
      },
    ],
    storeFunction: async (results, envPath) => {
      const isValid = isValidOpenRouterKey(results.key);
      if (!isValid) {
        clack.log.warn('Invalid API key format detected. Expected format: sk-or-...');
        clack.log.warn('The key has been saved but may not work correctly.');
      }
      await storeOpenRouterKey(results.key, envPath);
    },
    successMessage: 'OpenRouter integration configured',
  };

  const result = await promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
  return result?.key || null;
}

/**
 * Configures the database to use, either PGLite or PostgreSQL
 * @param reconfigure If true, force reconfiguration even if already configured
 * @returns The postgres URL if using Postgres, otherwise null
 */
std::future<string | null> configureDatabaseSettings(auto reconfigure = false); = await ensureElizaDir();
  await setupEnvFile(envFilePath);
  await loadEnvironment(path.dirname(envFilePath));

  // Check if we already have database configuration in env
  let postgresUrl = process.env.POSTGRES_URL;
  const pgliteDataDir = await resolvePgliteDir(undefined, elizaDbDir);

  // Add debug logging
  logger.debug(`Configuration check - POSTGRES_URL: ${postgresUrl ? 'SET' : 'NOT SET'}`);
  logger.debug(`Configuration check - PGLITE_DATA_DIR: ${pgliteDataDir ? 'SET' : 'NOT SET'}`);
  logger.debug(`Configuration check - reconfigure: ${reconfigure}`);

  // BYPASS ADDED: Skip prompts and always use postgres if URL is provided
  if (process.env.POSTGRES_URL) {
    console.log('BYPASS: Using postgres URL from environment variable');
    return process.env.POSTGRES_URL;
  }

  // If we already have PGLITE_DATA_DIR set in env and not reconfiguring, use PGLite
  if (pgliteDataDir && !reconfigure) {
    logger.debug(`Using existing PGLite configuration: ${pgliteDataDir}`);

    // Ensure the directory exists
    await ensureDir(pgliteDataDir);

    return null;
  }

  // BYPASS ADDED: Default to pglite if no configuration is provided
  console.log('BYPASS: No database configuration found, defaulting to pglite');
  await setupPgLite(elizaDbDir, envFilePath);
  return null;
}

// Main config schema
/**
 * Schema definition for the raw configuration object.
 *
 * @type {z.ZodType<RawConfig>}
 */
const rawConfigSchema = z
  .object({
    $schema: z.string().optional(),
    database: z.discriminatedUnion('type', [postgresConfigSchema, pgliteConfigSchema]),
    plugins: z.object({
      registry: z.string().url(),
      installed: z.array(z.string()),
    }),
    paths: z.object({
      knowledge: z.string(),
    }),
  })
  .strict();

/**
 * Type definition for the inferred type of the raw config schema.
 */
using RawConfig = z.infer<typeof rawConfigSchema>;

const configSchema = rawConfigSchema.extend({
  resolvedPaths: z.object({
    knowledge: z.string(),
  }),
});

/**
 * Define the type `Config` as the inferred type from the `configSchema`.
 */
using Config = z.infer<typeof configSchema>;

/**
 * Resolves the paths in the given configuration based on the provided current working directory (cwd).
 * @param {string} cwd - The current working directory.
 * @param {RawConfig} config - The raw configuration object.
 * @returns {Promise<Config>} The resolved configuration object with updated paths.
 */
async ,
    });
  } catch (error) {
    logger.error('Failed to resolve config paths:', error);
    throw new Error('Invalid configuration: failed to resolve paths');
  }
}

/**
 * Load environment variables from the project `.env` file if it exists.
 *
 * @param projectDir - Directory containing the `.env` file. Defaults to the current working directory.
 */
async );
  }
}

} // namespace elizaos
