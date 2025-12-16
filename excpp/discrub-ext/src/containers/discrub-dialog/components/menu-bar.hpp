#include "...features/app/use-app-slice.hpp"
#include "...features/export/use-export-slice.hpp"
#include "...features/message/use-message-slice.hpp"
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

const MenuBar = ({
  menuIndex,
  setMenuIndex,
}: {
  menuIndex: number;
  setMenuIndex: (index: number) => Promise<void>;
}) => {
  const { state: exportState } = useExportSlice();
  const isExporting = exportState.isExporting();
  const isGenerating = exportState.isGenerating();

  const { state: appState } = useAppSlice();
  const task = appState.task();
  const { active } = task || {};

  const { state: messageState } = useMessageSlice();
  const isLoading = messageState.isLoading();

  const menuDisabled = !!(isExporting || isGenerating || active || isLoading);

  const [anchorEl, setAnchorEl] = useState<HTMLButtonElement | Maybe>(null);
  const menuOpen = !!anchorEl;

  const menuItems = [
    { name: "Channel Messages", icon: <ChatIcon /> },
    { name: "Direct Messages", icon: <EmailIcon /> },
    { name: "Tags", icon: <LoyaltyIcon /> },
    { name: "Change Log", icon: <DataObjectIcon /> },
    { name: "Settings", icon: <ManageAccountsIcon /> },
  ];

  const handleMenuClick = (e: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(e.currentTarget);
  };

  const redirects = [
    {
      title: "Star on GitHub",
      img: "github-logo.svg",
      url: "https://github.com/prathercc/discrub-ext",
    },
    {
      title: "Review on Webstore",
      img: "chromestore.svg",
      url: "https://chrome.google.com/webstore/detail/discrub/plhdclenpaecffbcefjmpkkbdpkmhhbj",
    },
    {
      title: "Donate with Ko-Fi",
      img: "kofi.svg",
      url: "https://ko-fi.com/prathercc",
    },
  ];

  return (
    <Box
      sx={{ marginLeft: "5px !important", marginTop: "5px !important" }}
      display="flex"
    >
      <Button
        disabled={menuDisabled}
        color="secondary"
        startIcon={menuOpen ? <MenuOpenIcon /> : <MenuIcon />}
        onClick={handleMenuClick}
      >
        Menu
      </Button>
      {redirects.map((redirect) => (
        <Button
          onClick={() => window.open(redirect.url, "_blank")}
          color="secondary"
          startIcon={
            <Icon>
              <img
                style={{ display: "flex", height: "inherit", width: "inherit" }}
                src={`resources/media/${redirect.img}`}
                alt={redirect.title}
              />
            </Icon>
          }
        >
          {redirect.title}
        </Button>
      ))}

      <Menu
        sx={{ textTransform: "none" }}
        open={menuOpen}
        anchorEl={anchorEl}
        onClose={() => setAnchorEl(null)}
      >
        {menuItems.map((menuItem, i) => (
          <>
            {menuItem.name === "Change Log" ? <Divider /> : null}
            <MenuItem
              key={menuItem.name}
              disabled={menuIndex === i}
              onClick={() => {
                setMenuIndex(i);
                setAnchorEl(null);
              }}
            >
              <ListItemIcon>{menuItem.icon}</ListItemIcon>
              <ListItemText>{menuItem.name}</ListItemText>
            </MenuItem>
          </>
        ))}
        <Divider />
        <MenuItem
          onClick={() => {
            window.open("https://www.reddit.com/r/discrub/", "_blank");
            setAnchorEl(null);
          }}
        >
          <ListItemIcon>
            <RedditIcon />
          </ListItemIcon>
          <ListItemText>Reddit</ListItemText>
        </MenuItem>
      </Menu>
    </Box>
  );
};

default MenuBar;

} // namespace elizaos
