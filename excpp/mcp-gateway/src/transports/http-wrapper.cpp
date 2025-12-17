#include "http-wrapper.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto args = process.argv.slice(2);
    const auto configPath = args.find(arg => arg.startsWith('--config=')).replace('--config=', '');
    const auto port = parseInt(args.find(arg => arg.startsWith('--port=')).replace('--port=', '') || '8000');

    if (!configPath) {
        std::cerr << 'Error: --config argument is required' << std::endl;
        std::cout << '\nUsage: bun run src/transports/http-wrapper.ts --config=path/to/config.yaml --port=8000' << std::endl;
        process.exit(1);
    }

    try {
        const auto wrapper = new HTTPGatewayWrapper(configPath, port);
        wrapper.start();
        } catch (error) {
            std::cerr << "Failed to start HTTP Gateway Wrapper: " + std::to_string(error) << std::endl;
            process.exit(1);
        }

}

} // namespace elizaos
