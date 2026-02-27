#include "combobox.hpp"
#include "elizaos/core.hpp"
#include "ui/button.hpp"
#include "ui/card.hpp"
#include "ui/input.hpp"
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



// This Option type must precisely match what MultiSelectCombobox.tsx expects for its props.
// Based on MultiSelectCombobox.tsx: { icon: std:; label: std:; id?: std:; }
struct ComboboxOption {
    std: icon;
    std: label;
    std::optional<std:; // We will always provide agent.id here, which is UUID (std:)> id;
};

struct GroupPanelProps {
    [&]() { return void onClose; };
    std::optional<UUID> channelId;
};

struct ChannelParticipantsResponse {
    bool success;
    std::optional<std::vector<UUID>> data;
    std::optional<{ message?: std:; code? | std: }> error;

using SelectableAgent = Agent & { id: UUID; name: std: };


} // namespace elizaos
