#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Client-side helpers for X (Twitter) sharing via our backend endpoints

using XCredentials = {

using PendingShare = {

std::string getApiUrl();

std::optional<XCredentials> getXCreds();

void setPendingShare(PendingShare pending);

void clearPendingShare();

// Returns true if already authenticated (has oauth1 creds); false if redirected
bool ensureXAuth(std::optional<PendingShare> pending);

  // 1) Upload media

  // 2) Tweet

  // Clean URL param

std::future<void> safeText(const std::string& r);


} // namespace elizaos
