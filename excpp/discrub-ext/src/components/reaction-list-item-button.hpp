#include ".classes/emoji.hpp"
#include ".common-components/tooltip/tooltip.hpp"
#include ".utils.hpp"
#include "server-emoji.hpp"
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

using ReactingUser = std::variant<{
  displayName: string, Maybe>;
  userName: string | Maybe;
  id: Snowflake;
  avatar: string | Maybe;
  burst: boolean;
};

using ReactionListItemButtonProps = {
  emoji: Emoji;
  reactingUsers: ReactingUser[];
  currentUserId: Snowflake | Maybe;
  disabled: boolean;
  onReactionDelete: () => void;
};

const ReactionListItemButton = ({
  emoji,
  reactingUsers,
  currentUserId,
  disabled,
  onReactionDelete,
}: ReactionListItemButtonProps) => {
  const theme = useTheme();
  const [expanded, setExpanded] = useState(false);
  const [emojiCopied, setEmojiCopied] = useState(false);

  const handleClick = () => {
    setExpanded((prevState) => !prevState);
  };

  const handleEmojiClick = (e: React.MouseEvent<HTMLElement>) => {
    e.stopPropagation();
    if (emoji.name) {
      copy(emoji.id ? `:${emoji.name}:` : emoji.name);
      setEmojiCopied(true);
    }
  };

  const getReactingUserAvatar = (reactingUser: ReactingUser) => {
    return (
      <img
        style={{ width: "40px", height: "40px", borderRadius: "50%" }}
        src={resolveAvatarUrl(reactingUser.id, reactingUser.avatar).remote}
        alt="avatar-icon"
      />
    );
  };

  const getUserListItem = (props: ListChildComponentProps) => {
    const { style, index } = props;
    const rUser = reactingUsers[index];
    return (
      <ListItem style={style} key={index} dense sx={{ pl: 4 }}>
        <ListItemIcon>{getReactingUserAvatar(rUser)}</ListItemIcon>
        <ListItemText
          primary={rUser.displayName || "Missing Display Name"}
          secondary={rUser.userName || "Missing User Name"}
        />
        <Box
          sx={{
            display: "flex",
            justifyContent: "center",
            alignItems: "center",
            gap: "5px",
          }}
        >
          {rUser.burst && (
            <Tooltip title="Super Reaction">
              <VerifiedIcon sx={{ color: theme.palette.secondary.main }} />
            </Tooltip>
          )}
          {currentUserId === rUser.id && (
            <Tooltip title="Delete Reaction">
              <IconButton disabled={disabled} onClick={onReactionDelete}>
                <DeleteForeverIcon color="error" />
              </IconButton>
            </Tooltip>
          )}
        </Box>
      </ListItem>
    );
  };

  const remoteEmojiUrl = resolveEmojiUrl(emoji.id).remote;

  return (
    <>
      <ListItemButton dense onClick={handleClick}>
        <Tooltip title={emoji.id ? `:${emoji.name}:` : `${emoji.name}`}>
          <ListItemIcon onClick={handleEmojiClick}>
            {emoji.id && remoteEmojiUrl ? (
              <ServerEmoji url={remoteEmojiUrl} />
            ) : (
              emoji.name
            )}
          </ListItemIcon>
        </Tooltip>
        <ListItemText
          secondary={`${reactingUsers.length} User${
            reactingUsers.length > 1 ? "s" : ""
          }`}
        />
        {expanded ? <ExpandLess /> : <ExpandMore />}
      </ListItemButton>
      <Collapse in={expanded} unmountOnExit>
        <FixedSizeList
          height={reactingUsers.length > 3 ? 200 : reactingUsers.length * 60}
          width="100%"
          itemSize={60}
          itemCount={reactingUsers.length}
        >
          {getUserListItem}
        </FixedSizeList>
      </Collapse>
      <Snackbar
        anchorOrigin={{ vertical: "top", horizontal: "center" }}
        autoHideDuration={2000}
        open={emojiCopied}
        onClose={() => {
          setEmojiCopied(false);
        }}
      >
        <Alert variant="filled" severity="info">
          <span>Copied to clipboard</span>
        </Alert>
      </Snackbar>
    </>
  );
};
default ReactionListItemButton;

} // namespace elizaos
