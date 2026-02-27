// GatewayConnection.ts - WebSocket connection to SDK gateway
// Handles connection, reconnection, message sending/receiving

import type { BotAction, BotWorldState } from './types.js';

export interface GatewayMessageHandler {
    onAction(action: BotAction, actionId: string | null): void;
    onScreenshotRequest(screenshotId?: string): void;
    onConnected(): void;
    onDisconnected(): void;
    /** Called when gateway requests graceful disconnect (new session taking over) */
    onSaveAndDisconnect(reason: string): void;
}

// Extract bot credentials from URL query params
function getUrlParams(): URLSearchParams | null {
    if (typeof window === 'undefined') return null;
    return new URLSearchParams(window.location.search);
}

export function getBotUsername(): string {
    const params = getUrlParams();
    return params?.get('bot') || 'default';
}

export function getBotPassword(): string | null {
    const params = getUrlParams();
    return params?.get('password') || null;
}

export class GatewayConnection {
    private ws: WebSocket | null = null;
    private reconnectTimer: number | null = null;
    private connected: boolean = false;
    private handler: GatewayMessageHandler;
    private botUsername: string;
    /** When true, prevents auto-reconnect (used during graceful disconnect) */
    private preventReconnect: boolean = false;

    constructor(handler: GatewayMessageHandler) {
        this.handler = handler;
        this.botUsername = getBotUsername();
    }

    connect(): void {
        if (this.ws) return;

        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const host = window.location.host;
        const url = `${protocol}//${host}/gateway`;

        try {
            this.ws = new WebSocket(url);

            this.ws.onopen = () => {
                this.connected = true;
                console.log(`[GatewayConnection] Connected, registering as '${this.botUsername}'`);

                // Register as bot with gateway
                this.send({
                    type: 'connected',
                    username: this.botUsername,
                    clientId: `${this.botUsername}-${Date.now()}`
                });

                this.handler.onConnected();
            };

            this.ws.onmessage = (event) => {
                try {
                    const msg = JSON.parse(event.data);
                    this.handleMessage(msg);
                } catch (e) {
                    console.error('[GatewayConnection] Failed to parse message:', e);
                }
            };

            this.ws.onclose = () => {
                console.warn(`[LOGOUT DEBUG] GatewayConnection WebSocket closed - preventReconnect=${this.preventReconnect}`);
                this.connected = false;
                this.ws = null;
                this.handler.onDisconnected();

                // Only reconnect if not explicitly told to disconnect
                if (!this.preventReconnect) {
                    console.warn('[LOGOUT DEBUG] GatewayConnection scheduling auto-reconnect in 3s');
                    if (this.reconnectTimer) clearTimeout(this.reconnectTimer);
                    this.reconnectTimer = window.setTimeout(() => this.connect(), 3000);
                } else {
                    console.log('[GatewayConnection] Auto-reconnect disabled (graceful disconnect)');
                }
            };

            this.ws.onerror = () => {
                // Will trigger onclose
            };
        } catch (e) {
            console.error('[GatewayConnection] Failed to connect:', e);
        }
    }

    disconnect(): void {
        if (this.reconnectTimer) {
            clearTimeout(this.reconnectTimer);
            this.reconnectTimer = null;
        }
        if (this.ws) {
            this.ws.close();
            this.ws = null;
        }
        this.connected = false;
    }

    isConnected(): boolean {
        return this.connected;
    }

    send(msg: any): void {
        if (this.ws?.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(msg));
        }
    }

    sendState(state: BotWorldState, formattedState: string): void {
        if (!this.connected) return;
        this.send({
            type: 'state',
            state,
            formattedState
        });
    }

    sendActionResult(actionId: string, result: { success: boolean; message: string; data?: any }): void {
        this.send({
            type: 'actionResult',
            actionId,
            result
        });
    }

    sendScreenshot(dataUrl: string, screenshotId?: string): void {
        this.send({
            type: 'screenshot_response',
            dataUrl,
            screenshotId
        });
    }

    private handleMessage(msg: any): void {
        if (msg.type === 'action') {
            console.log(`[GatewayConnection] Received action: ${msg.action?.type} (${msg.actionId})`);
            this.handler.onAction(msg.action, msg.actionId || null);
        } else if (msg.type === 'status') {
            console.log(`[GatewayConnection] Gateway status: ${msg.status}`);
        } else if (msg.type === 'screenshot_request') {
            this.handler.onScreenshotRequest(msg.screenshotId);
        } else if (msg.type === 'save_and_disconnect') {
            console.warn(`[LOGOUT DEBUG] GatewayConnection received save_and_disconnect: ${msg.reason}`);
            console.log(`[GatewayConnection] Received save_and_disconnect: ${msg.reason}`);
            // Set flag to prevent auto-reconnect before calling handler
            this.preventReconnect = true;
            this.handler.onSaveAndDisconnect(msg.reason || 'Session being replaced');
        }
    }
}
