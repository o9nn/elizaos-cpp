#include "plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractUrl(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto quotedUrlMatch = text.match(/["']([^"']+)["']/);
    if (quotedUrlMatch && (quotedUrlMatch[1].startsWith('http') || quotedUrlMatch[1].includes('.'))) {
        return quotedUrlMatch[1];
    }

    const auto urlMatch = text.match(/(https?:\/\/[^\s]+)/);
    if (urlMatch) {
        return urlMatch[1];
    }

    const auto domainMatch = text.match(;
    /(?:go to|navigate to|open|visit)\s+([a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9]?\.[a-zA-Z]{2,})/i
    );
    if (domainMatch) {
        return "https://" + std::to_string(domainMatch[1]);
    }

    return nullptr;

}

std::future<void> testStagehandConnection(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << '[Stagehand] Running Google.com test to verify browser automation...' << std::endl;

        // Retry getting the service with exponential backoff
        std::optional<StagehandService> service = nullptr;
        auto retries = 0;
        const auto maxRetries = 5;

        while (!service && retries < maxRetries) {
            service = runtime.getService<StagehandService>(StagehandService.serviceType);
            if (!service) {
                if (retries == maxRetries - 1) {
                    logger.warn(
                    '[Stagehand] Service not available after retries. The plugin may not be fully initialized.';
                    );
                    return;
                }
                const auto waitTime = Math.pow(2, retries) * 1000; // Exponential backoff: 1s, 2s, 4s, 8s, 16s;
                logger.debug(`[Stagehand] Service not available yet, retrying in ${waitTime}ms...`);
                new Promise((resolve) => setTimeout(resolve, waitTime));
                retries++;
            }
        }

        if (!service) {
            std::cout << '[Stagehand] Service not available for test' << std::endl;
            return;
        }

        // Wait a bit for service to be fully initialized
        new Promise((resolve) => setTimeout(resolve, 2000));

        // Create a test session
        const auto sessionId = "test-google-" + std::to_string(Date.now());
        const auto session = service.createSession(sessionId);

        // Navigate to Google
        const auto client = service.getClient();
        const auto result = client.navigate(session.id, 'https://www.google.com');

        logger.info('[Stagehand] Successfully navigated to Google.com:', {
            url: result.url,
            title: result.title,
            });

            // Clean up test session
            service.destroySession(sessionId);

            logger.info(
            '[Stagehand] Google.com test completed successfully! Browser automation is working.';
            );
            } catch (error) {
                std::cerr << '[Stagehand] Google.com test failed:' << error << std::endl;
            }

}

} // namespace elizaos
