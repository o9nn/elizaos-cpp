#include ".common-components/date-time-picker/date-time-picker.hpp"
#include ".common-components/tooltip/tooltip.hpp"
#include ".features/message/use-message-slice.hpp"
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

using FieldProps = {
  toolTipTitle: string;
  toolTipDescription: string;
  label: string;
};

using BeforeAndAfterFieldsProps = {
  disabled: boolean;
  afterProps?: FieldProps;
  beforeProps?: FieldProps;
};

,
  beforeProps = {
    toolTipTitle: "Messages Before",
    toolTipDescription: "Search messages from before the provided date",
    label: "Messages Before",
  },
}: BeforeAndAfterFieldsProps) {
  const {
    state: messageState,
    setSearchAfterDate,
    setSearchBeforeDate,
  } = useMessageSlice();
  const searchAfterDate = messageState.searchAfterDate();
  const searchBeforeDate = messageState.searchBeforeDate();

  return (
    <Stack
      direction="row"
      justifyContent="center"
      alignItems="center"
      spacing={1}
    >
      <Tooltip
        arrow
        title={afterProps.toolTipTitle}
        description={afterProps.toolTipDescription}
        placement="left"
      >
        <DateTimePicker
          onDateChange={(e) => setSearchAfterDate(e)}
          label={afterProps.label}
          disabled={disabled}
          value={searchAfterDate}
        />
      </Tooltip>
      <Tooltip
        arrow
        title={beforeProps.toolTipTitle}
        description={beforeProps.toolTipDescription}
        placement="right"
      >
        <DateTimePicker
          onDateChange={(e) => setSearchBeforeDate(e)}
          label={beforeProps.label}
          disabled={disabled}
          value={searchBeforeDate}
        />
      </Tooltip>
    </Stack>
  );
}

default BeforeAndAfterFields;

} // namespace elizaos
