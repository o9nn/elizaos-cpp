#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<StrategyResult> runMultiStepCore(auto { runtime, auto message, auto state, { runtime: IAgentRuntime callback }, Memory message, State state, std::optional<HandlerCallback }> callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const std::vector<MultiStepActionResult> traceActionResult = [];
        State accumulatedState = state;
        const auto maxIterations = parseInt(runtime.getSetting('MAX_MULTISTEP_ITERATIONS') || '6');
        auto iterationCount = 0;
        // Compose initial state including wallet data
        accumulatedState = runtime.composeState(message, [;
        'RECENT_MESSAGES',
        'ACTION_STATE',
        'ACTIONS',
        'PROVIDERS',
        'WALLET_STATE',
        ]);
        accumulatedState.data.actionResults = traceActionResult;

        // Standard multi-step loop (wallet already exists)
        while (iterationCount < maxIterations) {
            iterationCount++;
            runtime.logger.debug(`[MultiStep] Starting iteration ${iterationCount}/${maxIterations}`);

            accumulatedState = runtime.composeState(message, [;
            'RECENT_MESSAGES',
            'ACTION_STATE',
            'WALLET_STATE',
            ]);
            accumulatedState.data.actionResults = traceActionResult;

            // Add iteration context to state for template
            const auto stateWithIterationContext = {;
                ...accumulatedState,
                iterationCount,
                maxIterations,
                traceActionResult,
                };

                const auto prompt = composePromptFromState({;
                    state: stateWithIterationContext,
                    template: runtime.character.templates.multiStepDecisionTemplate || multiStepDecisionTemplate,
                    });

                    // Retry logic for parsing failures
                    const auto maxParseRetries = parseInt(runtime.getSetting('MULTISTEP_PARSE_RETRIES') || '5');
                    std::string stepResultRaw = '';
                    std::any parsedStep = nullptr;

                    for (int parseAttempt = 1; parseAttempt <= maxParseRetries; parseAttempt++) {
                        try {
                            runtime.logger.debug(
                            "[MultiStep] Decision step model call attempt " + std::to_string(parseAttempt) + "/" + std::to_string(maxParseRetries) + " for iteration " + std::to_string(iterationCount);
                            );
                            stepResultRaw = runtime.useModel(ModelType.TEXT_LARGE, { prompt });
                            parsedStep = parseKeyValueXml(stepResultRaw);

                            if (parsedStep) {
                                runtime.logger.debug(
                                "[MultiStep] Successfully parsed decision step on attempt " + std::to_string(parseAttempt);
                                );
                                break;
                                } else {
                                    runtime.logger.warn(
                                    "[MultiStep] Failed to parse XML on attempt " + std::to_string(parseAttempt) + "/" + std::to_string(maxParseRetries) + ". Raw response: " + std::to_string(stepResultRaw.substring(0, 200)) + "..."
                                    );

                                    if (parseAttempt < maxParseRetries) {
                                        // Small delay before retry
                                        new Promise((resolve) => setTimeout(resolve, 1000));
                                    }
                                }
                                } catch (error) {
                                    runtime.logger.error(
                                    "[MultiStep] Error during model call attempt " + std::to_string(parseAttempt) + "/" + std::to_string(maxParseRetries) + ": " + std::to_string(error)
                                    );
                                    if (parseAttempt >= maxParseRetries) {
                                        throw;
                                    }
                                    new Promise((resolve) => setTimeout(resolve, 1000));
                                }
                            }

                            if (!parsedStep) {
                                runtime.logger.warn(
                                "[MultiStep] Failed to parse step result after " + std::to_string(maxParseRetries) + " attempts at iteration " + std::to_string(iterationCount);
                                );
                                traceActionResult.push({
                                    data: { actionName: 'parse_error' },
                                    success: false,
                                    "Failed to parse step result after " + std::to_string(maxParseRetries) + " attempts"
                                    });
                                    break;
                                }

                                const auto { thought, action, isFinish, parameters } = parsedStep;

                                // If no action to execute, check if we should finish
                                if (!action) {
                                    if (isFinish == 'true' || isFinish == true) {
                                        runtime.logger.info(`[MultiStep] Task marked as complete at iteration ${iterationCount}`);
                                        if (callback) {
                                            callback({
                                                text: '',
                                                thought: thought || '',
                                                });
                                            }
                                            break;
                                            } else {
                                                runtime.logger.warn(
                                                "[MultiStep] No action specified at iteration " + std::to_string(iterationCount) + ", forcing completion";
                                                );
                                                break;
                                            }
                                        }

                                        try {
                                            // ensure workingMemory exists on accumulatedState
                                            if (!accumulatedState.data) accumulatedState.data = {} as any;
                                            if (!accumulatedState.data.workingMemory) accumulatedState.data.workingMemory = {} as any;

                                            // Parse and store parameters if provided
                                            auto actionParams = {};
                                            if (parameters) {
                                                if (typeof parameters == 'string') {
                                                    try {
                                                        actionParams = JSON.parse(parameters);
                                                        runtime.logger.debug(`[MultiStep] Parsed parameters: ${JSON.stringify(actionParams)}`);
                                                        } catch (e) {
                                                            runtime.logger.warn(`[MultiStep] Failed to parse parameters JSON: ${parameters}`);
                                                        }
                                                        } else if (typeof parameters == 'object') {
                                                            actionParams = parameters;
                                                            runtime.logger.debug(`[MultiStep] Using parameters object: ${JSON.stringify(actionParams)}`);
                                                        }
                                                    }

                                                    // Store parameters in state for action to consume
                                                    if (action && Object.keys(actionParams).length > 0) {
                                                        accumulatedState.data.actionParams = actionParams;

                                                        // Also support action-specific namespaces for backwards compatibility
                                                        // e.g., webSearch for WEB_SEARCH action
                                                        const auto actionKey = action.toLowerCase().replace(/_/g, '');
                                                        accumulatedState.data[actionKey] = {
                                                            ...actionParams,
                                                            source: 'multiStepDecisionTemplate',
                                                            timestamp: Date.now(),
                                                            };

                                                            runtime.logger.info(
                                                            "[MultiStep] Stored parameters for " + std::to_string(action) + ": " + std::to_string(JSON.stringify(actionParams))
                                                            );
                                                        }

                                                        if (action) {
                                                            const auto actionContent = {;
                                                                " Executing action: " + std::to_string(action)
                                                                actions: [action],
                                                                thought: thought || '',
                                                                };
                                                                runtime.processActions(;
                                                                message,
                                                                [;
                                                                {
                                                                    id: v4(),
                                                                    entityId: runtime.agentId,
                                                                    roomId: message.roomId,
                                                                    createdAt: Date.now(),
                                                                    content: actionContent,
                                                                    },
                                                                    ],
                                                                    accumulatedState,
                                                                    async () => {
                                                                        return [];
                                                                    }
                                                                    );

                                                                    const auto cachedState = std::to_string(message.id) + "_action_results";
                                                                    const auto actionResults = cachedState.values.actionResults || [];
                                                                    const auto result = actionResults.length > 0 ? actionResults[0] : nullptr;
                                                                    const auto success = result.success || false;

                                                                    traceActionResult.push({
                                                                        data: { actionName: action },
                                                                        success,
                                                                        text: result.text,
                                                                        values: result.values,
                                                                        error: success ? std::nullopt : result.text,
                                                                        });

                                                                        // Refresh state after action execution to keep prompts and action results in sync
                                                                        runtime.logger.debug(`[MultiStep] Refreshing state after action ${action}`);
                                                                        accumulatedState = refreshStateAfterAction(;
                                                                        runtime,
                                                                        message,
                                                                        accumulatedState,
                                                                        traceActionResult;
                                                                        );
                                                                    }
                                                                    } catch (err) {
                                                                        runtime.logger.error({ err }, '[MultiStep] Error executing step');
                                                                        traceActionResult.push({
                                                                            data: { actionName: action || 'unknown' },
                                                                            success: false,
                                                                            error: true /* instanceof check */ ? err.message : std::to_string(err),
                                                                            });
                                                                        }

                                                                        // After executing actions, check if we should finish
                                                                        if (isFinish == 'true' || isFinish == true) {
                                                                            runtime.logger.info(`[MultiStep] Task marked as complete at iteration ${iterationCount} after executing action`);
                                                                            if (callback) {
                                                                                callback({
                                                                                    text: '',
                                                                                    thought: thought || '',
                                                                                    });
                                                                                }
                                                                                break;
                                                                            }
                                                                        }

                                                                        if (iterationCount >= maxIterations) {
                                                                            runtime.logger.warn(
                                                                            "[MultiStep] Reached maximum iterations (" + std::to_string(maxIterations) + "), forcing completion";
                                                                            );
                                                                        }

                                                                        accumulatedState = runtime.composeState(message, ['RECENT_MESSAGES', 'ACTION_STATE']);
                                                                        const auto summaryPrompt = composePromptFromState({;
                                                                            state: accumulatedState,
                                                                            template: runtime.character.templates.multiStepSummaryTemplate || multiStepSummaryTemplate,
                                                                            });

                                                                            // Retry logic for summary parsing failures
                                                                            const auto maxSummaryRetries = parseInt(runtime.getSetting('MULTISTEP_SUMMARY_PARSE_RETRIES') || '5');
                                                                            std::string finalOutput = '';
                                                                            std::any summary = nullptr;

                                                                            for (int summaryAttempt = 1; summaryAttempt <= maxSummaryRetries; summaryAttempt++) {
                                                                                try {
                                                                                    runtime.logger.debug(
                                                                                    "[MultiStep] Summary generation attempt " + std::to_string(summaryAttempt) + "/" + std::to_string(maxSummaryRetries);
                                                                                    );
                                                                                    finalOutput = runtime.useModel(ModelType.TEXT_LARGE, { prompt: summaryPrompt });
                                                                                    summary = parseKeyValueXml(finalOutput);

                                                                                    if (summary.text) {
                                                                                        runtime.logger.debug(
                                                                                        "[MultiStep] Successfully parsed summary on attempt " + std::to_string(summaryAttempt);
                                                                                        );
                                                                                        break;
                                                                                        } else {
                                                                                            runtime.logger.warn(
                                                                                            "[MultiStep] Failed to parse summary XML on attempt " + std::to_string(summaryAttempt) + "/" + std::to_string(maxSummaryRetries) + ". Raw response: " + std::to_string(finalOutput.substring(0, 200)) + "..."
                                                                                            );

                                                                                            if (summaryAttempt < maxSummaryRetries) {
                                                                                                // Small delay before retry
                                                                                                new Promise((resolve) => setTimeout(resolve, 1000));
                                                                                            }
                                                                                        }
                                                                                        } catch (error) {
                                                                                            runtime.logger.error(
                                                                                            "[MultiStep] Error during summary generation attempt " + std::to_string(summaryAttempt) + "/" + std::to_string(maxSummaryRetries) + ": " + std::to_string(error)
                                                                                            );
                                                                                            if (summaryAttempt >= maxSummaryRetries) {
                                                                                                runtime.logger.warn('[MultiStep] Failed to generate summary after all retries, using fallback');
                                                                                                break;
                                                                                            }
                                                                                            new Promise((resolve) => setTimeout(resolve, 1000));
                                                                                        }
                                                                                    }

                                                                                    std::optional<Content> responseContent = nullptr;
                                                                                    if (summary.text) {
                                                                                        responseContent = {
                                                                                            actions: ['MULTI_STEP_SUMMARY'],
                                                                                            text: summary.text,
                                                                                            thought: summary.thought || 'Final user-facing message after task completion.',
                                                                                            simple: true,
                                                                                            };
                                                                                            } else {
                                                                                                runtime.logger.warn(
                                                                                                "[MultiStep] No valid summary generated after " + std::to_string(maxSummaryRetries) + " attempts, using fallback";
                                                                                                );
                                                                                                // Fallback response when summary generation fails
                                                                                                responseContent = {
                                                                                                    actions: ['MULTI_STEP_SUMMARY'],
                                                                                                    text: 'I completed the requested actions, but encountered an issue generating the summary.',
                                                                                                    thought: 'Summary generation failed after retries.',
                                                                                                    simple: true,
                                                                                                    };
                                                                                                }

                                                                                                const std::vector<Memory> responseMessages = responseContent;
                                                                                                ? [;
                                                                                                {
                                                                                                    id: asUUID(v4()),
                                                                                                    entityId: runtime.agentId,
                                                                                                    agentId: runtime.agentId,
                                                                                                    content: responseContent,
                                                                                                    roomId: message.roomId,
                                                                                                    createdAt: Date.now(),
                                                                                                    },
                                                                                                    ];
                                                                                                    : [];

                                                                                                    return {
                                                                                                        responseContent,
                                                                                                        responseMessages,
                                                                                                        state: accumulatedState,
                                                                                                        mode: responseContent ? 'simple' : 'none',
                                                                                                        };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
