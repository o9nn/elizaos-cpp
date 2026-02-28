// BotClient.ts - Client with Bot SDK enabled
// This is the entry point for the bot development client

export * from '#/client/Client.js';
export { Client as default } from '#/client/Client.js';

// Re-export Bot SDK for external access
export * from '#/bot/index.js';
