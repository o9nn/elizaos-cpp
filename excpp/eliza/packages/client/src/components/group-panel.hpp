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
// Based on MultiSelectCombobox.tsx: { icon: string; label: string; id?: string; }
struct ComboboxOption {
    std::string icon;
    std::string label;
    std::optional<string; // We will always provide agent.id here, which is UUID (string)> id;
};

struct GroupPanelProps {
    () => void onClose;
    std::optional<UUID> channelId;
};

struct ChannelParticipantsResponse {
    bool success;
    std::optional<std::vector<UUID>> data;
    std::optional<{ message?: string; code?: number | string }> error;

using SelectableAgent = Agent & { id: UUID; name: string };


} // namespace elizaos
