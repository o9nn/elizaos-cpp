// User session storage service using Eliza's cache system

import type { IAgentRuntime } from "@elizaos/core";
import { Service } from "@elizaos/core";
import { v4 as uuidv4 } from "uuid";

export interface UserSessionMemory {
  id: string;
  entityId: string;
  walletAddress: string;

  quotesCreated: number;
  lastQuoteAt: number;
  dailyQuoteCount: number;
  dailyResetAt: number;

  totalDeals: number;
  totalVolumeUsd: number;
  totalSavedUsd: number;

  createdAt: number;
  updatedAt: number;
}

const SESSION_KEY = (entityId: string) => `user_session:${entityId}`;
const MAX_DAILY_QUOTES = 25;

export class UserSessionStorageService extends Service {
  static serviceType = "UserSessionService";
  capabilityDescription = "User session and rate limiting service";

  constructor(runtime: IAgentRuntime) {
    super();
    this.runtime = runtime;
  }

  async initialize(): Promise<void> {
    console.log("[UserSessionService] Initialized");
  }

  async stop(): Promise<void> {
    // Cleanup if needed
  }

  static async start(
    runtime: IAgentRuntime,
  ): Promise<UserSessionStorageService> {
    const service = new UserSessionStorageService(runtime);
    await service.initialize();
    return service;
  }

  async getOrCreateSession(
    entityId: string,
    walletAddress: string,
  ): Promise<UserSessionMemory> {
    const existing = await this.runtime.getCache<UserSessionMemory>(
      SESSION_KEY(entityId),
    );

    if (existing) {
      const now = Date.now();
      const needsReset = now > existing.dailyResetAt + 24 * 60 * 60 * 1000;

      if (needsReset) {
        const updated: UserSessionMemory = {
          ...existing,
          dailyQuoteCount: 0,
          dailyResetAt: now,
          updatedAt: now,
        };
        await this.runtime.setCache(SESSION_KEY(entityId), updated);
        return updated;
      }
      return existing;
    }

    const now = Date.now();
    const newSession: UserSessionMemory = {
      id: uuidv4(),
      entityId,
      walletAddress,
      quotesCreated: 0,
      lastQuoteAt: 0,
      dailyQuoteCount: 0,
      dailyResetAt: now,
      totalDeals: 0,
      totalVolumeUsd: 0,
      totalSavedUsd: 0,
      createdAt: now,
      updatedAt: now,
    };

    await this.runtime.setCache(SESSION_KEY(entityId), newSession);
    return newSession;
  }

  async checkRateLimit(entityId: string, walletAddress: string): Promise<void> {
    const session = await this.getOrCreateSession(entityId, walletAddress);
    if (session.dailyQuoteCount >= MAX_DAILY_QUOTES) {
      throw new Error(
        `Daily quote limit of ${MAX_DAILY_QUOTES} reached for ${entityId}`,
      );
    }
  }

  async incrementQuoteCount(
    entityId: string,
    walletAddress: string,
  ): Promise<void> {
    const session = await this.getOrCreateSession(entityId, walletAddress);
    const now = Date.now();

    const updated: UserSessionMemory = {
      ...session,
      quotesCreated: session.quotesCreated + 1,
      dailyQuoteCount: session.dailyQuoteCount + 1,
      lastQuoteAt: now,
      updatedAt: now,
    };

    await this.runtime.setCache(SESSION_KEY(entityId), updated);
  }

  async updateDealStats(
    entityId: string,
    walletAddress: string,
    volumeUsd: number,
    savedUsd: number,
  ): Promise<void> {
    const session = await this.getOrCreateSession(entityId, walletAddress);

    const updated: UserSessionMemory = {
      ...session,
      totalDeals: session.totalDeals + 1,
      totalVolumeUsd: session.totalVolumeUsd + volumeUsd,
      totalSavedUsd: session.totalSavedUsd + savedUsd,
      updatedAt: Date.now(),
    };

    await this.runtime.setCache(SESSION_KEY(entityId), updated);
  }
}

export function getUserSessionStorageService(
  runtime: IAgentRuntime,
): UserSessionStorageService {
  return new UserSessionStorageService(runtime);
}
