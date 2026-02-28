import { register } from 'prom-client';
import Environment from '#/util/Environment.js';
import World from '#/engine/World.js';
import { handleClientPage, handleCacheEndpoints } from './pages/client.js';
import { handleHiscoresPage, handleHiscoresPlayerPage } from './pages/hiscores.js';
import { handleScriptRunsListPage, handleScriptRunsForScriptPage, handleScriptRunViewerPage, handleScriptRunFilesPage } from './pages/scriptRuns.js';
import { handleScreenshotsListPage, handleScreenshotFilePage } from './pages/screenshots.js';
import { handleScreenshotUpload, handleExportCollisionApi } from './pages/api.js';
import { handleDisclaimerPage, handlePublicFiles } from './pages/static.js';
import { WebSocketData, handleWebSocketUpgrade, handleGatewayEndpointGet, websocketHandlers } from './websocket.js';

export type { WebSocketData };

export type WebSocketRoutes = {
    '/': Response
};

export async function startWeb() {
    Bun.serve<WebSocketData, WebSocketRoutes>({
        port: Environment.WEB_PORT,
        async fetch(req, server) {
            const url = new URL(req.url ?? '', `http://${req.headers.get('host')}`);

            // Handle WebSocket upgrades first
            const wsResponse = handleWebSocketUpgrade(req, server, url);
            if (wsResponse !== undefined) {
                return wsResponse;
            }

            // Gateway endpoint GET request
            const gatewayResponse = handleGatewayEndpointGet(url);
            if (gatewayResponse) return gatewayResponse;

            // Engine status endpoint
            if (url.pathname === '/engine-status' || url.pathname === '/engine-status/') {
                return new Response(JSON.stringify({
                    status: 'running',
                    server: 'rs-agent-engine',
                    timestamp: new Date().toISOString(),
                    uptime: process.uptime(),
                    version: '1.0.0'
                }, null, 2), {
                    headers: {
                        'Content-Type': 'application/json',
                        'Access-Control-Allow-Origin': '*'
                    }
                });
            }

            // Player count endpoint
            if (url.pathname === '/playercount' || url.pathname === '/playercount/') {
                return new Response(JSON.stringify({
                    count: World.getTotalPlayers()
                }), {
                    headers: {
                        'Content-Type': 'application/json',
                        'Access-Control-Allow-Origin': '*'
                    }
                });
            }

            // Gateway status endpoint (proxy all bot statuses)
            if (url.pathname === '/status' || url.pathname === '/status/') {
                try {
                    const gatewayUrl = process.env.GATEWAY_URL || 'http://localhost:7780';
                    const response = await fetch(`${gatewayUrl}/status`);
                    const data = await response.json();
                    return new Response(JSON.stringify(data, null, 2), {
                        headers: {
                            'Content-Type': 'application/json',
                            'Access-Control-Allow-Origin': '*'
                        }
                    });
                } catch (error) {
                    return new Response(JSON.stringify({
                        error: 'Failed to fetch gateway status',
                        message: error instanceof Error ? error.message : 'Unknown error'
                    }, null, 2), {
                        status: 503,
                        headers: {
                            'Content-Type': 'application/json',
                            'Access-Control-Allow-Origin': '*'
                        }
                    });
                }
            }

            // Bot status endpoint (proxy to gateway)
            const botStatusMatch = url.pathname.match(/^\/status\/([^\/]+)\/?$/);
            if (botStatusMatch) {
                const username = botStatusMatch[1];
                try {
                    const gatewayUrl = process.env.GATEWAY_URL || 'http://localhost:7780';
                    const response = await fetch(`${gatewayUrl}/status/${username}`);
                    const data = await response.json();
                    return new Response(JSON.stringify(data, null, 2), {
                        status: response.status,
                        headers: {
                            'Content-Type': 'application/json',
                            'Access-Control-Allow-Origin': '*'
                        }
                    });
                } catch (error) {
                    return new Response(JSON.stringify({
                        error: 'Failed to fetch bot status from gateway',
                        message: error instanceof Error ? error.message : 'Unknown error'
                    }, null, 2), {
                        status: 503,
                        headers: {
                            'Content-Type': 'application/json',
                            'Access-Control-Allow-Origin': '*'
                        }
                    });
                }
            }

            // Client pages (/, /bot, /rs2.cgi)
            const clientResponse = await handleClientPage(url);
            if (clientResponse) return clientResponse;

            // Cache endpoints
            const cacheResponse = handleCacheEndpoints(url);
            if (cacheResponse) return cacheResponse;

            // Disclaimer page
            const disclaimerResponse = handleDisclaimerPage(url);
            if (disclaimerResponse) return disclaimerResponse;

            // API endpoints
            const screenshotUploadResponse = await handleScreenshotUpload(req, url);
            if (screenshotUploadResponse) return screenshotUploadResponse;

            const exportCollisionResponse = handleExportCollisionApi(url);
            if (exportCollisionResponse) return exportCollisionResponse;

            // Hiscores
            const hiscoresResponse = await handleHiscoresPage(url);
            if (hiscoresResponse) return hiscoresResponse;

            const hiscoresPlayerResponse = await handleHiscoresPlayerPage(url);
            if (hiscoresPlayerResponse) return hiscoresPlayerResponse;

            // Screenshots
            const screenshotsListResponse = handleScreenshotsListPage(url);
            if (screenshotsListResponse) return screenshotsListResponse;

            const screenshotFileResponse = handleScreenshotFilePage(url);
            if (screenshotFileResponse) return screenshotFileResponse;

            // Script runs
            const scriptRunsListResponse = handleScriptRunsListPage(url);
            if (scriptRunsListResponse) return scriptRunsListResponse;

            const scriptRunsForScriptResponse = handleScriptRunsForScriptPage(url);
            if (scriptRunsForScriptResponse) return scriptRunsForScriptResponse;

            const scriptRunViewerResponse = handleScriptRunViewerPage(url);
            if (scriptRunViewerResponse) return scriptRunViewerResponse;

            const scriptRunFilesResponse = handleScriptRunFilesPage(url);
            if (scriptRunFilesResponse) return scriptRunFilesResponse;

            // Public static files
            const publicFilesResponse = handlePublicFiles(url);
            if (publicFilesResponse) return publicFilesResponse;

            // 404
            return new Response(null, { status: 404 });
        },
        websocket: websocketHandlers
    });
}

export async function startManagementWeb() {
    Bun.serve({
        port: Environment.WEB_MANAGEMENT_PORT,
        routes: {
            '/prometheus': new Response(await register.metrics(), {
                headers: {
                    'Content-Type': register.contentType
                }
            })
        },
        fetch() {
            return new Response(null, { status: 404 });
        },
    });
}
