#include "get-config.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isValidPostgresUrl(const std::string& url) {
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

            return patterns.some((pattern) => pattern.test(url));
        }

}

std::future<void> getElizaDirectories(std::optional<std::string> targetProjectDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto userEnv = UserEnvironment.getInstance();
    const auto paths = userEnv.getPathInfo();

    const auto projectRoot = targetProjectDir || paths.monorepoRoot || process.cwd();
    const auto elizaDir = targetProjectDir ? path.resolve(targetProjectDir, ".eliza") : paths.elizaDir;
    const auto envFilePath = targetProjectDir ? path.resolve(targetProjectDir, ".env") : paths.envFilePath;

    logger.debug('Eliza directories:', {
        elizaDir,
        projectRoot,
        targetProjectDir: targetProjectDir || "none",
        });

        const auto defaultElizaDbDir = path.resolve(projectRoot, ".eliza", ".elizadb");
        // Pass targetProjectDir to resolvePgliteDir to ensure it uses the correct base directory
        const auto elizaDbDir = resolvePgliteDir(std::nullopt, defaultElizaDbDir, targetProjectDir);

        return { elizaDir, elizaDbDir, envFilePath }

}

std::future<void> ensureDir(const std::string& dirPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!existsSync(dirPath)) {
        fs.mkdir(dirPath, { recursive: true });
        logger.debug(`Created directory: ${dirPath}`);
    }

}

