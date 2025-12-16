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

struct ResolveEnsParams {
    std::string query;
    std::optional<bool> includeFarcaster;
    std::optional<bool> includeExpiry;
};


using ResolveEnsInput = {
  query: string;
  includeFarcaster?: boolean;
  includeExpiry?: boolean;
};

using ResolveEnsActionResult = ActionResult & { input: ResolveEnsInput };

struct EnsDataResponse {
    std::optional<std::string> address;
    std::optional<std::string> ens;
    std::optional<std::string> ens_primary;
    std::optional<std::string> resolverAddress;
    std::optional<std::string> avatar;
    std::optional<std::string> avatar_small;
    std::optional<std::string> avatar_url;
    std::optional<std::string> contentHash;
    std::optional<std::string> description;
    std::optional<std::string> twitter;
    std::optional<std::string> github;
    std::optional<std::string> url;
    std::optional<std::string> header;
    std::optional<std::string> pgp;
    std::optional<std::string> expiry;
    std::optional<{> farcaster;
    std::optional<double> fid;
    std::optional<std::string> username;
    std::optional<std::string> display_name;
    std::optional<std::string> bio;
    std::optional<std::string> profile_url;
    std::optional<std::unordered_map<std::string, std::string>> wallets;
    std::optional<std::unordered_map<std::string, std::string>> records;
    std::optional<bool> error;
    std::optional<double> status;
    std::optional<std::string> message;
};


const buildRequestUrl = (params: ResolveEnsParams): string => {
  const trimmed = params.query.trim();
  const encodedQuery = encodeURIComponent(trimmed);
  const queryParams: string[] = [];

  if (params.includeFarcaster) {
    queryParams.push("farcaster=true");
  }

  if (params.includeExpiry) {
    queryParams.push("expiry=true");
  }

  const suffix = queryParams.length > 0 ? `?${queryParams.join("&")}` : "";
  return `https://ensdata.net/${encodedQuery}${suffix}`;
};

const formatSummary = (data: EnsDataResponse, query: string): string => {
  const ensName = data.ens ?? data.ens_primary ?? "Not set";
  const address = data.address ?? "Not found";
  const resolver = data.resolverAddress ?? "Not available";
  const avatarUrl = data.avatar_url ?? data.avatar ?? "Not set";
  const contentHash = data.contentHash ?? "Not set";
  const description = data.description ?? "None";
  const twitter = data.twitter ?? "Not linked";
  const github = data.github ?? "Not linked";
  const website = data.url ?? "Not provided";
  const expiry = data.expiry ?? "Unknown";

  let summary = ` **ENS Resolution for** \
\`${query}\`\n\n`;
  summary += ` **Primary ENS:** ${ensName}\n`;
  summary += ` **Resolved Address:** \`${address}\`\n`;
  summary += ` **Resolver Contract:** \`${resolver}\`\n`;
  summary += ` **Avatar:** ${avatarUrl}\n`;
  summary += ` **Content Hash:** ${contentHash}\n`;
  summary += ` **Description:** ${description}\n`;
  summary += ` **Website:** ${website}\n`;
  summary += ` **Twitter:** ${twitter}\n`;
  summary += ` **GitHub:** ${github}\n`;

  if (data.wallets) {
    const walletEntries = Object.entries(data.wallets);
    if (walletEntries.length > 0) {
      summary += `\n **Linked Wallets:**\n`;
      for (const [network, walletAddress] of walletEntries) {
        summary += `  • ${network.toUpperCase()}: \`${walletAddress}\`\n`;
      }
    }
  }

  if (data.farcaster) {
    const fc = data.farcaster;
    const fcUsername = fc.username ?? "Unknown";
    const fcDisplayName = fc.display_name ?? "Unknown";
    const fcBio = fc.bio ?? "None";
    summary += `\n **Farcaster Profile:**\n`;
    summary += `  • Username: ${fcUsername}\n`;
    summary += `  • Display Name: ${fcDisplayName}\n`;
    summary += `  • Bio: ${fcBio}\n`;
    if (fc.profile_url) {
      summary += `  • Profile: ${fc.profile_url}\n`;
    }
  }

  summary += `\n **Expiry:** ${expiry}`;

  return summary;
};

const cdpResolveEns: Action = {
  name: "RESOLVE_ENS",
  similes: [
    "ENS_LOOKUP",
    "ENS_RESOLVE",
    "ENS_RESOLVER",
    "LOOKUP_ENS",
    "RESOLVE_DOMAIN",
    "ENS_ADDRESS",
  ],
  description:
    "Resolves an ENS name or Ethereum address using ensdata.net. Returns associated primary ENS name, address, resolver, avatar, linked wallets, and optional Farcaster or expiry data.",
  parameters: {
    query: {
      type: "string",
      description:
        "ENS name (e.g., 'vitalik.eth') or Ethereum address to resolve. If an address is provided, returns the primary ENS and records if available.",
      required: true,
    },
    includeFarcaster: {
      type: "boolean",
      description:
        "Set to true to include Farcaster profile data when available.",
      required: false,
    },
    includeExpiry: {
      type: "boolean",
      description:
        "Set to true to include ENS expiry information when available.",
      required: false,
    },
  },
  validate: async (_runtime: IAgentRuntime, _message: Memory) => {
    return true;
  },
  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    _state?: State,
    _options?: Record<string, unknown>,
    callback?: HandlerCallback,
  ): Promise<ActionResult> => {
    try {
      const composedState = await runtime.composeState(message, ["ACTION_STATE"], true);
      const params = (composedState?.data?.actionParams ?? {}) as Partial<ResolveEnsParams>;

      if (!params.query || params.query.trim().length === 0) {
        const errorMsg = "ENS query is required. Provide an ENS name like 'vitalik.eth' or an Ethereum address.";
        logger.error(`[RESOLVE_ENS] ${errorMsg}`);
        const errorResult: ResolveEnsActionResult = {
          text: ` ${errorMsg}`,
          success: false,
          error: "missing_query",
          input: {
            query: "",
            includeFarcaster: params.includeFarcaster,
            includeExpiry: params.includeExpiry,
          },
        };
        callback?.({
          text: errorResult.text,
          content: { error: "missing_query", details: errorMsg },
        });
        return errorResult;
      }

      const resolvedParams: ResolveEnsParams = {
        query: params.query.trim(),
        includeFarcaster: params.includeFarcaster ?? false,
        includeExpiry: params.includeExpiry ?? false,
      };

      const url = buildRequestUrl(resolvedParams);

      callback?.({ text: ` Resolving ENS data for \`${resolvedParams.query}\`...` });
      logger.info(`[RESOLVE_ENS] Fetching ENS data from ${url}`);

      const response = await fetch(url, {
        headers: {
          Accept: "application/json",
        },
      });

      if (!response.ok) {
        const statusText = `${response.status} ${response.statusText}`;
        const errorMsg = `Failed to resolve ENS (${statusText}).`;
        logger.error(`[RESOLVE_ENS] ${errorMsg}`);
        const errorResult: ResolveEnsActionResult = {
          text: ` ${errorMsg}`,
          success: false,
          error: statusText,
          input: {
            query: resolvedParams.query,
            includeFarcaster: resolvedParams.includeFarcaster,
            includeExpiry: resolvedParams.includeExpiry,
          },
        };
        callback?.({
          text: errorResult.text,
          content: { error: "ens_lookup_failed", details: errorMsg },
        });
        return errorResult;
      }

      const data = (await response.json()) as EnsDataResponse;

      if (data.error) {
        const apiMessage = data.message ?? "ENS lookup failed.";
        logger.warn(`[RESOLVE_ENS] API error: ${apiMessage}`);
        const errorResult: ResolveEnsActionResult = {
          text: ` ${apiMessage}`,
          success: false,
          error: apiMessage,
          input: {
            query: resolvedParams.query,
            includeFarcaster: resolvedParams.includeFarcaster,
            includeExpiry: resolvedParams.includeExpiry,
          },
        };
        callback?.({
          text: errorResult.text,
          content: { error: "ens_lookup_failed", details: apiMessage },
        });
        return errorResult;
      }

      const summary = formatSummary(data, resolvedParams.query);

      const resultData = {
        query: resolvedParams.query,
        includeFarcaster: resolvedParams.includeFarcaster,
        includeExpiry: resolvedParams.includeExpiry,
        response: data,
      };

      callback?.({ text: summary, content: resultData });

      const successResult: ResolveEnsActionResult = {
        text: summary,
        success: true,
        data: resultData,
        values: resultData,
        input: {
          query: resolvedParams.query,
          includeFarcaster: resolvedParams.includeFarcaster,
          includeExpiry: resolvedParams.includeExpiry,
        },
      };

      return successResult;
    } catch (error) {
      const errorMsg = error instanceof Error ? error.message : String(error);
      logger.error(`[RESOLVE_ENS] Unexpected error: ${errorMsg}`);
      const failureText = ` Failed to resolve ENS: ${errorMsg}`;
      const failureResult: ResolveEnsActionResult = {
        text: failureText,
        success: false,
        error: errorMsg,
        input: {
          query: "",
        },
      };
      callback?.({
        text: failureText,
        content: { error: "ens_lookup_failed", details: errorMsg },
      });
      return failureResult;
    }
  },
  examples: [
    [
      { name: "{{user}}", content: { text: "resolve vitalik.eth" } },
      {
        name: "{{agent}}",
        content: {
          text: " Resolving ENS data for `vitalik.eth`...",
          action: "RESOLVE_ENS",
          query: "vitalik.eth",
        },
      },
    ],
    [
      { name: "{{user}}", content: { text: "lookup the ens for 0xd8dA6BF26964aF9D7eEd9e03E53415D37aA96045" } },
      {
        name: "{{agent}}",
        content: {
          text: " Resolving ENS data for `0xd8dA6BF26964aF9D7eEd9e03E53415D37aA96045`...",
          action: "RESOLVE_ENS",
          query: "0xd8dA6BF26964aF9D7eEd9e03E53415D37aA96045",
        },
      },
    ],
    [
      { name: "{{user}}", content: { text: "get ens info vitalik.eth including farcaster" } },
      {
        name: "{{agent}}",
        content: {
          text: " Resolving ENS data for `vitalik.eth`...",
          action: "RESOLVE_ENS",
          query: "vitalik.eth",
          includeFarcaster: true,
        },
      },
    ],
  ],
};

default cdpResolveEns;


} // namespace elizaos
