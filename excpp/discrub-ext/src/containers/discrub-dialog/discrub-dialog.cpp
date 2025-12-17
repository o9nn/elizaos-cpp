#include "discrub-dialog.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DiscrubDialog() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { palette } = useTheme();
    const auto [menuIndex, setMenuIndex] = useState(0);

    const auto { resetAdvancedFilters, resetMessageData, resetFilters } =;
    useMessageSlice();

    const auto { resetGuild } = useGuildSlice();
    const auto { resetDm } = useDmSlice();
    const auto { resetChannel } = useChannelSlice();
    const auto { setDiscrubPaused, setSettings, state: appState } = useAppSlice();
    const auto settings = appState.settings();
    const auto { getUserData } = useUserSlice();

    const auto handleChangeMenuIndex = async (index: number) => {;
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
            const auto init = async () => {;
                const auto settings = initializeSettings();
                setSettings(settings);
                };
                getUserData();
                init();
                // eslint-disable-next-line react-hooks/exhaustive-deps
                }, []);

                return (;
                <Box;
                sx={{
                    backgroundColor: palette.background.default,
                    "1px solid " + std::to_string(palette.secondary.dark)
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
                >;
                <DonationComponent />;
                <AnnouncementComponent />;
                <MenuBar menuIndex={menuIndex} setMenuIndex={handleChangeMenuIndex} />;
            {menuIndex == 0 && <ChannelMessages />}
        {menuIndex == 1 && <DirectMessages />}
    {menuIndex == 2 && <Tags />}
    {menuIndex == 3 && <About />}
    {menuIndex == 4 && (;
    <Settings settings={settings} onChangeSettings={setSettings} />;
    )}

    <Box sx={{ position: "fixed", top: "23px", right: "310px", opacity: 1 }}>
    <Stack;
    direction="row";
    alignContent="center";
    justifyContent="center";
    spacing={1}
    >;
    <Typography color="primary.main" variant="body2">;
    1.12.3;
    </Typography>;
    </Stack>;
    </Box>;
    <CloseWindowButton />;
    </Box>;
    );

}

} // namespace elizaos
