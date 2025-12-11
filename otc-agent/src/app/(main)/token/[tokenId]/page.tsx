"use client";

import { useParams } from "next/navigation";
import dynamicImport from "next/dynamic";
import { useTokenCache, useMarketDataRefresh } from "@/hooks/useTokenCache";
import { Footer } from "@/components/footer";

const Chat = dynamicImport(() => import("@/components/chat"), { ssr: false });

export const dynamic = "force-dynamic";

export default function TokenPage() {
  const params = useParams();
  const tokenId = params.tokenId as string;
  const {
    token,
    marketData: initialMarketData,
    isLoading: loading,
  } = useTokenCache(tokenId);
  const refreshedMarketData = useMarketDataRefresh(tokenId, token);
  const marketData = refreshedMarketData || initialMarketData;

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center">
        <div className="text-center">
          <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-blue-600 mx-auto mb-4"></div>
          <div className="text-xl text-zinc-600 dark:text-zinc-400">
            Loading token data...
          </div>
        </div>
      </div>
    );
  }

  if (!token) {
    return (
      <div className="min-h-screen flex items-center justify-center">
        <div className="text-center">
          <h1 className="text-2xl font-bold">Token Not Found</h1>
          <p className="text-zinc-600 dark:text-zinc-400 mt-2">
            This token may not be registered yet.
          </p>
        </div>
      </div>
    );
  }

  return (
    <div className="flex flex-col h-full overflow-hidden">
      <main className="flex-1 flex flex-col min-h-0 overflow-y-auto">
        <div className="flex-1 flex flex-col min-h-0 px-3 sm:px-4 md:px-6 py-3 sm:py-4">
          <div className="max-w-7xl mx-auto w-full flex-1 flex flex-col min-h-0">
            {/* Chat section with token header inside */}
            <div className="flex-1 min-h-0">
              <Chat token={token} marketData={marketData} />
            </div>
          </div>
        </div>
      </main>
      <Footer />
    </div>
  );
}
