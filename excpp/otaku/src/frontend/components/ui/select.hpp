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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void Select();

void SelectGroup();

void SelectValue();

void SelectTrigger(auto size, auto children);

void SelectContent(auto children, auto position);

void SelectLabel();

void SelectItem(auto children);

void SelectSeparator();

void SelectScrollUpButton();

void SelectScrollDownButton();


} // namespace elizaos