std::future<void> setupEnvFile(const std::string& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Check if .env file exists
            const auto envExists = existsSync(envFilePath);

            if (!envExists) {
                // Create the file with template variables
                fs.writeFile(envFilePath, SAMPLE_ENV_TEMPLATE, "utf8");

                if (!isQuietMode()) {
                    std::cout << "[Config] Created .env file with template variables at: " + envFilePath << std::endl;
                }
                } else {
                    // File exists, check if it's empty
                    const auto content = fs.readFile(envFilePath, "utf8");
                    const auto trimmedContent = content.trim();

                    if (trimmedContent == '') {
                        // File is empty, write the template
                        fs.writeFile(envFilePath, SAMPLE_ENV_TEMPLATE, "utf8");

                        logger.info(
                        "[Config] Populated empty .env file with template variables at: " + envFilePath
                        );
                        } else {
                            logger.debug(`[Config] .env file already exists and has content at: ${envFilePath}`);
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

std::future<void> ensureElizaDir(std::optional<std::string> targetProjectDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dirs = getElizaDirectories(targetProjectDir);
    ensureDir(dirs.elizaDir);

    // Also create registry-cache.json and config.json files if they don't exist
    const auto registryCachePath = path.join(dirs.elizaDir, "registry-cache.json");
    const auto configPath = path.join(dirs.elizaDir, "config.json");

    if (!existsSync(registryCachePath)) {
        fs.writeFile(registryCachePath, /* JSON.stringify */ std::string({}, nullptr, 2), "utf8");
        logger.debug(`Created registry cache file: ${registryCachePath}`);
    }

    if (!existsSync(configPath)) {
        fs.writeFile(configPath, /* JSON.stringify */ std::string({ version: "1.0.0" }, nullptr, 2), "utf8");
        logger.debug(`Created config file: ${configPath}`);
    }

    return dirs;

}

std::future<void> setupPgLite(const std::string& dbDir, const std::string& envPath, std::optional<std::string> targetProjectDir) {
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

            // Then ensure PGLITE_DATA_DIR is properly set in the .env file
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

std::future<void> storePostgresUrl(const std::string& url, const std::string& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!url) return;

        try {
            // Ensure parent directory exists
            const auto envDir = path.dirname(envFilePath);
            fs.mkdir(envDir, { recursive: true });

            // Read existing content first to avoid duplicates
            auto content = "";
            if (existsSync(envFilePath)) {
                content = fs.readFile(envFilePath, "utf8");
            }

            // Remove existing POSTGRES_URL line if present
            const auto lines = content.split("\n").filter((line) => !line.startsWith("POSTGRES_URL="));
            "lines.push_back(" + "POSTGRES_URL=" + url;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            process.env.POSTGRES_URL = url;

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

std::future<void> storePgliteDataDir(const std::string& dataDir, const std::string& envFilePath) {
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
            const auto lines = content.split("\n").filter((line) => !line.startsWith("PGLITE_DATA_DIR="));
            "lines.push_back(" + "PGLITE_DATA_DIR=" + dataDir;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            process.env.PGLITE_DATA_DIR = dataDir;

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

std::future<std::string> promptAndStorePostgresUrl(const std::string& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    clack.intro("🗄️  PostgreSQL Configuration");

    const auto response = clack.text({;
        message: "Enter your Postgres URL:",
        placeholder: "postgresql://user:password@host:port/dbname",
        validate: (value) => {
            if (value.trim() == '') return 'Postgres URL cannot be empty';

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

bool isValidOpenAIKey(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;

    // OpenAI API keys typically start with 'sk-' and are 51 characters long
    return key.startsWith("sk-") && key.size() >= 20;

}

bool isValidAnthropicKey(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;

    // Anthropic API keys typically start with 'sk-ant-'
    return key.startsWith("sk-ant-") && key.size() >= 20;

}

bool isValidGoogleKey(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;

    // Google API keys are typically 39 characters long and contain alphanumeric chars with dashes
    return key.size() == 39 && /^[A-Za-z0-9_-]+$/.test(key);

}

std::future<void> storeOpenAIKey(const std::string& key, const std::string& envFilePath) {
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
            const auto lines = content.split("\n").filter((line) => !line.startsWith("OPENAI_API_KEY="));
            "lines.push_back(" + "OPENAI_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            process.env.OPENAI_API_KEY = key;

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

std::future<void> storeGoogleKey(const std::string& key, const std::string& envFilePath) {
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
            const auto lines = content;
            .split("\n");
            .filter((line) => !line.startsWith("GOOGLE_GENERATIVE_AI_API_KEY="));
            "lines.push_back(" + "GOOGLE_GENERATIVE_AI_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            process.env.GOOGLE_GENERATIVE_AI_API_KEY = key;

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

std::future<void> storeAnthropicKey(const std::string& key, const std::string& envFilePath) {
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
            const auto lines = content.split("\n").filter((line) => !line.startsWith("ANTHROPIC_API_KEY="));
            "lines.push_back(" + "ANTHROPIC_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");
            process.env.ANTHROPIC_API_KEY = key;

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

std::future<std::string> promptAndStoreOpenAIKey(const std::string& envFilePath) {
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
            validate: (value) => {
                if (value.trim() == '') return 'OpenAI API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: async (results, envPath) => {
                    const auto isValid = isValidOpenAIKey(results.key);
                    if (!isValid) {
                        clack.log.warn("Invalid API key format detected. Expected format: sk-...");
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeOpenAIKey(results.key, envPath);
                    },
                    successMessage: "OpenAI integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
                    return result.key || nullptr;

}

std::future<std::string> promptAndStoreAnthropicKey(const std::string& envFilePath) {
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
            validate: (value) => {
                if (value.trim() == '') return 'Anthropic API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: async (results, envPath) => {
                    const auto isValid = isValidAnthropicKey(results.key);
                    if (!isValid) {
                        clack.log.warn("Invalid API key format detected. Expected format: sk-ant-...");
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeAnthropicKey(results.key, envPath);
                    },
                    successMessage: "Claude integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
                    return result.key || nullptr;

}

bool isValidOllamaEndpoint(const std::string& endpoint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!endpoint || typeof endpoint != 'string') return false;

    try {
        const auto url = new URL(endpoint);
        return url.protocol == "http:" || url.protocol == "https:";
        } catch {
            return false;
        }

}

std::future<void> storeOllamaConfig(const std::any& config, const std::string& envFilePath) {
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
            const auto lines = content;
            .split("\n");
            .filter(;
            [&](line) { return !line.startsWith("OLLAMA_API_ENDPOINT=") && !line.startsWith("OLLAMA_MODEL="); }
            );

            // Add new Ollama configuration
            "lines.push_back(" + "OLLAMA_API_ENDPOINT=" + config.endpoint;
            "lines.push_back(" + "OLLAMA_MODEL=" + config.model;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");

            // Update process.env
            process.env.OLLAMA_API_ENDPOINT = config.endpoint;
            process.env.OLLAMA_MODEL = config.model;

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

std::future<> promptAndStoreOllamaEmbeddingConfig(const std::string& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    endpoint: string; embeddingModel: string
}

std::future<> promptAndStoreOllamaConfig(const std::string& envFilePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    endpoint: string; model: string
}

std::future<std::string> promptAndStoreGoogleKey(const std::string& envFilePath) {
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
            validate: (value) => {
                if (value.trim() == '') return 'Google API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: async (results, envPath) => {
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

                    const auto result = promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
                    return result.key || nullptr;

}

bool isValidOpenRouterKey(const std::string& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!key || typeof key != 'string') return false;
    // OpenRouter keys typically start with "sk-or-" followed by alphanumeric characters
    return key.startsWith("sk-or-") && key.size() > 10;

}

std::future<void> storeOpenRouterKey(const std::string& key, const std::string& envFilePath) {
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
            const auto lines = content.split("\n").filter((line) => !line.startsWith("OPENROUTER_API_KEY="));

            // Add new OpenRouter API key
            "lines.push_back(" + "OPENROUTER_API_KEY=" + key;

            fs.writeFile(envFilePath, lines.join("\n"), "utf8");

            // Update process.env
            process.env.OPENROUTER_API_KEY = key;

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

std::future<std::string> promptAndStoreOpenRouterKey(const std::string& envFilePath) {
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
            validate: (value) => {
                if (value.trim() == '') return 'OpenRouter API key cannot be empty';
                return std::nullopt;
                },
                },
                ],
                storeFunction: async (results, envPath) => {
                    const auto isValid = isValidOpenRouterKey(results.key);
                    if (!isValid) {
                        clack.log.warn("Invalid API key format detected. Expected format: sk-or-...");
                        clack.log.warn("The key has been saved but may not work correctly.");
                    }
                    storeOpenRouterKey(results.key, envPath);
                    },
                    successMessage: "OpenRouter integration configured",
                    };

                    const auto result = promptAndStoreProviderConfig<{ key: string }>(config, envFilePath);
                    return result.key || nullptr;

}

std::future<std::string> configureDatabaseSettings(auto reconfigure) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Set up directories and env file
    const auto { elizaDbDir, envFilePath } = ensureElizaDir();
    setupEnvFile(envFilePath);
    loadEnvironment(path.dirname(envFilePath));

    // Check if we already have database configuration in env
    auto postgresUrl = process.env.POSTGRES_URL;
    const auto pgliteDataDir = resolvePgliteDir(std::nullopt, elizaDbDir);

    // Add debug logging
    logger.debug(`Configuration check - POSTGRES_URL: ${postgresUrl ? 'SET' : 'NOT SET'}`);
    logger.debug(`Configuration check - PGLITE_DATA_DIR: ${pgliteDataDir ? 'SET' : 'NOT SET'}`);
    logger.debug(`Configuration check - reconfigure: ${reconfigure}`);

    // BYPASS ADDED: Skip prompts and always use postgres if URL is provided
    if (process.env.POSTGRES_URL) {
        std::cout << "BYPASS: Using postgres URL from environment variable" << std::endl;
        return process.env.POSTGRES_URL;
    }

    // If we already have PGLITE_DATA_DIR set in env and not reconfiguring, use PGLite
    if (pgliteDataDir && !reconfigure) {
        logger.debug(`Using existing PGLite configuration: ${pgliteDataDir}`);

        // Ensure the directory exists
        ensureDir(pgliteDataDir);

        return nullptr;
    }

    // BYPASS ADDED: Default to pglite if no configuration is provided
    std::cout << 'BYPASS: No database configuration found << defaulting to pglite' << std::endl;
    setupPgLite(elizaDbDir, envFilePath);
    return nullptr;

}

std::future<void> resolveConfigPaths(const std::string& cwd, RawConfig config) {
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

} // namespace elizaos
