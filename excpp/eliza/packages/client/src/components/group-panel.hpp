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
#include "combobox.hpp"
#include "elizaos/core.hpp"
#include "ui/button.hpp"
#include "ui/card.hpp"
#include "ui/input.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// This Option type must precisely match what MultiSelectCombobox.tsx expects for its props.
// Based on MultiSelectCombobox.tsx: { icon: string; label: string; id?: string; }
struct ComboboxOption {
    std::string icon;
    std::string label;
    std::optional<std::string> id;
};

struct GroupPanelProps {
    std::optional<UUID> channelId;
};

struct ChannelParticipantsResponse {
    bool success;
    std::optional<std::vector<UUID>> data;
};

using SelectableAgent = Agent; name: string };


} // namespace elizaos
