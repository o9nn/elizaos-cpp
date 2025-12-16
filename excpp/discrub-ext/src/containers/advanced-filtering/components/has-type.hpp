#include "...common-components/multi-value-select/multi-value-select.hpp"
#include "...common-components/tooltip/tooltip.hpp"
#include "...enum/has-type.hpp"
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

using HasTypeProps = {
  disabled: boolean;
};

: HasTypeProps) {
  const { state: messageState, setSelectedHasTypes } = useMessageSlice();
  const selectedHasTypes = messageState.selectedHasTypes();

  return (
    <Tooltip
      arrow
      title="Messages Containing"
      description="Search messages that contain the following type(s)"
      placement="left"
    >
      <MultiValueSelect
        disabled={disabled}
        label="Messages Containing"
        onChange={(values) => setSelectedHasTypes(values as HasTypeEnum[])}
        value={selectedHasTypes}
        values={Object.values(HasTypeEnum)}
      />
    </Tooltip>
  );
}

default HasType;

} // namespace elizaos
