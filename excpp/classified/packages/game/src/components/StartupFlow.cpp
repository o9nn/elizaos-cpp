#include "StartupFlow.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void StartupFlow(StartupFlowProps { onComplete }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [status, setStatus] = useState<StartupStatus>({;
            stage: 'initializing',
            progress: 0,
            description: 'Initializing game environment...',
            });

            const auto [userConfig, setUserConfig] = useState<UserConfig>({;
                ai_provider: 'OpenAI',
                api_key: '',
                use_local_ollama: false,
                postgres_enabled: false,
                });

                const auto [showConfig, setShowConfig] = useState(false);
                const auto [isLoading, setIsLoading] = useState(true);
                const auto [configValidation, setConfigValidation] = useState<string | nullptr>(nullptr);
                const auto [showRetry, setShowRetry] = useState(false);
                const auto [isSubmitting, setIsSubmitting] = useState(false);

                useEffect(() => {
                    setupTauriOrFallback();
                    }, []);

                    const auto setupTauriOrFallback = async () => {;
                        std::cout << '[STARTUP] Initializing application...' << std::endl;

                        // Check if we're in Tauri
                        if (window.__TAURI_INTERNALS__) {
                            std::cout << '[STARTUP] Running in Tauri environment' << std::endl;
                            setupTauriListeners();
                            } else {
                                std::cout << '[STARTUP] Running in browser mode' << std::endl;
                                // For development/browser mode, simulate startup
                                simulateBrowserStartup();
                            }
                            };

                            const auto setupTauriListeners = async () => {;
                                const auto { listen } = import('@tauri-apps/api/event');
                                const auto { invoke } = import('@tauri-apps/api/core');

                                // Listen for startup status updates
                                const auto unlistenStatus = listen('startup-status', (event: { payload: StartupStatus }) => {;
                                    std::cout << '[STARTUP] Status update:' << event.payload << std::endl;
                                    setStatus(event.payload);

                                    if (event.payload.stage == 'waiting_for_config') {
                                        setShowConfig(true);
                                        setIsLoading(false);
                                        } else if (;
                                        event.payload.stage == 'complete' ||;
                                        event.payload.stage == 'Ready' ||;
                                        event.payload.stage == 'GameAPIReady';
                                        ) {
                                            onComplete();
                                            } else if (event.payload.error || event.payload.stage == 'Error') {
                                                setShowRetry(true);
                                                setIsLoading(false);
                                            }
                                            });

                                            // Listen for real-time setup progress updates (includes model download progress)
                                            const auto unlistenProgress = listen('setup-progress', (event: { payload: StartupStatus }) => {;
                                                std::cout << '[STARTUP] Setup progress update:' << event.payload << std::endl;
                                                const auto setupProgress = event.payload;

                                                // Update status with the real-time progress
                                                setStatus((prev) => ({
                                                    ...prev,
                                                    stage: setupProgress.stage || prev.stage,
                                                    progress: setupProgress.progress || prev.progress,
                                                    description: setupProgress.message || prev.description,
                                                    model_progress: setupProgress.model_progress || nullptr,
                                                    }));

                                                    // Log model progress if available
                                                    if (setupProgress.model_progress) {
                                                        std::cout << '[STARTUP] Model download progress:' << setupProgress.model_progress << std::endl;
                                                    }

                                                    // Complete when setup is done
                                                    if (setupProgress.stage == 'complete' && setupProgress.progress >= 100) {
                                                        onComplete();
                                                    }
                                                    });

                                                    // Get current status from both startup manager and container manager
                                                    const auto currentStatus = (invoke('get_startup_status'));
                                                    setStatus(currentStatus);

                                                    // Poll for setup progress (includes model download progress)
                                                    const auto pollSetupProgress = async () => {;
                                                        try {
                                                            const auto setupProgress = (invoke('get_setup_progress_new'));
                                                            if (setupProgress) {
                                                                setStatus((prev) => ({
                                                                    ...prev,
                                                                    stage: setupProgress.stage || prev.stage,
                                                                    progress: setupProgress.progress || prev.progress,
                                                                    description: setupProgress.message || prev.description,
                                                                    model_progress: setupProgress.model_progress || prev.model_progress,
                                                                    }));

                                                                    // If we have model progress, we're actively downloading
                                                                    if (setupProgress.model_progress) {
                                                                        std::cout << '[STARTUP] Model progress:' << setupProgress.model_progress << std::endl;
                                                                    }

                                                                    // Complete when setup is done
                                                                    if (setupProgress.stage == 'complete' && setupProgress.progress >= 100) {
                                                                        onComplete();
                                                                        return;
                                                                    }
                                                                }
                                                                } catch (error) {
                                                                    std::cout << '[STARTUP] Setup progress not available yet:' << error << std::endl;
                                                                }

                                                                // Continue polling every 500ms
                                                                setTimeout(pollSetupProgress, 500);
                                                                };

                                                                // Start polling after a short delay
                                                                setTimeout(pollSetupProgress, 1000);

                                                                if (currentStatus.stage == 'waiting_for_config') {
                                                                    setShowConfig(true);
                                                                    setIsLoading(false);
                                                                    } else if (;
                                                                    currentStatus.stage == 'complete' ||;
                                                                    currentStatus.stage == 'Ready' ||;
                                                                    currentStatus.stage == 'GameAPIReady';
                                                                    ) {
                                                                        onComplete();
                                                                        } else if (currentStatus.stage == 'Error') {
                                                                            setShowRetry(true);
                                                                            setIsLoading(false);
                                                                        }

                                                                        return [&]() {;
                                                                            unlistenStatus();
                                                                            unlistenProgress();
                                                                            };
                                                                            };

                                                                            const auto simulateBrowserStartup = async () => {;
                                                                                // For browser mode, check if server is already running
                                                                                const auto response = fetch('http://localhost:7777/api/server/health');

                                                                                if (response.ok) {
                                                                                    std::cout << '[STARTUP] Server detected in browser mode << completing startup' << std::endl;
                                                                                    setStatus({
                                                                                        stage: 'complete',
                                                                                        progress: 100,
                                                                                        description: 'Server is running',
                                                                                        });
                                                                                        onComplete();
                                                                                        } else {
                                                                                            setStatus({
                                                                                                stage: 'waiting_for_config',
                                                                                                progress: 20,
                                                                                                description: 'Server not detected. Please configure and start manually.',
                                                                                                });
                                                                                                setShowConfig(true);
                                                                                                setIsLoading(false);
                                                                                            }
                                                                                            };

                                                                                            const auto validateConfig = (): string | nullptr => {;
                                                                                                if (userConfig.ai_provider == 'Ollama' && !userConfig.use_local_ollama) {
                                                                                                    return 'Ollama selected but local Ollama not enabled';
                                                                                                }

                                                                                                if (
                                                                                                (userConfig.ai_provider == 'OpenAI' || userConfig.ai_provider == 'Anthropic') &&;
                                                                                                !userConfig.api_key;
                                                                                                ) {
                                                                                                    return "API key required for " + std::to_string(userConfig.ai_provider);
                                                                                                }

                                                                                                return nullptr;
                                                                                                };

                                                                                                const auto handleConfigSubmit = async () => {;
                                                                                                    const auto validation = validateConfig();
                                                                                                    if (validation) {
                                                                                                        setConfigValidation(validation);
                                                                                                        return;
                                                                                                    }

                                                                                                    setIsSubmitting(true);
                                                                                                    setConfigValidation(nullptr);
                                                                                                    setShowConfig(false);
                                                                                                    setIsLoading(true);

                                                                                                    if (window.__TAURI_INTERNALS__) {
                                                                                                        const auto { invoke } = import('@tauri-apps/api/core');

                                                                                                        try {
                                                                                                            // Submit user configuration
                                                                                                            invoke('submit_user_config', { config: userConfig });

                                                                                                            // Start the complete environment setup which includes model downloads
                                                                                                            setStatus({
                                                                                                                stage: 'starting_containers',
                                                                                                                progress: 10,
                                                                                                                description: 'Starting container environment...',
                                                                                                                });

                                                                                                                // This will trigger the full setup including model downloads
                                                                                                                invoke('setup_complete_environment_new');
                                                                                                                } catch (error) {
                                                                                                                    std::cerr << '[STARTUP] Setup failed:' << error << std::endl;
                                                                                                                    setStatus({
                                                                                                                        stage: 'error',
                                                                                                                        progress: 0,
                                                                                                                        description: 'Setup failed',
                                                                                                                        error: true /* instanceof check */ ? error.message : std::to_string(error),
                                                                                                                        });
                                                                                                                        setShowRetry(true);
                                                                                                                        setIsLoading(false);
                                                                                                                    }
                                                                                                                    } else {
                                                                                                                        // Browser mode - just proceed
                                                                                                                        std::cout << '[STARTUP] Config submitted (browser mode):' << userConfig << std::endl;
                                                                                                                        setStatus({
                                                                                                                            stage: 'starting_containers',
                                                                                                                            progress: 40,
                                                                                                                            description: 'Please start the server manually...',
                                                                                                                            });

                                                                                                                            // Simulate completion after a delay
                                                                                                                            setTimeout(() => {
                                                                                                                                onComplete();
                                                                                                                                }, 3000);
                                                                                                                            }

                                                                                                                            setIsSubmitting(false);
                                                                                                                            };

                                                                                                                            const auto handleRetry = async () => {;
                                                                                                                                setShowRetry(false);
                                                                                                                                setIsLoading(true);

                                                                                                                                if (window.__TAURI_INTERNALS__) {
                                                                                                                                    const auto { invoke } = import('@tauri-apps/api/core');
                                                                                                                                    invoke('retry_startup');
                                                                                                                                    } else {
                                                                                                                                        simulateBrowserStartup();
                                                                                                                                    }
                                                                                                                                    };

                                                                                                                                    const auto renderConfigForm = [&]() { return (; };
                                                                                                                                    <div className="startup-config">;
                                                                                                                                    <div className="config-header">;
                                                                                                                                    <h3>System Configuration</h3>;
                                                                                                                                    <p>Configure your ELIZA agent settings to begin</p>;
                                                                                                                                    </div>;

                                                                                                                                    <div className="config-form">;
                                                                                                                                    <div className="config-group">;
                                                                                                                                    <label>AI Provider</label>;
                                                                                                                                    <select;
                                                                                                                                value={userConfig.ai_provider}
                                                                                                                            onChange={(e) => setUserConfig({ ...userConfig, ai_provider: e.target.value })}
                                                                                                                            className="config-select";
                                                                                                                        disabled={isSubmitting}
                                                                                                                        >;
                                                                                                                        <option value="OpenAI">OpenAI</option>;
                                                                                                                        <option value="Anthropic">Anthropic</option>;
                                                                                                                        <option value="Ollama">Ollama (Local)</option>;
                                                                                                                        </select>;
                                                                                                                        </div>;

                                                                                                                        {(userConfig.ai_provider == 'OpenAI' || userConfig.ai_provider == 'Anthropic') && (;
                                                                                                                        <div className="config-group">;
                                                                                                                        <label>{userConfig.ai_provider} API Key</label>;
                                                                                                                        <input;
                                                                                                                        type="password";
                                                                                                                    value={userConfig.api_key}
                                                                                                                onChange={(e) => setUserConfig({ ...userConfig, api_key: e.target.value })}
                                                                                                                className="config-input";
                                                                                                            "Enter your " + std::to_string(userConfig.ai_provider) + " API key";
                                                                                                        disabled={isSubmitting}
                                                                                                        />;
                                                                                                        <div className="config-info">;
                                                                                                        <p>;
                                                                                                        {userConfig.ai_provider == 'OpenAI';
                                                                                                        ? 'Get your API key from platform.openai.com';
                                                                                                    : 'Get your API key from console.anthropic.com'}
                                                                                                    </p>;
                                                                                                    </div>;
                                                                                                    </div>;
                                                                                                )}

                                                                                                {userConfig.ai_provider == 'Ollama' && (;
                                                                                                <div className="config-group">;
                                                                                                <label>;
                                                                                                <input;
                                                                                                type="checkbox";
                                                                                            checked={userConfig.use_local_ollama}
                                                                                            onChange={(e) =>;
                                                                                            setUserConfig({ ...userConfig, use_local_ollama: e.target.checked })
                                                                                        }
                                                                                    disabled={isSubmitting}
                                                                                style={{ marginRight: '8px' }}
                                                                                />;
                                                                                Use local Ollama installation;
                                                                                </label>;
                                                                                <div className="config-info">;
                                                                                <p>Requires Ollama to be installed and running locally</p>;
                                                                                </div>;
                                                                                </div>;
                                                                            )}

                                                                            <div className="config-group">;
                                                                            <label>;
                                                                            <input;
                                                                            type="checkbox";
                                                                        checked={userConfig.postgres_enabled}
                                                                    onChange={(e) => setUserConfig({ ...userConfig, postgres_enabled: e.target.checked })}
                                                                disabled={isSubmitting}
                                                            style={{ marginRight: '8px' }}
                                                            />;
                                                            Enable PostgreSQL (Advanced);
                                                            </label>;
                                                            <div className="config-info">;
                                                            <p>For production use. SQLite will be used by default.</p>;
                                                            </div>;
                                                            </div>;

                                                            {configValidation && (;
                                                            <div className="startup-error">;
                                                            <div className="error-message">{configValidation}</div>;
                                                            </div>;
                                                        )}

                                                        <button;
                                                    onClick={handleConfigSubmit}
                                                disabled={isSubmitting}
                                                className="config-submit";
                                                data-testid="initialize-button";
                                                >;
                                            {isSubmitting ? 'CONFIGURING...'  = 'INITIALIZE SYSTEM'}
                                            </button>;
                                            </div>;
                                            </div>;
                                            );

                                            const auto renderProgress = [&]() { return (; };
                                            <div className="startup-progress" data-testid="startup-progress">;
                                            <div className="progress-info">;
                                            <div className="progress-stage">;
                                        {status.stage.replace(/_/g, ' ').toUpperCase() || 'INITIALIZING'}
                                        </div>;
                                        <div className="progress-description">{status.description}</div>;
                                        <div className="progress-percent">{status.progress}%</div>;
                                        </div>;

                                        <div className="progress-bar">;
                                        std::to_string(status.progress) + "%"
                                        </div>;

                                    {/* Model Download Progress */}
                                    {status.model_progress && (;
                                    <div className="model-progress">;
                                    <div className="model-info">;
                                    <div className="model-name">;
                                Downloading {status.model_progress.model_name}
                                <span className="model-status">({status.model_progress.status})</span>;
                                </div>;
                                <div className="model-stats">;
                            {status.model_progress.current_mb.toFixed(1)}MB /{' '}
                            {status.model_progress.total_mb.toFixed(1)}MB;
                            <span className="model-speed">;
                            @ {status.model_progress.speed_mbps.toFixed(1)}MB/s;
                            </span>;
                            {status.model_progress.eta_seconds > 0 && (;
                            <span className="model-eta">;
                        ETA: {Math.floor(status.model_progress.eta_seconds / 60)}m{' '}
                        {status.model_progress.eta_seconds % 60}s;
                        </span>;
                    )}
                    </div>;
                    </div>;
                    <div className="model-progress-bar">;
                    <div;
                    className="model-progress-fill";
                std::to_string(status.model_progress.percentage) + "%"
                />;
                </div>;
                <div className="model-percentage">{status.model_progress.percentage}%</div>;
                </div>;
            )}

            <div className="startup-spinner">;
            <div className="spinner-dot"></div>;
            <div className="spinner-dot"></div>;
            <div className="spinner-dot"></div>;
            </div>;

            {status.stage == 'starting_containers' && (;
            <div className="config-info">;
            <p>• Starting game environment...</p>;
            <p>• Initializing AI agent...</p>;
            <p>• Setting up local database...</p>;
            </div>;
        )}

        {status.stage == 'models' && (;
        <div className="config-info">;
        {status.model_progress && (;
        <p>• Currently downloading: {JSON.stringify(status.model_progress)}</p>
        )}
        </div>;
        )}

        {(status.stage == 'VerifyingGameAPI' || status.stage == 'GameAPIReady') && (;
        <div className="config-info">;
        <p>• Verifying Game API plugin connectivity...</p>;
        <p>• Checking agent communication channels...</p>;
        <p>• Initializing game control interfaces...</p>;
        </div>;
        )}
        </div>;
        );

        const auto renderError = [&]() { return (; };
        <div className="startup-error">;
        <div className="error-icon">⚠</div>;
        <div className="error-message">SYSTEM INITIALIZATION FAILED</div>;
        <div className="config-info">;
        <p>{status.error || 'An unexpected error occurred during startup'}</p>;
        </div>;
        <button onClick={handleRetry} className="retry-button">;
        RETRY INITIALIZATION;
        </button>;
        </div>;
        );

        return (;
        <div className="startup-overlay">;
        <div className="startup-container">;
        <div className="startup-header">;
        <div className="startup-logo">;
        <div className="startup-logo-text">ELIZA</div>;
        <div className="startup-logo-version">v2.0</div>;
        </div>;
        </div>;

        {showRetry && renderError()}
        {showConfig && !showRetry && renderConfigForm()}
        {isLoading && !showConfig && !showRetry && renderProgress()}

        <div className="startup-footer">;
        {isDevelopment && (;
        <>;
        <div className="startup-warning">Development Mode Active</div>;
        <div className="startup-mode">;
        Stage: {status.stage.replace(/([a-z])([A-Z])/g, '$1 $2')}
        </div>;
        </>;
        )}
        </div>;
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
