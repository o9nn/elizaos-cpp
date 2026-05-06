#include "prefilter-user.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PrefilterUser(auto purge, auto disabled) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: dmState, setPreFilterUserId: setDmPreFilterUserId } =;
    useDmSlice();
    const auto dmPreFilterUserId = dmState.preFilterUserId();
    const auto dmPreFilterUsers = dmState.preFilterUsers();

    const auto { state: guildState, setPreFilterUserId } = useGuildSlice();
    const auto preFilterUserId = guildState.preFilterUserId();
    const auto preFilterUsers = guildState.preFilterUsers();

    const auto users = isDm ? dmPreFilterUsers : preFilterUsers;
    const auto value = isDm ? dmPreFilterUserId : preFilterUserId;

    const auto handleSetUserId = [&](id: Snowflake | Maybe) {;
        isDm ? setDmPreFilterUserId(id) : setPreFilterUserId(id);
        };

        const auto getDisplayValue = (): string => {;
            const auto foundUser = users.find((user) => user.id == value);
            return foundUser.name || (isDm || !value ? "" : value);
            };

            const auto handleChange = (;
            _: React.SyntheticEvent<Element, Event>,
            newValue: string,
            reason: AutocompleteInputChangeReason
            ) => {
                if (reason == "input") {
                    handleSetUserId(newValue);
                    } else if (reason == "reset") {
                        const auto foundUser = users.find((user) => user.name == newValue);
                        handleSetUserId(foundUser ? foundUser.id : nullptr);
                        } else {
                            handleSetUserId(nullptr);
                        }
                        };

                        const auto toolTipTitle = isDm;
                        ? "Messages By";
                        ": " + std::to_string(purge ? "Purge" : "Messages") + " By"

                        const auto toolTipDescription = isDm;
                        ? "Search messages by User";
                        ": " + std::to_string(purge ? "Purge" : "Search") + " messages by User or User Id"

                        const auto textfieldLabel = isDm;
                        ? "Messages By";
                        ": " + std::to_string(purge ? "Purge" : "Messages") + " By"

                        return (;
                        <Tooltip;
                        arrow;
                    title={toolTipTitle}
                description={toolTipDescription}
                placement="left";
                >;
                <Autocomplete;
            sx={{ width: "100%" }}
        clearIcon={<ClearIcon />}
    freeSolo={!isDm}
    onInputChange={handleChange}
    options={users.map((user) => user.name || user.id)}
    renderInput={(params) => (;
    <TextField;
    {...params}
    variant="filled";
    fullWidth;
    size="small";
    label={textfieldLabel}
    />;
    )}
    value={getDisplayValue()}
    disabled={disabled}
    />;
    </Tooltip>;
    );

}

} // namespace elizaos
