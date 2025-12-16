#include "..features/app/use-app-slice.hpp"
#include "..features/channel/use-channel-slice.hpp"
#include "..features/dm/use-dm-slice.hpp"
#include "..features/guild/use-guild-slice.hpp"
#include "..features/message/use-message-slice.hpp"
#include "..features/user/use-user-slice.hpp"
#include "..services/chrome-service.hpp"
#include ".channel-messages/channel-messages.hpp"
#include ".direct-messages/direct-messages.hpp"
#include ".tags/tags.hpp"
#include "components/about.hpp"
#include "components/announcement-component.hpp"
#include "components/close-window-button.hpp"
#include "components/donation-component.hpp"
#include "components/menu-bar.hpp"
#include "components/settings.hpp"
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
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;

 = useTheme();
  const [menuIndex, setMenuIndex] = useState(0);

  const { resetAdvancedFilters, resetMessageData, resetFilters } =
    useMessageSlice();

  const { resetGuild } = useGuildSlice();
  const { resetDm } = useDmSlice();
  const { resetChannel } = useChannelSlice();
  const { setDiscrubPaused, setSettings, state: appState } = useAppSlice();
  const settings = appState.settings();
  const { getUserData } = useUserSlice();

  const handleChangeMenuIndex = async (index: number) => {
    resetAdvancedFilters();
    resetMessageData();
    resetGuild();
    resetDm();
    resetChannel();
    resetFilters();
    setDiscrubPaused(false);
    setMenuIndex(index);
  };

  useEffect(() => {
    const init = async () => {
      const settings = await initializeSettings();
      setSettings(settings);
    };
    getUserData();
    init();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  return (
    <Box
      sx={{
        backgroundColor: palette.background.default,
        border: `1px solid ${palette.secondary.dark}`,
        wordWrap: "break-word",
        overflow: "hidden",
        borderRadius: "6px",
        boxShadow: "10px 11px 7px -1px rgba(0,0,0,0.41)",
        height: "615px",
        maxHeight: "615px",
        maxWidth: "720px",
        width: "720px",
        p: 0,
        m: 0,
      }}
    >
      <DonationComponent />
      <AnnouncementComponent />
      <MenuBar menuIndex={menuIndex} setMenuIndex={handleChangeMenuIndex} />
      {menuIndex === 0 && <ChannelMessages />}
      {menuIndex === 1 && <DirectMessages />}
      {menuIndex === 2 && <Tags />}
      {menuIndex === 3 && <About />}
      {menuIndex === 4 && (
        <Settings settings={settings} onChangeSettings={setSettings} />
      )}

      <Box sx={{ position: "fixed", top: "23px", right: "310px", opacity: 1 }}>
        <Stack
          direction="row"
          alignContent="center"
          justifyContent="center"
          spacing={1}
        >
          <Typography color="primary.main" variant="body2">
            1.12.3
          </Typography>
        </Stack>
      </Box>
      <CloseWindowButton />
    </Box>
  );
}

default DiscrubDialog;

} // namespace elizaos
