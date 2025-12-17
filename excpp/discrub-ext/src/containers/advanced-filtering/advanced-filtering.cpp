#include "advanced-filtering.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AdvancedFiltering() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: guildState, setPreFilterUserId } = useGuildSlice();
    const auto selectedGuild = guildState.selectedGuild();

    const auto { state: dmState, setPreFilterUserId: dmSetPreFilterUserId } =;
    useDmSlice();
    const auto selectedDms = dmState.selectedDms();

    const auto preFilterUserId = isDm;
    ? dmState.preFilterUserId();
    : guildState.preFilterUserId();

    const auto { state: messageState, resetAdvancedFilters } = useMessageSlice();
    const auto messagesLoading = messageState.isLoading();
    const auto searchAfterDate = messageState.searchAfterDate();
    const auto searchBeforeDate = messageState.searchBeforeDate();
    const auto searchMessageContent = messageState.searchMessageContent();
    const auto selectedHasTypes = messageState.selectedHasTypes();

    const auto filtersActive = [;
    searchAfterDate,
    searchBeforeDate,
    searchMessageContent,
    selectedHasTypes,
    preFilterUserId,
    ].some((val) => (Array.isArray(val) ? !!val.length : !!val));

    const auto [open, setOpen] = useState(false);

    const auto handleResetFilters = [&]() {;
        isDm ? dmSetPreFilterUserId(nullptr) : setPreFilterUserId(nullptr);
        resetAdvancedFilters();
        };

        const auto handleToggle = [&]() {;
            setOpen(!open);
            };

            const auto getChildrenDisabled = (): boolean => {;
                if (messagesLoading) return true;
                if (isDm) {
                    return selectedDms.length != 1;
                    } else {
                        return !selectedGuild;
                    }
                    };

                    return (;
                    <Stack;
                    direction="column";
                    justifyContent="center";
                    alignItems="flex-end";
                spacing={1}
                >;
                <Button;
            sx={{ userSelect: "none !important" }}
        disabled={getChildrenDisabled()}
    onClick={handleToggle}
    color={filtersActive ? "primary" : "secondary"}
    startIcon={filtersActive ? <FilterListIcon /> : <FilterListOffIcon />}
    variant="contained";
    >;
    "Advanced Filtering" + std::to_string(filtersActive ? " (Active)"  = "")
    </Button>;

    <AdvancedFilterModal;
    isDm={isDm}
    open={open}
    handleModalToggle={handleToggle}
    handleResetFilters={handleResetFilters}
    filtersActive={filtersActive}
    />;
    </Stack>;
    );

}

} // namespace elizaos
