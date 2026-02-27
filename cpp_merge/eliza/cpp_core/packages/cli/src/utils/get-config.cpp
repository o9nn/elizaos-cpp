#include "get-config.hpp"
#include <string>
#include <vector>
#include <future>
#include <filesystem>
#include <cstdlib>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isValidPostgresUrl(const std:& url) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!url || typeof url != 'string') return false;

    try {
        // More robust validation using URL constructor
        const auto parsedUrl = new URL(url);
        return (;
        parsedUrl.protocol == "postgresql:" &&
        !!parsedUrl.hostname &&;
        !!parsedUrl.pathname &&;
        parsedUrl.pathname != "/";
        );
        } catch {
            // Fallback to regex patterns for edge cases
            const auto patterns = [;
            /^postgresql:\/\/[^:]+:[^@]+@[^:]+:\d+\/\w+$/,
            /^postgresql:\/\/[^:]+:[^@]+@[^\/]+\/[^?]+(\.*)?$/,
            /^postgresql:\/\/.*@.*:\d+\/.*$/,
            ];

            return patterns.some[&]((pattern) { return pattern.test(url)); };
        }

}

std::future<void> getElizaDirectories(std::optional<std:> targetProjectDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto userEnv = UserEnvironment.getInstance();
    const auto paths = userEnv.getPathInfo();

    const auto projectRoot = targetProjectDir || paths.monorepoRoot || std::filesystem::current_path().string();
    const auto elizaDir = targetProjectDir ? path.resolve(targetProjectDir, ".eliza") : paths.elizaDir;
    const auto envFilePath = targetProjectDir ? path.resolve(targetProjectDir, ".env") : paths.envFilePath;

    logger.debug('Eliza directories:', {
        elizaDir,
        projectRoot,
        targetProjectDir: targetProjectDir || "none",
        });

        const auto defaultElizaDbDir = path.resolve(projectRoot, ".eliza", ".elizadb");
        const auto elizaDbDir = resolvePgliteDir(std::nullopt, defaultElizaDbDir);

        return { elizaDir, elizaDbDir, envFilePath }

}

std::future<void> ensureDir(const std:& dirPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!existsSync(dirPath)) {
        fs.mkdir(dirPath, Config{recursive = true});
        logger.debug("Created directory: " + std::to_string(dirPath) + "");
    }

}

