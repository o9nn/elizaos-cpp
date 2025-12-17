#include "has-type.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void HasType() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: messageState, setSelectedHasTypes } = useMessageSlice();
    const auto selectedHasTypes = messageState.selectedHasTypes();

    return (;
    <Tooltip;
    arrow;
    title="Messages Containing";
    description="Search messages that contain the following type(s)";
    placement="left";
    >;
    <MultiValueSelect;
    disabled={disabled}
    label="Messages Containing";
    onChange={(values) => setSelectedHasTypes(values[])}
    value={selectedHasTypes}
    values={Object.values(HasTypeEnum)}
    />;
    </Tooltip>;
    );

}

} // namespace elizaos
