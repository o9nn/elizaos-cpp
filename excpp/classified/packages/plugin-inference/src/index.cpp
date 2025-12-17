#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void resetInferenceState() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    state.providers.clear();
    state.providers = new Map([;
    [;
    InferenceProvider.OPENAI,
    {
        plugin: openaiPlugin,
        status: 'not_configured',
        displayName: 'OpenAI',
        message: 'API key required',
        },
        ],
        [;
        InferenceProvider.ANTHROPIC,
        {
            plugin: anthropicPlugin,
            status: 'not_configured',
            displayName: 'Anthropic Claude',
            message: 'API key required',
            },
            ],
            [;
            InferenceProvider.OLLAMA,
            {
                plugin: ollamaPlugin,
                status: 'not_configured',
                displayName: 'Ollama (Local)',
                message: 'Local server required',
                },
                ],
                [;
                InferenceProvider.ELIZAOS,
                {
                    plugin: elizaOSServicesPlugin,
                    status: 'not_configured',
                    displayName: 'ElizaOS Cloud',
                    message: 'API key required',
                    },
                    ],
                    [;
                    InferenceProvider.GROQ,
                    {
                        plugin: groqPlugin,
                        status: 'not_configured',
                        displayName: 'Groq',
                        message: 'API key required',
                        },
                        ],
                        [;
                        InferenceProvider.LOCAL_EMBEDDING,
                        {
                            plugin: localEmbeddingPlugin,
                            status: 'not_configured',
                            displayName: 'Transformers.js (Local)',
                            message: 'Pure JavaScript embeddings',
                            },
                            ],
                            ]);
                            state.preferences = [;
                            InferenceProvider.OLLAMA,
                            InferenceProvider.GROQ,
                            InferenceProvider.ELIZAOS,
                            InferenceProvider.OPENAI,
                            InferenceProvider.ANTHROPIC,
                            ];
                            state.selectedProvider = nullptr;
                            state.initialized = false;

}

std::future<void> initializeProviders(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (state.initialized) {
        return;
    }

    std::cout << '[INFERENCE] Initializing inference providers...' << std::endl;

    // Always ensure local_embedding is available for embeddings
    const auto localEmbeddingInfo = state.providers.get(InferenceProvider.LOCAL_EMBEDDING);
    if (localEmbeddingInfo) {
        try {
            if (localEmbeddingInfo.plugin.init) {
                localEmbeddingInfo.plugin.init(localEmbeddingInfo.plugin.config || {}, runtime);
            }
            localEmbeddingInfo.status = 'available';
            localEmbeddingInfo.message = 'Ready';
            std::cout << '[INFERENCE] Local embedding provider initialized successfully' << std::endl;
            } catch (error) {
                std::cerr << '[INFERENCE] Failed to initialize local embedding provider:' << error << std::endl;
                localEmbeddingInfo.status = 'error';
                localEmbeddingInfo.message = true /* instanceof check */ ? error.message : 'Initialization failed';
            }
        }

        // Load preferences from environment or runtime setting
        const auto envPreferences = runtime.getSetting('INFERENCE_PREFERENCES');
        if (envPreferences) {
            try {
                const auto prefArray = envPreferences.split(',').map((p: string) => p.trim());
                // Validate preferences are valid providers
                const auto validPrefs = prefArray.filter((p: InferenceProvider) =>;
                Object.values(InferenceProvider).includes(p);
                );
                if (validPrefs.length > 0) {
                    state.preferences = validPrefs;
                    std::cout << "[INFERENCE] Using preferences from config: " + std::to_string(validPrefs.join(', ')) << std::endl;
                }
                } catch (error) {
                    std::cout << '[INFERENCE] Invalid preferences format << using defaults' << std::endl;
                }
            }

            // Load selected provider from runtime setting
            const auto selectedProvider = runtime.getSetting('SELECTED_PROVIDER');
            if (selectedProvider && Object.values(InferenceProvider).includes(selectedProvider)) {
                state.selectedProvider = selectedProvider;
                std::cout << "[INFERENCE] Using selected provider: " + std::to_string(selectedProvider) << std::endl;
            }

            // Initialize all providers in parallel, but handle failures gracefully
            const auto initPromises = Array.from(state.providers.entries()).map(;
            async ([providerName, providerInfo]) => {
                // Skip local_embedding as it's already initialized above
                if (providerName == InferenceProvider.LOCAL_EMBEDDING) {
                    return;
                }

                try {
                    // First, try to initialize the plugin
                    if (providerInfo.plugin.init) {
                        try {
                            providerInfo.plugin.init(providerInfo.plugin.config || {}, runtime);
                            logger.debug(`[INFERENCE] Successfully initialized ${providerName}`);
                            } catch (initError) {
                                // If init fails, mark as error but don't throw
                                std::cout << "[INFERENCE] Failed to initialize " + std::to_string(providerName) + ":" << initError << std::endl;
                                providerInfo.status = 'error';
                                providerInfo.message =;
                                true /* instanceof check */ ? initError.message : 'Initialization failed';
                                return; // Continue with next provider;
                            }
                        }

                        // Then check if it's valid
                        const auto isValid = checkProviderValidity(runtime, providerName, providerInfo);
                        providerInfo.status = isValid ? 'available' : 'not_configured';
                        providerInfo.message = isValid ? 'Ready' : getProviderConfigMessage(providerName);

                        std::cout << "[INFERENCE] Provider " + std::to_string(providerName) + ": " + std::to_string(providerInfo.status) << std::endl;
                        } catch (error) {
                            std::cerr << "[INFERENCE] Error checking provider " + std::to_string(providerName) + ":" << error << std::endl;
                            providerInfo.status = 'error';
                            providerInfo.message = true /* instanceof check */ ? error.message : 'Unknown error';
                        }
                    }
                    );

                    // Wait for all providers to initialize (failures won't stop the process)
                    Promise.allSettled(initPromises);

                    // Emit initialization event for dependent plugins (if emitEvent is available)
                    if (runtime.emitEvent) {
                        runtime.emitEvent('inference:initialized', {
                            providers: Array.from(state.providers.entries()).map(([name, info]) => ({
                                name,
                                status: info.status,
                                message: info.message,
                                })),
                                selectedProvider: state.selectedProvider,
                                preferences: state.preferences,
                                });
                            }

                            state.initialized = true;
                            std::cout << '[INFERENCE] Provider initialization complete' << std::endl;

                            // Log summary of available providers
                            const auto availableProviders = Array.from(state.providers.entries());
                            .filter(([_, info]) => info.status == 'available');
                            .map(([name, _]) => name);

                            if (availableProviders.length == 0) {
                                std::cout << '[INFERENCE] No providers are available. Please configure at least one provider.' << std::endl;
                                } else {
                                    std::cout << "[INFERENCE] Available providers: " + std::to_string(availableProviders.join(', ')) << std::endl;
                                }

}

