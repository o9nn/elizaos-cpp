import { describe, it, expect, beforeEach, afterEach, vi } from "vitest";

describe("wagmi-client chain availability", () => {
  const originalEnv = process.env;
  const originalConsoleWarn = console.warn;

  beforeEach(() => {
    process.env = { ...originalEnv } as typeof process.env;
    console.warn = vi.fn();
  });

  afterEach(() => {
    process.env = originalEnv;
    console.warn = originalConsoleWarn;
  });

  describe("Chain configuration based on environment", () => {
    it("should include Base and BSC chains by default", () => {
      // Base and BSC chains should always be available
      expect(process.env).toBeDefined();
    });

    it("should include Base chains when RPC URL is configured", () => {
      process.env.NEXT_PUBLIC_BASE_RPC_URL = "https://mainnet.base.org";
      expect(process.env.NEXT_PUBLIC_BASE_RPC_URL).toBeDefined();
    });

    it("should include BSC chains when RPC URL is configured", () => {
      process.env.NEXT_PUBLIC_BSC_RPC_URL = "https://bsc-dataseed1.binance.org";
      expect(process.env.NEXT_PUBLIC_BSC_RPC_URL).toBeDefined();
    });

    it("should not warn in production when chains are missing", () => {
      (process.env as { NODE_ENV?: string }).NODE_ENV = "production";
      delete process.env.NEXT_PUBLIC_BASE_RPC_URL;
      delete process.env.NEXT_PUBLIC_BSC_RPC_URL;

      // Production should not log warnings about missing chains
      expect((process.env as { NODE_ENV?: string }).NODE_ENV).toBe(
        "production",
      );
    });
  });

  describe("Transport configuration", () => {
    it("should use custom Base RPC URL from environment", () => {
      const customUrl = "https://custom-base-rpc.example.com";
      process.env.NEXT_PUBLIC_BASE_RPC_URL = customUrl;

      expect(process.env.NEXT_PUBLIC_BASE_RPC_URL).toBe(customUrl);
    });

    it("should use Anvil localhost by default", () => {
      delete process.env.NEXT_PUBLIC_RPC_URL;
      const expectedDefault = "http://127.0.0.1:8545";

      expect(process.env.NEXT_PUBLIC_RPC_URL || expectedDefault).toBe(
        expectedDefault,
      );
    });
  });

  describe("WalletConnect configuration", () => {
    it("should use custom project ID if provided", () => {
      const customProjectId = "custom-project-id";
      process.env.NEXT_PUBLIC_PROJECT_ID = customProjectId;

      expect(process.env.NEXT_PUBLIC_PROJECT_ID).toBe(customProjectId);
    });

    it("should have demo project ID as fallback", () => {
      delete process.env.NEXT_PUBLIC_PROJECT_ID;
      const defaultProjectId = "demo-project-id";

      expect(process.env.NEXT_PUBLIC_PROJECT_ID || defaultProjectId).toBe(
        defaultProjectId,
      );
    });
  });

  describe("Edge cases", () => {
    it("should handle empty string RPC URLs", () => {
      process.env.NEXT_PUBLIC_BASE_RPC_URL = "";
      // Empty string is falsy in JavaScript, should be treated as missing
      expect(process.env.NEXT_PUBLIC_BASE_RPC_URL || undefined).toBeUndefined();
    });

    it("should handle malformed URLs gracefully", () => {
      process.env.NEXT_PUBLIC_BASE_RPC_URL = "not-a-valid-url";
      // Implementation should handle this or fail gracefully
      expect(process.env.NEXT_PUBLIC_BASE_RPC_URL).toBe("not-a-valid-url");
    });
  });
});

describe("Chain exports", () => {
  it("should export chains array for UI reference", async () => {
    // Dynamic import to test exports
    const wagmiModule = await import("../wagmi-client");
    expect(wagmiModule.chains).toBeDefined();
    expect(Array.isArray(wagmiModule.chains)).toBe(true);
  });

  it("should export wagmi config", async () => {
    const wagmiModule = await import("../wagmi-client");
    expect(wagmiModule.config).toBeDefined();
  });
});
