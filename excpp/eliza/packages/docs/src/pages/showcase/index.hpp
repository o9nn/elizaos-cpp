#include "_components/ShowcaseCards.hpp"
#include "_components/ShowcaseFilters.hpp"
#include "_components/ShowcaseLayout.hpp"
#include "_components/ShowcaseLayout/styles.module.css.hpp"
#include "_components/ShowcaseSearchBar.hpp"
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



void ShowcaseHeader();

void filterUsers(const std::vector<User>& users, const std::string& search, const std::vector<std::string>& selectedTags, 'OR' | 'AND' operator);


} // namespace elizaos
