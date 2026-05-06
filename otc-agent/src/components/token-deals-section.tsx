"use client";

import { useMemo, useState } from "react";
import { useRouter } from "next/navigation";
import Image from "next/image";
import type {
  OTCConsignment,
  Token,
  TokenMarketData,
} from "@/services/database";

interface TokenDealsSectionProps {
  token: Token;
  marketData: TokenMarketData | null;
  consignments: OTCConsignment[];
}

function getDealTerms(c: OTCConsignment) {
  return c.isNegotiable
    ? { discountBps: c.maxDiscountBps ?? 0, lockupDays: c.maxLockupDays ?? 0 }
    : {
        discountBps: c.fixedDiscountBps ?? 0,
        lockupDays: c.fixedLockupDays ?? 0,
      };
}

function getDealScore(c: OTCConsignment) {
  const { discountBps, lockupDays } = getDealTerms(c);
  return discountBps - lockupDays; // higher discount, shorter lockup = better
}

export function TokenDealsSection({
  token,
  marketData,
  consignments,
}: TokenDealsSectionProps) {
  const router = useRouter();
  const [isExpanded, setIsExpanded] = useState(true);

  const formatAmount = (amount: string) => {
    const divisor = 10 ** token.decimals;
    const num = Number(amount) / divisor;
    if (num >= 1000000) return `${(num / 1000000).toFixed(2)}M`;
    if (num >= 1000) return `${(num / 1000).toFixed(2)}K`;
    return num.toFixed(2);
  };

  // Filter to only active consignments with remaining balance
  const activeConsignments = useMemo(
    () =>
      consignments.filter(
        (c) => c.status === "active" && BigInt(c.remainingAmount) > 0n,
      ),
    [consignments],
  );

  const sortedConsignments = useMemo(
    () =>
      [...activeConsignments].sort((a, b) => getDealScore(b) - getDealScore(a)),
    [activeConsignments],
  );

  const totalAvailable = activeConsignments.reduce(
    (sum, c) => sum + BigInt(c.remainingAmount),
    0n,
  );

  // Don't render if no active consignments
  if (activeConsignments.length === 0) {
    return null;
  }

  return (
    <div className="border border-zinc-200 dark:border-zinc-800 rounded-xl overflow-hidden">
      <div
        className="bg-zinc-50 dark:bg-zinc-900/50 p-4 cursor-pointer hover:bg-zinc-100 dark:hover:bg-zinc-900 transition-colors"
        onClick={() => setIsExpanded(!isExpanded)}
      >
        <div className="flex items-center justify-between">
          <div className="flex items-center gap-4 flex-1 min-w-0">
            {token.logoUrl ? (
              <Image
                src={token.logoUrl}
                alt={token.symbol}
                width={48}
                height={48}
                className="w-12 h-12 rounded-full flex-shrink-0"
              />
            ) : (
              <div className="w-12 h-12 rounded-full bg-gradient-to-br from-brand-400 to-brand-600 flex items-center justify-center text-white text-lg font-bold flex-shrink-0">
                {token.symbol.slice(0, 2).toUpperCase()}
              </div>
            )}
            <div className="flex-1 min-w-0">
              <div className="flex items-center gap-2">
                <h3 className="text-lg font-semibold truncate">
                  {token.symbol}
                </h3>
                <span className="text-sm text-zinc-500 dark:text-zinc-400 truncate">
                  {token.name}
                </span>
                {activeConsignments[0] && (
                  <span
                    className={`inline-flex items-center rounded-full px-2 py-0.5 text-xs font-medium ${
                      activeConsignments[0].chain === "base"
                        ? "bg-blue-600/15 text-blue-700 dark:text-blue-400"
                        : "bg-purple-600/15 text-purple-700 dark:text-purple-400"
                    }`}
                  >
                    {activeConsignments[0].chain.toUpperCase()}
                  </span>
                )}
              </div>
              <div className="flex items-center gap-4 mt-1 text-sm">
                {marketData?.priceUsd != null && (
                  <div>
                    <span className="text-zinc-600 dark:text-zinc-400">
                      Price:{" "}
                    </span>
                    <span className="font-medium">
                      $
                      {marketData.priceUsd < 0.0001
                        ? marketData.priceUsd.toExponential(2)
                        : marketData.priceUsd.toFixed(4)}
                    </span>
                  </div>
                )}
                <div>
                  <span className="font-medium">
                    {formatAmount(totalAvailable.toString())} {token.symbol}
                  </span>
                </div>
                <div>
                  <span className="text-zinc-600 dark:text-zinc-400">
                    Listings:{" "}
                  </span>
                  <span className="font-medium">
                    {activeConsignments.length}
                  </span>
                </div>
              </div>
            </div>
          </div>
          <svg
            className={`w-5 h-5 transition-transform ${isExpanded ? "rotate-180" : ""}`}
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth={2}
              d="M19 9l-7 7-7-7"
            />
          </svg>
        </div>
      </div>

      {isExpanded && (
        <div className="divide-y divide-zinc-200 dark:divide-zinc-800">
          {sortedConsignments.map((consignment) => {
            const { discountBps, lockupDays } = getDealTerms(consignment);
            const discountPct = (discountBps / 100).toFixed(1);
            return (
              <div
                key={consignment.id}
                className="p-4 hover:bg-zinc-50 dark:hover:bg-zinc-900/30 transition-colors cursor-pointer group"
                onClick={() => router.push(`/token/${token.id}`)}
              >
                <div className="flex items-center justify-between gap-4">
                  <span className="font-medium group-hover:text-brand-500 transition-colors">
                    {formatAmount(consignment.remainingAmount)} {token.symbol}
                  </span>
                  <div className="flex items-center gap-2">
                    <span className="text-sm text-zinc-500 dark:text-zinc-400">
                      {discountPct}% off · {lockupDays}d
                    </span>
                    {consignment.isNegotiable && (
                      <span className="inline-flex items-center rounded-full bg-brand-500/15 text-brand-600 dark:text-brand-400 px-2 py-1 text-xs font-medium">
                        Negotiable
                      </span>
                    )}
                    <svg
                      className="w-5 h-5 text-zinc-400 group-hover:text-brand-500 transition-colors"
                      fill="none"
                      stroke="currentColor"
                      viewBox="0 0 24 24"
                    >
                      <path
                        strokeLinecap="round"
                        strokeLinejoin="round"
                        strokeWidth={2}
                        d="M9 5l7 7-7 7"
                      />
                    </svg>
                  </div>
                </div>
              </div>
            );
          })}
        </div>
      )}
    </div>
  );
}
