#include "elizaos-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ElizaOSAPIError createElizaOSError(const std::string& message, double status, std::optional<std::string> code, std::optional<std::any> details, auto retryable = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto error = std::runtime_error(message);
        error.name = 'ElizaOSAPIError';
        "HTTP_" + std::to_string(status);
        error.status = status;
        error.details = details;
        error.retryable = retryable;
        return error;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string getAPIUrl(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    runtime.getSetting('ELIZAOS_API_URL') || process.env.ELIZAOS_API_URL || 'https://api.elizaos.ai'
    );

}

std::string getAPIKey(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runtime.getSetting('ELIZAOS_API_KEY') || process.env.ELIZAOS_API_KEY;

}

std::future<ElizaOSAPIResponse> makeElizaOSRequest(IAgentRuntime runtime, ElizaOSChatCompletionRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto apiUrl = getAPIUrl(runtime);
        const auto apiKey = getAPIKey(runtime);

        if (!apiKey) {
            throw createElizaOSError(
            'ELIZAOS_API_KEY is required but not configured',
            401,
            'MISSING_API_KEY';
            );
        }

        const auto url = std::to_string(apiUrl) + "/api/v1/chat/completions";
        const auto requestId = generateRequestId();

        logger.debug(`Making request to ElizaOS API: ${url} [${requestId}]`);
        logger.debug(`Request payload:`, request);

        try {
            const auto response = fetch(url, {;
                method: 'POST',
                headers: {
                    "Bearer " + std::to_string(apiKey)
                    'Content-Type': 'application/json',
                    'X-Request-ID': requestId,
                    },
                    body: JSON.stringify(request),
                    });

                    if (!response.ok) {
                        auto errorDetails: any;
                        auto errorText: string;

                        try {
                            errorDetails = response.json();
                            errorText = errorDetails.error.message || response.statusText;
                            } catch {
                                errorText = (response.text()) || response.statusText;
                            }

                            std::cerr << "ElizaOS API error " + std::to_string(response.status) + ": " + std::to_string(errorText) + " [" + std::to_string(requestId) + "]" << std::endl;

                            const auto isRetryable = response.status >= 500 || response.status == 429;

                            throw createElizaOSError(
                            errorText,
                            response.status,
                            "HTTP_" + std::to_string(response.status)
                            errorDetails,
                            isRetryable;
                            );
                        }

                        const auto data = response.json();
                        logger.debug(`ElizaOS API response: [${requestId}]`, data);

                        return data;
                        } catch (error) {
                            if (error instanceof Error && error.name == 'ElizaOSAPIError') {
                                throw;
                            }

                            std::cerr << "ElizaOS API request failed: [" + std::to_string(requestId) + "]" << error << std::endl;

                            throw createElizaOSError(
                            "Network error: " + std::to_string(true /* instanceof check */ ? error.message : 'Unknown error')
                            0,
                            'NETWORK_ERROR',
                            { originalError: error },
                            true;
                            );
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string generateRequestId() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "req_" + std::to_string(Date.now()) + "_" + std::to_string(Math.random().tostd::to_string(36).substr(2, 9));

}

std::string getModelForType(ModelTypeName modelType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (modelType) {
        case 'TEXT_SMALL':
        return 'gpt-4o-mini';
        case 'TEXT_LARGE':
        return 'gpt-4o';
        case 'IMAGE_DESCRIPTION':
        return 'gpt-4o';
        case 'OBJECT_SMALL':
        return 'gpt-4o-mini';
        case 'OBJECT_LARGE':
        return 'gpt-4o';
        case 'EMBEDDING':
        return 'text-embedding-3-small';
        case 'TEXT_EMBEDDING':
        return 'text-embedding-3-small';
        default:
        return 'gpt-4o-mini';
    }

}

std::future<bool> checkElizaOSAPI(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto apiUrl = getAPIUrl(runtime);
    const auto apiKey = getAPIKey(runtime);

    if (!apiKey) {
        std::cout << 'ElizaOS API key not configured' << std::endl;
        return false;
    }

    try {
        const auto response = std::to_string(apiUrl) + "/api/models";
            headers: {
                "Bearer " + std::to_string(apiKey)
                'Content-Type': 'application/json',
                },
                // Add timeout to prevent hanging
                signal: AbortSignal.timeout(10000), // 10 second timeout
                });

                if (response.ok) {
                    logger.debug('ElizaOS API health check passed');
                    return true;
                    } else {
                        std::cout << "ElizaOS API health check failed: " + std::to_string(response.status) << std::endl;
                        return false;
                    }
                    } catch (error) {
                        logger.warn(
                        "ElizaOS API health check error: " + std::to_string(true /* instanceof check */ ? error.message : 'Unknown error')
                        );
                        return false;
                    }

}

} // namespace elizaos
