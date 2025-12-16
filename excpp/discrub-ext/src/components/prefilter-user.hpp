#include ".common-components/tooltip/tooltip.hpp"
#include ".features/dm/use-dm-slice.hpp"
#include ".features/guild/use-guild-slice.hpp"
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

using PrefilterUserProps = {
  isDm?: boolean;
  purge?: boolean;
  disabled?: boolean;
};

: PrefilterUserProps) {
  const { state: dmState, setPreFilterUserId: setDmPreFilterUserId } =
    useDmSlice();
  const dmPreFilterUserId = dmState.preFilterUserId();
  const dmPreFilterUsers = dmState.preFilterUsers();

  const { state: guildState, setPreFilterUserId } = useGuildSlice();
  const preFilterUserId = guildState.preFilterUserId();
  const preFilterUsers = guildState.preFilterUsers();

  const users = isDm ? dmPreFilterUsers : preFilterUsers;
  const value = isDm ? dmPreFilterUserId : preFilterUserId;

  const handleSetUserId = (id: Snowflake | Maybe) => {
    isDm ? setDmPreFilterUserId(id) : setPreFilterUserId(id);
  };

  const getDisplayValue = (): string => {
    const foundUser = users.find((user) => user.id === value);
    return foundUser?.name || (isDm || !value ? "" : value);
  };

  const handleChange = (
    _: React.SyntheticEvent<Element, Event>,
    newValue: string,
    reason: AutocompleteInputChangeReason
  ) => {
    if (reason === "input") {
      handleSetUserId(newValue);
    } else if (reason === "reset") {
      const foundUser = users.find((user) => user.name === newValue);
      handleSetUserId(foundUser ? foundUser.id : null);
    } else {
      handleSetUserId(null);
    }
  };

  const toolTipTitle = isDm
    ? "Messages By"
    : `${purge ? "Purge" : "Messages"} By`;

  const toolTipDescription = isDm
    ? "Search messages by User"
    : `${purge ? "Purge" : "Search"} messages by User or User Id`;

  const textfieldLabel = isDm
    ? "Messages By"
    : `${purge ? "Purge" : "Messages"} By`;

  return (
    <Tooltip
      arrow
      title={toolTipTitle}
      description={toolTipDescription}
      placement="left"
    >
      <Autocomplete
        sx={{ width: "100%" }}
        clearIcon={<ClearIcon />}
        freeSolo={!isDm}
        onInputChange={handleChange}
        options={users?.map((user) => user.name || user.id)}
        renderInput={(params) => (
          <TextField
            {...params}
            variant="filled"
            fullWidth
            size="small"
            label={textfieldLabel}
          />
        )}
        value={getDisplayValue()}
        disabled={disabled}
      />
    </Tooltip>
  );
}

default PrefilterUser;

} // namespace elizaos
