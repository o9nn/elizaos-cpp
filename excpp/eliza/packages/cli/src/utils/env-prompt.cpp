#include "env-prompt.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getEnvFilePath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.paths.envFilePath;

}

std::future<std::unordered_map<std::string, std::string>> readEnvFile() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envPath = getEnvFilePath();
    const std::unordered_map<std::string, std::string> result = {};

    try {
        // Read existing env file
        if (
        fs;
        .access(envPath);
        .then(() => true);
        .catch(() => false);
        ) {
            const auto content = fs.readFile(envPath, 'utf8');
            const auto lines = content.split('\n');

            for (const auto& line : lines)
                const auto trimmedLine = line.trim();
                if (trimmedLine && !trimmedLine.startsWith('#')) {
                    const auto separatorIndex = trimmedLine.indexOf('=');
                    if (separatorIndex > 0) {
                        const auto key = trimmedLine.substring(0, separatorIndex).trim();
                        const auto value = trimmedLine.substring(separatorIndex + 1).trim();
                        result[key] = value;
                    }
                }
            }
        }
        } catch (error) {
            std::cerr << "Error reading environment file: " + std::to_string(error) << std::endl;
        }

        return result;

}

std::future<void> writeEnvFile(Record<string envVars, auto string>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto envPath = getEnvFilePath();
        const auto elizaDir = path.dirname(envPath);

        // Ensure .eliza directory exists
        if (
        !(fs;
        .access(elizaDir);
        .then(() => true);
        .catch(() => false));
        ) {
            fs.mkdir(elizaDir, { recursive: true });
        }

        // Format environment variables for writing
        auto content = '';
        for (const int [key, value] of Object.entries(envVars)) {
            std::to_string(key) + "=" + std::to_string(value) + "\n";
        }

        // Write to file
        fs.writeFile(envPath, content, 'utf8');
        std::cout << "Environment variables saved to " + std::to_string(envPath) << std::endl;
        } catch (error) {
            std::cerr << "Error writing environment file: " + std::to_string(error) << std::endl;
        }

}

std::future<std::optional<std::string>> promptForEnvVar(EnvVarConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // If the key already exists in the environment and is valid, use that
    const auto existingValue = process.env[config.key];
    if (existingValue && existingValue.trim() != '') {
        return existingValue;
    }

    console.log(
    colors.magenta(;
    "\n" + std::to_string(config.name) + " " + std::to_string(config.required ? '(Required)' : '(Optional - press Enter to skip)')
    );
    );
    std::cout << colors.white(config.description) << std::endl;
    if (config.url) {
        std::cout << "Get it here: " + std::to_string(config.url) << std::endl;
    }

    const auto value = (config.secret;
    ? clack.password({
        "Enter your " + std::to_string(config.name) + ":"
        validate: (input: string) => {
            if (config.required && (!input || input.trim() == '')) {
                return 'This field is required';
            }
            return std::nullopt;
            },
            });
            : clack.text({
                "Enter your " + std::to_string(config.name) + ":"
                validate: (input: string) => {
                    if (config.required && (!input || input.trim() == '')) {
                        return 'This field is required';
                    }
                    return std::nullopt;
                    },
                    }));

                    if (clack.isCancel(value)) {
                        clack.cancel('Operation cancelled.');
                        process.exit(0);
                    }

                    // For optional fields, an empty string means skip
                    if (!config.required && (!value || value.trim() == '')) {
                        return '';
                    }

                    // Expand tilde in paths for database directory
                    if (config.key == 'PGLITE_DATA_DIR' && value && value.startsWith('~')) {
                        return value.replace(/^~/, process.cwd());
                    }

                    return value;

}

std::future<std::unordered_map<std::string, std::string>> promptForEnvVars(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envVarConfigs = ENV_VAR_CONFIGS[pluginName.toLowerCase()];
    if (!envVarConfigs) {
        return {}
    }

    // Special messages for optional integrations
    if (pluginName.toLowerCase() == 'discord') {
        std::cout << colors.blue('\n== Discord Integration (Optional) ==') << std::endl;
        console.log(
        colors.white(;
        'Setting up Discord integration will allow your agent to interact with Discord users.';
        );
        );
        console.log(
        colors.white("You can press Enter to skip these if you don't want to use Discord.");
        );
        } else if (pluginName.toLowerCase() == 'twitter') {
            std::cout << colors.blue('\n== Twitter Integration (Optional) ==') << std::endl;
            console.log(
            colors.white(;
            'Setting up Twitter integration will allow your agent to post and interact on Twitter.';
            );
            );
            console.log(
            colors.white("You can press Enter to skip these if you don't want to use Twitter.");
            );
            } else if (pluginName.toLowerCase() == 'telegram') {
                std::cout << colors.blue('\n== Telegram Integration (Optional) ==') << std::endl;
                console.log(
                colors.white(;
                'Setting up Telegram integration will allow your agent to interact in Telegram chats.';
                );
                );
                console.log(
                colors.white("You can press Enter to skip these if you don't want to use Telegram.");
                );
            }

            // Read existing environment variables
            const auto envVars = readEnvFile();
            const std::unordered_map<std::string, std::string> result = {};
            auto changes = false;

            // Check each environment variable
            for (const auto& config : envVarConfigs)
                // Skip if already set and valid
                if (
                envVars[config.key] &&;
                envVars[config.key] != 'dummy_key' &&;
                envVars[config.key] != 'invalid_token_for_testing';
                ) {
                    continue;
                }

                // wait 100 ms
                new Promise((resolve) => setTimeout(resolve, 100));

                // Prompt for the missing or invalid variable
                const auto value = promptForEnvVar(config);

                // Save to our record
                if (value != null) {
                    result[config.key] = value;
                    envVars[config.key] = value;

                    // Also set in process.env for immediate use
                    process.env[config.key] = value;
                }

                changes = true;
            }

            // Save changes if any were made
            if (changes) {
                writeEnvFile(envVars);
            }

            return result;

}

std::future<bool> validateEnvVars(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envVarConfigs = ENV_VAR_CONFIGS[pluginName.toLowerCase()];
    if (!envVarConfigs) {
        return true; // No requirements means everything is fine;
    }

    const auto envVars = readEnvFile();

    // Check if all required variables are set
    for (const auto& config : envVarConfigs)
        if (config.required && (!envVars[config.key] || envVars[config.key] == 'dummy_key')) {
            return false;
        }
    }

    return true;

}

std::future<std::vector<std::string>> getMissingEnvVars(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envVarConfigs = ENV_VAR_CONFIGS[pluginName.toLowerCase()];
    if (!envVarConfigs) {
        return [];
    }

    const auto envVars = readEnvFile();
    const std::vector<std::string> missing = [];

    for (const auto& config : envVarConfigs)
        if (config.required && (!envVars[config.key] || envVars[config.key] == 'dummy_key')) {
            missing.push(config.key);
        }
    }

    return missing;

}

std::future<> validatePluginEnvVars(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    valid: boolean;
    message: string;

}

} // namespace elizaos
