#include "...enum/discrub-setting.hpp"
#include "...features/app/app-types.hpp"
#include "config.hpp"
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

;
;
;
;

using SettingsProps = {
  settings: AppSettings;
  onChangeSettings: (settings: AppSettings) => void;
};

: SettingsProps) {
  const visibleSettings = [
    DiscrubSetting.REACTIONS_ENABLED,
    DiscrubSetting.SERVER_NICKNAME_LOOKUP,
    DiscrubSetting.DISPLAY_NAME_LOOKUP,
    DiscrubSetting.RANDOM_DELETE_DELAY,
    DiscrubSetting.RANDOM_SEARCH_DELAY,
  ];

  return (
    <Paper sx={{ padding: "10px", margin: "10px 10px 0px 10px" }}>
      <Stack
        justifyContent="center"
        alignItems="flex-start"
        direction="column"
        spacing={1}
      >
        <Typography variant="body1"> Settings</Typography>
        <Typography variant="caption">Customize your experience</Typography>
      </Stack>

      <Stack sx={{ maxHeight: "600px", overflow: "auto" }}>
        <Config
          settings={settings}
          visibleSettings={visibleSettings}
          onChangeSettings={onChangeSettings}
        />
      </Stack>
    </Paper>
  );
}

default Settings;

} // namespace elizaos
