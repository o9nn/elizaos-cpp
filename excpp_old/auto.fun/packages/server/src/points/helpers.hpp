#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".db.hpp"
#include ".env.hpp"
#include ".migration/migrations.hpp"
#include ".redis.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// point events for now
using PointEvent = std::variant<, std::any, std::any, { type: "prebond_buy">; usdVolume: number }

// helper calc points per event
double calculatePoints(PointEvent evt);

std::future<void> awardUserPoints(const std::string& userAddress, PointEvent event, auto description = "");

std::future<void> awardGraduationPoints(const std::string& mint);

/* Malibu To do: add this to a cron job to run once a week */

  // 2) Compute sum of all points

  // calculate the users share and apply cap


} // namespace elizaos