std::future<bool> checkProviderValidity(IAgentRuntime runtime, InferenceProvider providerName, ProviderInfo providerInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try to get the validation service for the provider
    const auto validationServiceName = std::to_string(providerName) + "-validation";
    const auto validationService = runtime.getService(validationServiceName) | nullptr;

    if (validationService && typeof validationService.isValid == 'function') {
        return validationService.isValid();
    }

    // Fallback: check based on provider type if no validation service
    switch (providerName) {
        case InferenceProvider.OPENAI:
        return !!runtime.getSetting('OPENAI_API_KEY');

        case InferenceProvider.ANTHROPIC:
        return !!runtime.getSetting('ANTHROPIC_API_KEY');

        case InferenceProvider.GROQ:
        return !!runtime.getSetting('GROQ_API_KEY');

        case InferenceProvider.ELIZAOS:
        // Check if authenticated with ElizaOS
        return !!runtime.getSetting('ELIZAOS_API_KEY');

        case InferenceProvider.OLLAMA:
        // Ollama doesn't require API key, just check if endpoint is accessible
        return true;

        case InferenceProvider.LOCAL_EMBEDDING:
        // Local embedding doesn't require external configuration
        return true;

        default:
        return false;
    }

}

std::string getProviderConfigMessage(InferenceProvider provider) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (provider) {
        case InferenceProvider.OPENAI:
        return 'Set OPENAI_API_KEY environment variable';
        case InferenceProvider.ANTHROPIC:
        return 'Set ANTHROPIC_API_KEY environment variable';
        case InferenceProvider.GROQ:
        return 'Set GROQ_API_KEY environment variable';
        case InferenceProvider.OLLAMA:
        return 'Start Ollama server (ollama serve)';
        case InferenceProvider.ELIZAOS:
        return 'Set ELIZAOS_API_KEY environment variable';
        case InferenceProvider.LOCAL_EMBEDDING:
        return 'Ready to use (pure JavaScript)';
        default:
        return 'Configuration required';
    }

}

std::future<> getActiveProvider(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    name: string; plugin: Plugin
}

std::future<> getProviderStatus(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    providers: Array<{
        name: string;
        displayName: string;
        status: 'available' | 'not_configured' | 'error';
        message: string;
        }>;
        active: string | nullptr;
        selected: string | nullptr;
        preferences: string[];

}

std::future<void> setSelectedProvider(IAgentRuntime runtime, const std::string& provider) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!state.initialized) {
            initializeProviders(runtime);
        }

        if (provider && !state.providers.has(provider as InferenceProvider)) {
            throw std::runtime_error(`Unknown provider: ${provider}`);
        }

        state.selectedProvider = provider | nullptr;

        // Optionally persist to runtime settings
        if (provider) {
            runtime.setSetting('SELECTED_PROVIDER', provider);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> setProviderPreferences(IAgentRuntime runtime, const std::vector<std::string>& preferences) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!state.initialized) {
        initializeProviders(runtime);
    }

    const auto validPreferences = preferences;
    .map((p) => p);
    .filter((p) => state.providers.has(p));

    state.preferences = validPreferences;

    // Optionally persist to runtime settings
    runtime.setSetting('INFERENCE_PREFERENCES', validPreferences.join(','));

}

} // namespace elizaos
