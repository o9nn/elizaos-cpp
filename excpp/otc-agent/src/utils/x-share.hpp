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

// Client-side helpers for X (Twitter) sharing via our backend endpoints

using XCredentials = {
  entityId?: string;
  username?: string;
  screen_name?: string;
  oauth1Token?: string;
  oauth1TokenSecret?: string;
  accessToken?: string;
  refreshToken?: string;
  expiresAt?: number;
};

using PendingShare = {
  text: string;
  dataUrl: string;
};

const STORAGE_KEY = "twitter-oauth-token";
const OAUTH_REDIRECT_ORIGIN_KEY = "OAUTH_REDIRECT_ORIGIN";
const PENDING_DEAL_SHARE_KEY = "pending-deal-share";


  if (typeof window !== "undefined") {
    return window.location.origin;
  }
  return "";
}







// Returns true if already authenticated (has oauth1 creds); false if redirected
/api/share/oauth/request_token?oauth_version=1.0a`;
  return false;
}

std::future<{ success: boolean; tweetId?: string; tweetUrl?: string }> shareOnX(const std::string& text, const std::string& dataUrl, std::optional<XCredentials | null> creds);

  // 1) Upload media
  const uploadResp = await fetch(`${apiUrl}/api/share/upload-media`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "X-Twitter-OAuth-Token": c.oauth1Token,
      "X-Twitter-OAuth-Token-Secret": c.oauth1TokenSecret,
    },
    body: JSON.stringify({ image: dataUrl }),
  });
  if (!uploadResp.ok) {
    const err = await safeText(uploadResp);
    throw new Error(err || "Failed to upload media to X");
  }
  const uploadJson = (await uploadResp.json()) as { media_id_string?: string };
  const mediaId = uploadJson.media_id_string;
  if (!mediaId) throw new Error("No media_id returned from upload");

  // 2) Tweet
  const tweetResp = await fetch(`${apiUrl}/api/share/tweet`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      "X-Twitter-OAuth-Token": c.oauth1Token,
      "X-Twitter-OAuth-Token-Secret": c.oauth1TokenSecret,
    },
    body: JSON.stringify({ text, media_ids: [mediaId] }),
  });
  if (!tweetResp.ok) {
    const err = await safeText(tweetResp);
    throw new Error(err || "Failed to create tweet");
  }
  const posted = (await tweetResp.json()) as {
    tweet?: { data?: { id?: string } };
  };
  const id = posted?.tweet?.data?.id;
  const username = c.screen_name || c.username;
  const tweetUrl =
    id && username ? `https://twitter.com/${username}/status/${id}` : undefined;
  return { success: true, tweetId: id, tweetUrl };
}

std::future<
  | { resumed: false }
  | { resumed: true; success: boolean; error?: string; tweetUrl?: string }
> resumeFreshAuth();;
  const url = new URL(window.location.href);
  const isFresh = url.searchParams.get("fresh_auth") === "true";
  const pendingRaw = localStorage.getItem(PENDING_DEAL_SHARE_KEY);
  if (!isFresh || !pendingRaw) return { resumed: false };

  const pending = JSON.parse(pendingRaw) as PendingShare;
  const creds = getXCreds();
  if (!creds?.oauth1Token || !creds?.oauth1TokenSecret) {
    return {
      resumed: true,
      success: false,
      error: "Missing credentials after auth.",
    };
  }
  const result = await shareOnX(pending.text, pending.dataUrl, creds);
  clearPendingShare();
  // Clean URL param
  url.searchParams.delete("fresh_auth");
  window.history.replaceState({}, "", url.toString());
  return { resumed: true, success: true, tweetUrl: result.tweetUrl };
}

async 

const XShareStorageKeys = {
  STORAGE_KEY,
  OAUTH_REDIRECT_ORIGIN_KEY,
  PENDING_DEAL_SHARE_KEY,
};

} // namespace elizaos
