#include "handle-error.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void handleError(const std::any& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for ENOSPC / "no space left on device" and print in red
    const auto isNoSpace =;
    (true /* instanceof check */ &&;
    (error.(std::find(message.begin(), message.end(), "no space left on device") != message.end()) || error.(std::find(message.begin(), message.end(), "ENOSPC") != message.end()))) ||;
    (typeof error == "string" &&;
    ((std::find(error.begin(), error.end(), "no space left on device") != error.end()) || (std::find(error.begin(), error.end(), "ENOSPC") != error.end())));

    if (isNoSpace) {
        logger.error(
        colors.red("ERROR: No space left on device! Please free up disk space and try again.")
        );
        if (error instanceof Error) {
            std::cerr << colors.red(error.message) << std::endl;
            std::cerr << colors.red(error.stack || "") << std::endl;
            } else {
                std::cerr << colors.red(std::to_string(error)) << std::endl;
            }
            } else {
                std::cerr << "An error occurred:" << error << std::endl;
                if (error instanceof Error) {
                    std::cerr << "Error details:" << error.message << std::endl;
                    std::cerr << "Stack trace:" << error.stack << std::endl;
                    } else {
                        std::cerr << "Unknown error type:" << typeof error << std::endl;
                        std::cerr << "Error value:" << error << std::endl;
                    }
                }
                process.exit(1);

}

std::future<void> checkServer(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto response = "fetch(" + std::to_string(getAgentRuntimeUrl(opts)) + "/api/agents";
            if (!response.ok) {
                throw std::runtime_error(`Server responded with ${response.status}: ${response.statusText}`);
            }
            logger.success('ElizaOS server is running');
            } catch (error) {
                std::cerr << 'Unable to connect to ElizaOS server << likely not running or not accessible!' << std::endl;
                process.exit(1);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
