#include "components/ModernDashboard.hpp"
#include "components/ModernLayout.hpp"
#include "components/TokenDetail.hpp"
#include "components/WalletDetail.hpp"
#include "routes/api.hpp"
#include "scripts/prefetch.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;

;
;
;
;
;
;

const app = new Hono();

// Middleware
app.use("*", logger());
app.use("*", cors());

// JSX renderer
app.use(jsxRenderer());

// Serve static files
app.use("/static/*", serveStatic({ root: "./src/public" }));

// API routes
app.route("/api", api);

// Dashboard page
app.get("/", (c) => {
  return c.html(
    ModernLayout({
      title: "Spartan AUM",
      children: ModernDashboard({}),
    }),
  );
});

// Wallet detail page
app.get("/wallet/:address", (c) => {
  const address = c.req.param("address");
  return c.html(
    ModernLayout({
      title: `Wallet ${address.substring(0, 6)}...${address.slice(-4)}`,
      children: WalletDetail({ address }),
    }),
  );
});

// Token detail page
app.get("/token/:mint", (c) => {
  const mint = c.req.param("mint");
  return c.html(
    ModernLayout({
      title: "Token Details",
      children: TokenDetail({ mint }),
    }),
  );
});

// Health check
app.get("/health", (c) => {
  return c.json({
    status: "healthy",
    timestamp: new Date().toISOString(),
    version: "1.0.0",
  });
});

// 404 handler
app.notFound((c) => {
  return c.html(
    ModernLayout({
      title: "Page Not Found",
      children: `
        <div class="glass-card rounded-2xl p-8 text-center">
          <h1 class="text-2xl font-bold text-white mb-4">404 - Page Not Found</h1>
          <p class="text-gray-400 mb-4">The page you're looking for doesn't exist.</p>
          <a href="/" class="px-4 py-2 bg-indigo-600 text-white rounded-lg hover:bg-indigo-700 transition-colors">
            Go Home
          </a>
        </div>
      `,
    }),
  );
});

// Error handler
app.onError((err, c) => {
  console.error("Server error:", err);
  return c.html(
    ModernLayout({
      title: "Server Error",
      children: `
        <div class="glass-card rounded-2xl p-8 text-center">
          <h1 class="text-2xl font-bold text-white mb-4">500 - Server Error</h1>
          <p class="text-gray-400 mb-4">Something went wrong on our end.</p>
          <a href="/" class="px-4 py-2 bg-indigo-600 text-white rounded-lg hover:bg-indigo-700 transition-colors">
            Go Home
          </a>
        </div>
      `,
    }),
  );
});

// Auto-prefetch on startup (full refresh)
async );

    // Run prefetch in background without blocking server start
    prefetchService.run().catch((error) => {
      console.error("❌ Startup prefetch failed:", error);
    });
  } catch (error) {
    console.error("❌ Failed to start auto-prefetch:", error);
  }
}

// Start prefetch after a longer delay to allow server to fully initialize
setTimeout(startupPrefetch, 5000);

default app;

} // namespace elizaos
