#include "..features/dm/use-dm-slice.hpp"
#include "..features/guild/use-guild-slice.hpp"
#include "..features/message/use-message-slice.hpp"
#include "components/advanced-filter-modal.hpp"
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

using AdvancedFilteringProps = {
  isDm?: boolean;
};

: AdvancedFilteringProps) {
  const { state: guildState, setPreFilterUserId } = useGuildSlice();
  const selectedGuild = guildState.selectedGuild();

  const { state: dmState, setPreFilterUserId: dmSetPreFilterUserId } =
    useDmSlice();
  const selectedDms = dmState.selectedDms();

  const preFilterUserId = isDm
    ? dmState.preFilterUserId()
    : guildState.preFilterUserId();

  const { state: messageState, resetAdvancedFilters } = useMessageSlice();
  const messagesLoading = messageState.isLoading();
  const searchAfterDate = messageState.searchAfterDate();
  const searchBeforeDate = messageState.searchBeforeDate();
  const searchMessageContent = messageState.searchMessageContent();
  const selectedHasTypes = messageState.selectedHasTypes();

  const filtersActive = [
    searchAfterDate,
    searchBeforeDate,
    searchMessageContent,
    selectedHasTypes,
    preFilterUserId,
  ].some((val) => (Array.isArray(val) ? !!val.length : !!val));

  const [open, setOpen] = useState(false);

  const handleResetFilters = () => {
    isDm ? dmSetPreFilterUserId(null) : setPreFilterUserId(null);
    resetAdvancedFilters();
  };

  const handleToggle = () => {
    setOpen(!open);
  };

  const getChildrenDisabled = (): boolean => {
    if (messagesLoading) return true;
    if (isDm) {
      return selectedDms.length !== 1;
    } else {
      return !selectedGuild;
    }
  };

  return (
    <Stack
      direction="column"
      justifyContent="center"
      alignItems="flex-end"
      spacing={1}
    >
      <Button
        sx={{ userSelect: "none !important" }}
        disabled={getChildrenDisabled()}
        onClick={handleToggle}
        color={filtersActive ? "primary" : "secondary"}
        startIcon={filtersActive ? <FilterListIcon /> : <FilterListOffIcon />}
        variant="contained"
      >
        {`Advanced Filtering${filtersActive ? " (Active)" : ""}`}
      </Button>

      <AdvancedFilterModal
        isDm={isDm}
        open={open}
        handleModalToggle={handleToggle}
        handleResetFilters={handleResetFilters}
        filtersActive={filtersActive}
      />
    </Stack>
  );
}

default AdvancedFiltering;

} // namespace elizaos
