#include "settings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Settings(auto { settings, SettingsProps onChangeSettings }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto visibleSettings = [;
    DiscrubSetting.REACTIONS_ENABLED,
    DiscrubSetting.SERVER_NICKNAME_LOOKUP,
    DiscrubSetting.DISPLAY_NAME_LOOKUP,
    DiscrubSetting.RANDOM_DELETE_DELAY,
    DiscrubSetting.RANDOM_SEARCH_DELAY,
    ];

    return (;
    <Paper sx={{ padding: "10px", margin: "10px 10px 0px 10px" }}>
    <Stack;
    justifyContent="center";
    alignItems="flex-start";
    direction="column";
    spacing={1}
    >;
    <Typography variant="body1"> Settings</Typography>;
    <Typography variant="caption">Customize your experience</Typography>;
    </Stack>;

    <Stack sx={{ maxHeight: "600px", overflow: "auto" }}>
    <Config;
    settings={settings}
    visibleSettings={visibleSettings}
    onChangeSettings={onChangeSettings}
    />;
    </Stack>;
    </Paper>;
    );

}

} // namespace elizaos
