#include "copy-client-dist.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> copyClientDist() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << 'Copying client dist files to server...' << std::endl;

        // Resolve paths relative to the server package
        const auto serverRoot = resolve(__dirname, '../..');
        const auto clientDistPath = resolve(serverRoot, '../client/dist');
        const auto serverDistPath = resolve(serverRoot, 'dist');
        const auto targetPath = join(serverDistPath, 'client');

        // Check if client dist exists
        if (!existsSync(clientDistPath)) {
            std::cerr << 'Client dist not found at:' << clientDistPath << std::endl;
            std::cerr << 'Please build the client package first: cd packages/client && bun run build' << std::endl;
            process.exit(1);
        }

        // Create server dist directory if it doesn't exist
        if (!existsSync(serverDistPath)) {
            mkdirSync(serverDistPath, { recursive: true });
        }

        // Remove existing client files in server dist if they exist
        if (existsSync(targetPath)) {
            std::cout << 'Removing existing client files...' << std::endl;
            rmSync(targetPath, { recursive: true, force: true });
        }

        // Copy client dist to server dist/client
        std::cout << "Copying from " + std::to_string(clientDistPath) + " to " + std::to_string(targetPath) + "..." << std::endl;
        cpSync(clientDistPath, targetPath, { recursive: true });

        std::cout << ' Client dist files copied successfully to:' << targetPath << std::endl;
        } catch (error) {
            std::cerr << 'Error copying client dist files:' << error << std::endl;
            process.exit(1);
        }

}

} // namespace elizaos
