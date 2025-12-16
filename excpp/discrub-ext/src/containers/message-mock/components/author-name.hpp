#include "...classes/guild.hpp"
#include "...classes/message.hpp"
#include "...classes/role.hpp"
#include "...features/export/export-types.hpp"
#include "...utils.hpp"
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

using AuthorNameProps = {
  msg: Message;
  userMap: ExportUserMap;
  selectedGuild: Guild | Maybe;
  getRolePath: (x: Snowflake, y: string | Maybe) => ResolvedFilePathObject;
};

const AuthorName = ({
  msg,
  userMap,
  selectedGuild,
  getRolePath,
}: AuthorNameProps) => {
  const author = msg.author;

  const {
    roles: guildRoles,
    nick: guildNickname,
    joinedAt,
  } = userMap[author.id]?.guilds[String(selectedGuild?.id)] || {};

  let roleNames: string[] = [];
  let colorRole: Role | Maybe = null;
  let iconRole: Role | Maybe = null;

  if (selectedGuild) {
    const highestRole = getHighestRoles(guildRoles, selectedGuild);
    if (highestRole) {
      colorRole = highestRole.colorRole;
      iconRole = highestRole.iconRole;
    }
    roleNames = getRoleNames(guildRoles, selectedGuild);
  }

  const { local, remote }: ResolvedFilePathObject = iconRole
    ? getRolePath(iconRole.id, iconRole.icon)
    : { local: undefined, remote: undefined };

  return (
    <>
      <strong
        title={formatUserData({
          userId: author.id,
          userName: author.username,
          displayName: author.global_name,
          guildNickname,
          joinedAt,
          roleNames,
        })}
        style={{ color: colorRole ? getColor(colorRole.color) : undefined }}
      >
        {guildNickname || author.global_name || author.username}
      </strong>
      {iconRole && (
        <img
          title={iconRole.name}
          style={{ width: "20px", height: "20px" }}
          src={local || remote}
          alt="role-icon"
        />
      )}
      {!iconRole && msg.author.bot && (
        <span
          title="Verified Bot"
          style={{
            color: "#FFF !important",
            backgroundColor: "#5865f2",
            borderRadius: "5px",
            display: "inline-flex",
            fontSize: "10px",
            alignItems: "center",
            justifyContent: "center",
            height: "16px",
            width: "38px",
            gap: "2px",
            wordBreak: "keep-all",
          }}
        >
          <CheckIcon sx={{ width: "12px !important" }} /> APP
        </span>
      )}
    </>
  );
};
default AuthorName;

} // namespace elizaos
