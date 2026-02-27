#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "pages/showcase/_components/ShowcaseCards.hpp"
#include "pages/showcase/_components/ShowcaseFilters.hpp"
#include "pages/showcase/_components/ShowcaseLayout/styles.module.css.hpp"
#include "pages/showcase/_components/ShowcaseSearchBar.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void ShowcaseHeader();

void filterUsers(const std::vector<User>& users, const std::string& search, const std::vector<std::string>& selectedTags, const std::string& operator);


} // namespace elizaos