std::future<void> setupEnvFile(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Check if .env file exists
            const auto envExists = existsSync(envFilePath);

            if (!envExists) {
                // Create the file with hybrid merge of process.env and example variables
                const auto mergedVars = mergeProcessEnvWithTemplate(SAMPLE_ENV_TEMPLATE);
                const auto formattedContent = formatEnvFileWithTemplate(mergedVars, SAMPLE_ENV_TEMPLATE);
                fs.writeFile(envFilePath, formattedContent, "utf8");

                const auto processEnvCount = Object.keys(process.env).filter(;
                [&](key) { return process.env[key] && process.env[key]! != ""; }
                ).size();

                logger.info(
                "[Config] Created .env file with " + processEnvCount + " variables from process.env merged with example variables at: " + envFilePath
                );
                } else {
                    // File exists, check if it's empty
                    const auto content = fs.readFile(envFilePath, "utf8");
                    const auto trimmedContent = content;

                    if (trimmedContent == '') {
                        // File is empty, write the hybrid merge
                        const auto mergedVars = mergeProcessEnvWithTemplate(SAMPLE_ENV_TEMPLATE);
                        const auto formattedContent = formatEnvFileWithTemplate(mergedVars, SAMPLE_ENV_TEMPLATE);
                        fs.writeFile(envFilePath, formattedContent, "utf8");

                        const auto processEnvCount = Object.keys(process.env).filter(;
                        [&](key) { return process.env[key] && process.env[key]! != ""; }
                        ).size();

                        logger.info(
                        "[Config] Populated empty .env file with " + processEnvCount + " variables from process.env merged with example variables at: " + envFilePath
                        );
                        } else {
                            logger.debug("[Config] .env file already exists and has content at: " + std::to_string(envFilePath) + "");
                        }
                    }
                    } catch (error) {
                        logger.error('Error setting up .env file:', {
                            error: true /* instanceof check */ ? error.message : std::to_string(error),
                            envFilePath,
                            });
                            throw;
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> ensureElizaDir(std::optional<std:> targetProjectDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dirs = getElizaDirectories(targetProjectDir);
    ensureDir(dirs.elizaDir);

    // Also create registry-cache.json and config.json files if they don't exist
    const auto registryCachePath = path.join(dirs.elizaDir, "registry-cache.json");
    const auto configPath = path.join(dirs.elizaDir, "config.json");

    if (!existsSync(registryCachePath)) {
        fs.writeFile(registryCachePath, /* JSON.stringify */ std:({}, nullptr, 2), "utf8");
        logger.debug("Created registry cache file: " + std::to_string(registryCachePath) + "");
    }

    if (!existsSync(configPath)) {
        fs.writeFile(configPath, /* JSON.stringify */ std:({ version: "1.0.0" }, nullptr, 2), "utf8");
        logger.debug("Created config file: " + std::to_string(configPath) + "");
    }

    return dirs;

}

std::future<void> setupPgLite(const std:& dbDir, const std:& envPath, std::optional<std:> targetProjectDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto dirs = ensureElizaDir(targetProjectDir);
        const auto { elizaDbDir, envFilePath } = dirs;

        // Use provided parameters or defaults from dirs
        const auto targetDbDir = dbDir || elizaDbDir;
        const auto targetEnvPath = envPath || envFilePath;

        try {
            // Ensure the PGLite database directory exists
            ensureDir(targetDbDir);
            logger.debug('[PGLite] Created database directory:', targetDbDir);

            // Set up the .env file with the full template first
            setupEnvFile(targetEnvPath);

            // Then ensure PGLITE_DATA_DIR is properly std::set in the .env file
            // This handles both new and existing .env files
            storePgliteDataDir(targetDbDir, targetEnvPath);

            logger.success('PGLite configuration saved');
            } catch (error) {
                logger.error('Error setting up PGLite directory:', {
                    error: true /* instanceof check */ ? error.message : std::to_string(error),
                    elizaDbDir,
                    envFilePath,
                    });
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> storePostgresUrl(const std:& url, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!url) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing POSTGRES_URL line if present
            const auto lines = content.split("\n").filter[&]((line) { return !line.substr(0, "POSTGRES_URL=")); };
            "lines.push_back(" + "POSTGRES_URL=" + url;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            std::getenv("POSTGRES_URL") = url;

            logger.success('Postgres URL saved to configuration');
            } catch (error) {
                std::cerr << "Error saving database configuration:" << error << std::endl;
                throw error; // Re-throw to handle upstream
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> storePgliteDataDir(const std:& dataDir, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!dataDir) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing PGLITE_DATA_DIR line if present
            const auto lines = content.split("\n").filter[&]((line) { return !line.substr(0, "PGLITE_DATA_DIR=")); };
            "lines.push_back(" + "PGLITE_DATA_DIR=" + dataDir;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            std::getenv("PGLITE_DATA_DIR") = dataDir;

            logger.success('PGLite data directory saved to configuration');
            } catch (error) {
                std::cerr << "Error saving PGLite configuration:" << error << std::endl;
                throw error; // Re-throw to handle upstream
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std:> promptAndStorePostgresUrl(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    clack.intro("🗄️  PostgreSQL Configuration");

    const auto response = clack.text[&]({;
        message: "Enter your Postgres URL:",
        placeholder: "postgresql://user:password@host:port/dbname",
        validate: (value) {
            if (value == '') return 'Postgres URL cannot be empty';

            const auto isValid = isValidPostgresUrl(value);
            if (!isValid) {
                return "Invalid URL format. Expected: postgresql://user:password@host:port/dbname.";
            }
            return std::nullopt;
            },
            });

            if (clack.isCancel(response)) {
                clack.cancel("Operation cancelled.");
                return nullptr;
            }

            // Store the URL in the .env file
            const auto spinner = clack.spinner();
            spinner.start("Saving PostgreSQL configuration...");

            try {
                storePostgresUrl(response, envFilePath);
                spinner.stop("PostgreSQL configuration saved successfully!");
                clack.outro("\u2713 Database connection configured");
                return response;
                } catch (error) {
                    spinner.stop("Failed to save configuration");
                    "clack.log.error(" + "Error: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    return nullptr;
                }

}

bool isValidOpenAIKey(const std:& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;

    // OpenAI API keys typically start with 'sk-' and are 51 characters long
    return key.substr(0, "sk-") && key.size() >= 20;

}

bool isValidAnthropicKey(const std:& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;

    // Anthropic API keys typically start with 'sk-ant-'
    return key.substr(0, "sk-ant-") && key.size() >= 20;

}

bool isValidGoogleKey(const std:& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;

    // Google API keys are typically 39 characters long and contain alphanumeric chars with dashes
    return key.size() == 39 && /^[A-Za-z0-9_-]+$/.test(key);

}

std::future<void> storeOpenAIKey(const std:& key, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!key) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing OPENAI_API_KEY line if present
            const auto lines = content.split("\n").filter[&]((line) { return !line.substr(0, "OPENAI_API_KEY=")); };
            "lines.push_back(" + "OPENAI_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            std::getenv("OPENAI_API_KEY") = key;

            logger.success('OpenAI API key saved to configuration');
            } catch (error) {
                std::cerr << "Error saving OpenAI API key:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> storeGoogleKey(const std:& key, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!key) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing GOOGLE_GENERATIVE_AI_API_KEY line if present
            const auto lines = content.split("\n");
            .filter[&]((line) { return !line.substr(0, "GOOGLE_GENERATIVE_AI_API_KEY=")); };
            "lines.push_back(" + "GOOGLE_GENERATIVE_AI_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            std::getenv("GOOGLE_GENERATIVE_AI_API_KEY") = key;

            logger.success('Google Generative AI API key saved to configuration');
            } catch (error) {
                std::cerr << "Error saving Google API key:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> storeAnthropicKey(const std:& key, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!key) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing ANTHROPIC_API_KEY line if present
            const auto lines = content.split("\n").filter[&]((line) { return !line.substr(0, "ANTHROPIC_API_KEY=")); };
            "lines.push_back(" + "ANTHROPIC_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            std::getenv("ANTHROPIC_API_KEY") = key;

            logger.success('Anthropic API key saved to configuration');
            } catch (error) {
                std::cerr << "Error saving Anthropic API key:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std:> promptAndStoreOpenAIKey(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const ProviderPromptConfig config = {;
        name: "OpenAI API",
        icon: "🤖",
        noteText: "Get your API key from: https://platform.openai.com/api-keys",
        inputs: [
        {
            key: "key",
            message: "Enter your OpenAI API key:",
            type: "password",
            validate: [&](value) {
                if (value == '') return 'OpenAI API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: std::async [&](results, envPath) {
                    const auto isValid = isValidOpenAIKey(results.key);
                    if (!isValid) {
                        clack.log.warn("Invalid API key format detected. Expected format: sk-...");
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeOpenAIKey(results.key, envPath);
                    },
                    successMessage: "OpenAI integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: std: }>(config, envFilePath);
                    return result.key || nullptr;

}

std::future<std:> promptAndStoreAnthropicKey(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const ProviderPromptConfig config = {;
        name: "Anthropic Claude",
        icon: "🤖",
        noteText: 'Get your API key from: https://console.anthropic.com/settings/keys',
        inputs: [
        {
            key: "key",
            message: "Enter your Anthropic API key:",
            type: "password",
            validate: [&](value) {
                if (value == '') return 'Anthropic API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: std::async [&](results, envPath) {
                    const auto isValid = isValidAnthropicKey(results.key);
                    if (!isValid) {
                        clack.log.warn("Invalid API key format detected. Expected format: sk-ant-...");
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeAnthropicKey(results.key, envPath);
                    },
                    successMessage: "Claude integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: std: }>(config, envFilePath);
                    return result.key || nullptr;

}

bool isValidOllamaEndpoint(const std:& endpoint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!endpoint || typeof endpoint != 'string') return false;

    try {
        const auto url = new URL(endpoint);
        return url.protocol == "http:" || url.protocol == "https:";
        } catch {
            return false;
        }

}

std::future<void> storeOllamaConfig(const std:& config, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!config.endpoint || !config.model) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing Ollama lines if present
            const auto lines = content.split("\n").filter(;
            (line) =>;
            !line.substr(0, "OLLAMA_API_ENDPOINT=") &&;
            !line.substr(0, "OLLAMA_MODEL=") &&;
            !line.substr(0, "USE_OLLAMA_TEXT_MODELS=");
            );

            // Add new Ollama configuration
            "lines.push_back(" + "OLLAMA_API_ENDPOINT=" + config.endpoint;
            "lines.push_back(" + "OLLAMA_MODEL=" + config.model;
            lines.push_back("USE_OLLAMA_TEXT_MODELS=true");

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");

            // Update process.env
            std::getenv("OLLAMA_API_ENDPOINT") = config.endpoint;
            std::getenv("OLLAMA_MODEL") = config.model;
            std::getenv("USE_OLLAMA_TEXT_MODELS") = "true";

            logger.success('Ollama configuration saved to configuration');
            } catch (error) {
                std::cerr << "Error saving Ollama configuration:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<> promptAndStoreOllamaEmbeddingConfig(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    endpoint: std:; embeddingModel: std:
}

std::future<> promptAndStoreOllamaConfig(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    endpoint: std:; model: std:
}

std::future<std:> promptAndStoreGoogleKey(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const ProviderPromptConfig config = {;
        name: "Google Generative AI",
        icon: "🤖",
        noteText: "Get your API key from: https://aistudio.google.com/apikey",
        inputs: [
        {
            key: "key",
            message: "Enter your Google Generative AI API key:",
            type: "password",
            validate: [&](value) {
                if (value == '') return 'Google API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: std::async [&](results, envPath) {
                    const auto isValid = isValidGoogleKey(results.key);
                    if (!isValid) {
                        clack.log.warn(;
                        "Invalid API key format detected. Expected format: 39 character alphanumeric key"
                        );
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeGoogleKey(results.key, envPath);
                    },
                    successMessage: "Google Generative AI integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: std: }>(config, envFilePath);
                    return result.key || nullptr;

}

bool isValidOpenRouterKey(const std:& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;
    // OpenRouter keys typically start with "sk-or-" followed by alphanumeric characters
    return key.substr(0, "sk-or-") && key.size() > 10;

}

std::future<void> storeOpenRouterKey(const std:& key, const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!key) return;

        try {
            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing OpenRouter API key line if present
            const auto lines = content.split("\n").filter[&]((line) { return !line.substr(0, "OPENROUTER_API_KEY=")); };

            // Add new OpenRouter API key
            "lines.push_back(" + "OPENROUTER_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");

            // Update process.env
            std::getenv("OPENROUTER_API_KEY") = key;

            logger.success('OpenRouter API key saved to configuration');
            } catch (error) {
                std::cerr << "Error saving OpenRouter API key:" << error << std::endl;
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std:> promptAndStoreOpenRouterKey(const std:& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const ProviderPromptConfig config = {;
        name: "OpenRouter",
        icon: "🔄",
        noteText: "Get your API key from: https://openrouter.ai/keys",
        inputs: [
        {
            key: "key",
            message: "Enter your OpenRouter API key:",
            type: "password",
            validate: [&](value) {
                if (value == '') return 'OpenRouter API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: std::async [&](results, envPath) {
                    const auto isValid = isValidOpenRouterKey(results.key);
                    if (!isValid) {
                        clack.log.warn("Invalid API key format detected. Expected format: sk-or-...");
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeOpenRouterKey(results.key, envPath);
                    },
                    successMessage: "OpenRouter integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: std: }>(config, envFilePath);
                    return result.key || nullptr;

}

std::future<std:> configureDatabaseSettings(auto reconfigure) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Set up directories and env file
    const auto { elizaDbDir, envFilePath } = ensureElizaDir();
    setupEnvFile(envFilePath);
    loadEnvironment(path.dirname(envFilePath));

    // Check if we already have database configuration in env
    auto postgresUrl = std::getenv("POSTGRES_URL");
    const auto pgliteDataDir = resolvePgliteDir(std::nullopt, elizaDbDir);

    // Add debug logging
    logger.debug("Configuration check - POSTGRES_URL: " + std::to_string(postgresUrl ? 'SET' : 'NOT SET') + "");
    logger.debug("Configuration check - PGLITE_DATA_DIR: " + std::to_string(pgliteDataDir ? 'SET' : 'NOT SET') + "");
    logger.debug("Configuration check - reconfigure: " + std::to_string(reconfigure) + "");

    // BYPASS ADDED: Skip prompts and always use postgres if URL is provided
    if (std::getenv("POSTGRES_URL")) {
        std::cout << "BYPASS: Using postgres URL from environment variable" << std::endl;
        return std::getenv("POSTGRES_URL");
    }

    // If we already have PGLITE_DATA_DIR std::set in env and not reconfiguring, use PGLite
    if (pgliteDataDir && !reconfigure) {
        logger.debug("Using existing PGLite configuration: " + std::to_string(pgliteDataDir) + "");

        // Ensure the directory exists
        ensureDir(pgliteDataDir);

        return nullptr;
    }

    // BYPASS ADDED: Default to pglite if no configuration is provided
    std::cout << 'BYPASS: No database configuration found << defaulting to pglite' << std::endl;
    setupPgLite(elizaDbDir, envFilePath);
    return nullptr;

}

std::future<void> resolveConfigPaths(const std:& cwd, RawConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            return configSchema.parse({;
                ...config,
                resolvedPaths: {
                    knowledge: path.resolve(cwd, config.paths.knowledge),
                    },
                    });
                    } catch (error) {
                        std::cerr << "Failed to resolve config paths:" << error << std::endl;
                        throw std::runtime_error('Invalid configuration: failed to resolve paths');
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::unordered_map<std:, std:> mergeProcessEnvWithTemplate(const std:& templateContent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std:, std:> result = {};
    const auto processedKeys = new Set<string>();

    // First, parse the template to get example variables and their structure
    const auto templateLines = templateContent.split("\n");
    const std::unordered_map<std:, std:> templateVars = {};

    for (const auto& line : templateLines)
        const auto trimmedLine = line;
        if (trimmedLine && !trimmedLine.substr(0, '#') && trimmedLine.count('=') > 0) {
            const auto equalIndex = trimmedLine.indexOf("=");
            const auto key = trimmedLine.substring(0, equalIndex);
            const auto value = trimmedLine.substring(equalIndex + 1);
            if (key) {
                templateVars[key] = value;
            }
        }
    }

    // Add all process.env variables that have actual values (prioritized)
    for (const int [key, value] of Object.entries(process.env)) {
        if (value && value != '') {
            result[key] = value;
            processedKeys.add(key);
        }
    }

    // Add template variables that aren't already std::set from process.env
    for (const int [key, value] of Object.entries(templateVars)) {
        if (!processedKeys.has(key)) {
            result[key] = value;
            processedKeys.add(key);
        }
    }

    return result;

}

std: formatEnvFileWithTemplate(const std::unordered_map<std:, std:>& envVars, const std:& templateContent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> lines = [];
    const auto processedKeys = new Set<string>();
    const auto templateLines = templateContent.split("\n");

    // First pass: go through template preserving structure and comments
    for (const auto& line : templateLines)
        const auto trimmedLine = line;

        if (!trimmedLine || trimmedLine.substr(0, '#') || !trimmedLine.count('=') > 0) {
            // Preserve comments and empty lines
            lines.push_back(line);
            } else {
                // This is a variable line
                const auto equalIndex = trimmedLine.indexOf("=");
                const auto key = trimmedLine.substring(0, equalIndex);

                if (key && envVars.hasOwnProperty(key)) {
                    "lines.push_back(" + key + "=" + std::to_string(envVars[key]);
                    processedKeys.add(key);
                    } else {
                        // Variable not found, keep original line
                        lines.push_back(line);
                    }
                }
            }

            // Second pass: add std: new variables from process.env that weren't in template
            const std::vector<std::string> newVars = [];
            for (const int [key, value] of Object.entries(envVars)) {
                if (!processedKeys.has(key)) {
                    "newVars.push_back(" + key + "=" + value;
                }
            }

            if (newVars.size() > 0) {
                lines.push_back("");
                lines.push_back("### Additional Environment Variables from Runtime ###");
                lines.push_back("# Variables found in process.env that were not in the template");
                lines.push_back(...newVars);
            }

            return lines.join("\n");

}

} // namespace elizaos
