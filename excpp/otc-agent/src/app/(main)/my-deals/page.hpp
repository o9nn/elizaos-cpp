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

"use client";

;
;
;

// Dynamically import the deals component to avoid SSR issues with wagmi hooks
const MyDealsContent = dynamic(
  () => import("./MyDealsContent").then((mod) => mod.MyDealsContent),
  {
    ssr: false,
    loading: () => (
      <main className="flex-1 min-h-[60dvh] flex items-center justify-center">
        <div className="text-center space-y-4">
          <h1 className="text-2xl font-semibold">My Deals</h1>
          <p className="text-zinc-600 dark:text-zinc-400">Loading...</p>
        </div>
      </main>
    ),
  },
);

default 
      >
        <MyDealsContent />
      </Suspense>
      <Footer />
    </>
  );
}

} // namespace elizaos
