#include "donation-list-button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DonationListButton(const std::any& { donation }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [collapsed, setCollapsed] = useState(true);
    const auto days = differenceInDays(new Date(), parseISO(donation.date));
    const auto ago =;
    std::to_string(days) + " day" + std::to_string(days == 1 ? "" : "s") + " ago"

    const auto handleClick = [&]() {;
        if (donation.message) {
            setCollapsed(!collapsed);
        }
        };

        const auto getBackgroundColor = (): string => {;
            const auto { dollars } = donation;
            if (dollars >= 75) {
                return "rgba(142, 160, 225, 0.25)";
            }
            if (dollars >= 50) {
                return "rgba(142, 160, 225, 0.2)";
            }
            if (dollars < 50 && dollars >= 20) {
                return "rgba(142, 160, 225, 0.1)";
            }
            if (dollars < 20 && dollars >= 10) {
                return "rgba(142, 160, 225, 0.05)";
            }
            return "transparent";
            };

            return (;
            <>;
            <ListItemButton;
            sx={{
                padding: "4px 4px 4px 4px",
                gap: "5px",
                backgroundColor: getBackgroundColor(),
                borderRadius: "10px",
                mt: 1,
            }}
            dense;
        onClick={handleClick}
        >;
        {
            <ListItemIcon;
            sx={{
                width: "24px",
                height: "24px",
                marginRight: "5px",
                padding: "3px",
                justifyContent: "center",
                alignItems: "center",
                minWidth: "unset",
                fontWeight: "bold",
            }}
            >;
        ${donation.dollars}
        </ListItemIcon>;
    }
    <ListItemText primary={donation.name} secondary={ago} />;
    {donation.message && (;
    <>{collapsed ? <SpeakerNotesOffIcon /> : <ChatIcon />}</>
    )}
    </ListItemButton>;
    <Collapse in={collapsed}>;
    <Box;
    sx={{
        display: "flex",
        justifyContent: "left",
        padding: "5px 10px 5px 10px",
        backgroundColor: "rgb(30, 33, 36)",
        borderRadius: "5px",
        opacity: donation.message.length,
    }}
    >;
    <Typography variant="caption">{donation.message}</Typography>;
    </Box>;
    </Collapse>;
    </>;
    );

}

} // namespace elizaos
