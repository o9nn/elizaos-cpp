"use client";
import "@/app/globals.css";

import { Suspense, useState, useCallback } from "react";
import dynamic from "next/dynamic";
import { Footer } from "@/components/footer";
import { useRenderTracker } from "@/utils/render-tracker";

const DealsGrid = dynamic(
  () => import("@/components/deals-grid").then((m) => m.DealsGrid),
  { ssr: false },
);
const DealFilters = dynamic(
  () => import("@/components/deal-filters").then((m) => m.DealFilters),
  { ssr: false },
);

const INITIAL_FILTERS = {
  chains: ["ethereum", "base", "bsc", "solana"] as (
    | "ethereum"
    | "base"
    | "bsc"
    | "solana"
  )[],
  minMarketCap: 0,
  maxMarketCap: 0,
  negotiableTypes: ["negotiable", "fixed"] as ("negotiable" | "fixed")[],
  searchQuery: "",
};

function MarketplaceContent() {
  useRenderTracker("MarketplaceContent");

  const [filters, setFilters] = useState(INITIAL_FILTERS);

  // Memoize the callback to prevent DealFilters re-renders
  const handleFiltersChange = useCallback(
    (newFilters: typeof INITIAL_FILTERS) => {
      setFilters(newFilters);
    },
    [],
  );

  return (
    <div className="relative flex flex-col h-full min-h-0">
      <main className="flex-1 flex flex-col min-h-0 px-4 sm:px-6 py-4 sm:py-8">
        <div className="max-w-7xl mx-auto w-full flex flex-col min-h-0 flex-1">
          {/* Filters - Fixed */}
          <div className="mb-4 flex-shrink-0">
            <DealFilters
              filters={filters}
              onFiltersChange={handleFiltersChange}
            />
          </div>

          {/* Deals Grid - Scrollable */}
          <div className="flex-1 min-h-0 overflow-y-auto">
            <DealsGrid filters={filters} searchQuery={filters.searchQuery} />
          </div>
        </div>
      </main>
      <Footer />
    </div>
  );
}

export default function Page() {
  return (
    <Suspense
      fallback={
        <div className="min-h-screen flex items-center justify-center">
          <div className="text-center">
            <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-blue-600 mx-auto mb-4"></div>
            <div className="text-xl text-zinc-600 dark:text-zinc-400">
              Loading OTC Marketplace...
            </div>
          </div>
        </div>
      }
    >
      <MarketplaceContent />
    </Suspense>
  );
}
