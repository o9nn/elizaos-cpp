#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "classes/channel.hpp"
#include "classes/message.hpp"
#include "enum/filter-name.hpp"
#include "enum/filter-type.hpp"
#include "enum/has-type.hpp"
#include "enum/sort-direction.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using MessageState = std::vector<{

using Filter = std::variant<, {

using DeleteConfiguration = {

using MessageData = std::vector<{

using SearchMessageProps = std::variant<{

using MessageSearchOptions = std::variant<{

} // namespace elizaos
