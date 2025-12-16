#include "elizaos/core.hpp"
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

const tokenContextProvider: Provider = {
  name: "TOKEN_CONTEXT",
  description: "Provides context about available tokens and OTC deals",
  position: 98,
  dynamic: true,
  get: async (
    runtime: IAgentRuntime,
    message: Memory,
  ): Promise<ProviderResult> => {
    const messageText = message.content?.text || "";

    const tokenMatch = messageText.match(/\b([A-Z]{2,6})\b/);
    let tokenId: string | null = null;

    if (tokenMatch) {
      const symbol = tokenMatch[1];
      const allTokens = await TokenDB.getAllTokens();
      const token = allTokens.find((t) => t.symbol === symbol);
      if (token) tokenId = token.id;
    }

    if (!tokenId) {
      const popularTokens = await TokenDB.getAllTokens({ isActive: true });
      const top5 = popularTokens.slice(0, 5);

      const consignmentCounts = await Promise.all(
        top5.map(async (t) => {
          const consignments = await ConsignmentDB.getConsignmentsByToken(t.id);
          return { token: t, count: consignments.length };
        }),
      );

      const text =
        `Available tokens for OTC deals:\n` +
        consignmentCounts
          .map(
            ({ token, count }) => `- ${token.symbol}: ${count} active deal(s)`,
          )
          .join("\n");

      return { text };
    }

    const token = await TokenDB.getToken(tokenId);
    const consignments = await ConsignmentDB.getConsignmentsByToken(tokenId);

    if (consignments.length === 0) {
      return {
        text: `Token: ${token.symbol}\nNo active OTC deals available for this token.`,
      };
    }

    const negotiableConsignments = consignments.filter((c) => c.isNegotiable);
    const fixedConsignments = consignments.filter((c) => !c.isNegotiable);

    let text = `Token: ${token.symbol}\n`;

    if (negotiableConsignments.length > 0) {
      // CONFIDENTIAL: Do not reveal actual min/max bounds or amounts to prevent gaming
      // The AI should negotiate naturally; server-side validation enforces bounds
      text += `Negotiable deals available: ${negotiableConsignments.length} listing(s) with terms open for negotiation\n`;
    }

    if (fixedConsignments.length > 0) {
      // Fixed terms are public since they're non-negotiable
      text += `Fixed price deals: `;
      text += fixedConsignments
        .map(
          (c) =>
            `${c.fixedDiscountBps / 100}% discount, ${c.fixedLockupDays} days lockup`,
        )
        .join("; ");
    }

    return { text };
  },
};

} // namespace elizaos
