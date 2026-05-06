#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "..components/profile/ApiSection.hpp"
#include "..components/profile/ProfileData.hpp"
#include "..components/profile/ProfileHoldings.hpp"
#include "..components/profile/ProfileTotals.hpp"
#include "..components/profile/ProfileWallets.hpp"
#include "..components/profile/Socials.hpp"
#include "index.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using View = std::variant<"profile", "holdings">;


} // namespace elizaos
