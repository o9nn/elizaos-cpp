#include "routes.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void sendSuccess(const std::any& res, const std::any& data, auto status = 200) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    res.writeHead(status, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ success: true, data }));

}

void sendError(const std::any& res, double status, const std::string& code, const std::string& message, std::optional<std::string> details) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    res.writeHead(status, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ success: false, error: { code, message, details } }));

}

std::future<void> getLeaderboardDataHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto service = runtime.getService<CommunityInvestorService>(ServiceType.COMMUNITY_INVESTOR);
        if (!service) {
            return sendError(res, 500, 'SERVICE_NOT_FOUND', 'CommunityInvestorService not found');
        }
        try {
            const auto leaderboardData = service.getLeaderboardData(runtime);
            // Return the leaderboard data directly as an array, not wrapped in an object
            sendSuccess(res, leaderboardData);
            } catch (error: any) {
                std::cerr << '[API /leaderboard] Error fetching leaderboard data:' << error << std::endl;
                sendError(res, 500, 'LEADERBOARD_ERROR', 'Failed to fetch leaderboard data', error.message);
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> communityInvestorPanelHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto pluginDistPath = path.dirname(new URL(import.meta.url).pathname.replace(/^\/[A-Z]:/, ''));
            const auto indexPath = path.join(pluginDistPath, 'index.html');

            logger.debug(`[COMMUNITY INVESTOR PANEL] pluginDistPath: ${pluginDistPath}`);
            logger.debug(`[COMMUNITY INVESTOR PANEL] indexPath: ${indexPath}`);
            logger.debug(`[COMMUNITY INVESTOR PANEL] File exists: ${fs.existsSync(indexPath)}`);

            if (fs.existsSync(indexPath)) {
                auto html = fs.promises.readFile(indexPath, 'utf8');
                const auto agentId = runtime.agentId;
                if (agentId) {
                    html = html.replace(;
                    '<head>',
                    "<head>\n    <script>window.elizaAgentId = "" + std::to_string(agentId) + "";</script>";
                    );
                    logger.debug(`[COMMUNITY INVESTOR PANEL] Injected agentId: ${agentId}`);
                    } else {
                        std::cout << '[COMMUNITY INVESTOR PANEL] AgentId not available in runtime for injection.' << std::endl;
                    }
                    logger.debug(
                    "[COMMUNITY INVESTOR PANEL] Serving HTML (first 250 chars after potential injection): " + std::to_string(html.substring(0, 250))
                    );
                    res.writeHead(200, { 'Content-Type': 'text/html' });
                    res.end(html);
                    } else {
                        std::cerr << "[COMMUNITY INVESTOR PANEL] Frontend index.html not found at " + std::to_string(indexPath) << std::endl;
                        // Fallback: serve a basic HTML page that loads the JS bundle from the assets folder
                        const auto html = `;
                        <!DOCTYPE html>;
                        <html lang="en">;
                        <head>;
                        <meta charset="UTF-8">;
                        <meta name="viewport" content="width=device-width, initial-scale=1.0">;
                        <title>Community Investor Leaderboard</title>;
                        <style>;
                    body { font-family: system-ui, -apple-system, sans-serif; margin: 0; padding: 20px; background: #0f0f0f; color: #fff; }
                .container { max-width: 1200px; margin: 0 auto; }
            .loading { text-align: center; padding: 40px; color: #666; }
        .error { text-align: center; padding: 40px; color: #ff6b6b; }
        </style>;
        </head>;
        <body>;
        <div class="container">;
        <div id="root">;
        <div class="error">;
        <h2>Community Investor Leaderboard</h2>;
        <p>Frontend assets not found. Please ensure the frontend is built correctly.</p>;
        <p>Run: <code>npm run build</code> in the spartan package directory.</p>
        <p>Looking for: ${indexPath}</p>
        </div>;
        </div>;
        </div>;
        </body>;
        </html>`;
        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.end(html);
        }
        } catch (error: any) {
            std::cerr << '[COMMUNITY INVESTOR PANEL] Error serving leaderboard panel:' << error << std::endl;
            sendError(res, 500, 'PANEL_ERROR', 'Failed to load leaderboard panel', error.message);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> communityInvestorAssetsHandler(const std::any& req, const std::any& res, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            logger.debug(
            "[COMMUNITY INVESTOR ASSET HANDLER] Called with req.path: " + std::to_string(req.path) + ", req.originalUrl: " + std::to_string(req.originalUrl)
            );
            const auto pluginDistPath = path.dirname(new URL(import.meta.url).pathname.replace(/^\/[A-Z]:/, ''));
            // When running from dist/index.js, pluginDistPath is the dist directory itself.
            // Assets are in a subfolder 'assets' within this dist directory.
            const auto assetsBasePath = path.join(pluginDistPath, 'assets');

            const auto requestedAssetPath = req.path;
            const auto assetsMarker = '/assets/';
            const auto assetsStartIndex = requestedAssetPath.indexOf(assetsMarker);

            if (assetsStartIndex == -1) {
                return sendError(res, 400, 'BAD_REQUEST', 'Invalid asset path');
            }
            const auto assetName = requestedAssetPath.substring(assetsStartIndex + assetsMarker.length);
            if (!assetName || assetName.includes('..')) {
                return sendError(;
                res,
                400,
                'BAD_REQUEST',
                "Invalid asset name: '" + std::to_string(assetName) + "' from path " + std::to_string(requestedAssetPath)
                );
            }

            const auto assetPath = path.join(assetsBasePath, assetName);
            logger.debug(`[COMMUNITY INVESTOR ASSET HANDLER] Attempting to serve asset: ${assetPath}`);

            if (fs.existsSync(assetPath)) {
                const auto fileStream = fs.createReadStream(assetPath);
                auto contentType = 'application/octet-stream'; // Default;
                if (assetPath.endsWith('.js')) contentType = 'application/javascript';
                else if (assetPath.endsWith('.css')) contentType = 'text/css';
                else if (assetPath.endsWith('.svg')) contentType = 'image/svg+xml';
                else if (assetPath.endsWith('.png')) contentType = 'image/png';
                else if (assetPath.endsWith('.jpg') || assetPath.endsWith('.jpeg'));
                contentType = 'image/jpeg';
                else if (assetPath.endsWith('.gif')) contentType = 'image/gif';
                else if (assetPath.endsWith('.ico')) contentType = 'image/x-icon';
                else if (assetPath.endsWith('.woff')) contentType = 'font/woff';
                else if (assetPath.endsWith('.woff2')) contentType = 'font/woff2';

                res.writeHead(200, { 'Content-Type': contentType });
                fileStream.pipe(res);
                } else {
                    std::cout << "[COMMUNITY INVESTOR ASSET HANDLER] Asset not found: " + std::to_string(assetPath) << std::endl;
                    "Asset not found: " + std::to_string(assetName)
                }
                } catch (error: any) {
                    std::cerr << "[COMMUNITY INVESTOR ASSET HANDLER] Error serving asset " + std::to_string(req.path) + ":" << error << std::endl;
                    sendError(res, 500, 'ASSET_ERROR', 'Failed to load asset', error.message);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
