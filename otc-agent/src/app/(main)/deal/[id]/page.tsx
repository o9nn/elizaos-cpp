"use client";

import { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import { DealCompletion } from "@/components/deal-completion";

// Force dynamic rendering for this route
export const dynamic = "force-dynamic";

export default function DealPage() {
  const params = useParams();
  const router = useRouter();
  const [quote, setQuote] = useState<any>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    async function loadDeal() {
      const quoteId = params.id as string;
      if (!quoteId) {
        router.push("/");
        return;
      }

      // Retry logic - service may not be ready immediately after redirect
      let retries = 3;
      let delay = 500;

      while (retries > 0) {
        console.log(`[DealPage] Fetching deal (${4 - retries}/3):`, quoteId);

        const response = await fetch(`/api/quote/executed/${quoteId}`, {
          cache: "no-store",
        });

        if (!response.ok) {
          const errorText = await response.text();
          console.warn("[DealPage] Fetch failed:", errorText);

          // Retry if service not ready
          if (errorText.includes("not registered") && retries > 1) {
            await new Promise((r) => setTimeout(r, delay));
            delay *= 2;
            retries--;
            continue;
          }

          throw new Error("Deal not found");
        }

        const data = await response.json();
        console.log("[DealPage] Quote loaded:", data.quote?.quoteId);
        setQuote(data.quote);
        setLoading(false);
        return;
      }
    }

    loadDeal();
  }, [params.id, router]);

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center">
        <div className="text-center">
          <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-green-500 mx-auto mb-4"></div>
          <div className="text-xl text-zinc-600 dark:text-zinc-400">
            Loading your deal...
          </div>
        </div>
      </div>
    );
  }

  if (!quote) {
    return (
      <div className="min-h-screen flex items-center justify-center">
        <div className="text-center">
          <h1 className="text-2xl font-bold text-zinc-900 dark:text-zinc-100 mb-2">
            Deal Not Found
          </h1>
          <p className="text-zinc-600 dark:text-zinc-400">
            This deal may have expired or doesn&apos;t exist.
          </p>
        </div>
      </div>
    );
  }

  return <DealCompletion quote={quote} />;
}
