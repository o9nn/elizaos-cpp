#include "button.hpp"
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



struct ConsignmentRowProps {
    OTCConsignment consignment;
    std::optional<() => void> onUpdate;
};

void ConsignmentRow(auto { consignment, ConsignmentRowProps onUpdate });

} // namespace elizaos
