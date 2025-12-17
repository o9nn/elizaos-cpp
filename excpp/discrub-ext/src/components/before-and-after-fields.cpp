#include "before-and-after-fields.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void BeforeAndAfterFields(auto {
  disabled, "Messages After" afterProps = {
    toolTipTitle, "Search messages from after the provided date" toolTipDescription, "Messages After" label, auto }, "Messages Before" beforeProps = {
    toolTipTitle, "Search messages from before the provided date" toolTipDescription, "Messages Before" label, auto }, BeforeAndAfterFieldsProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        state: messageState,
        setSearchAfterDate,
        setSearchBeforeDate,
        } = useMessageSlice();
        const auto searchAfterDate = messageState.searchAfterDate();
        const auto searchBeforeDate = messageState.searchBeforeDate();

        return (;
        <Stack;
        direction="row";
        justifyContent="center";
        alignItems="center";
    spacing={1}
    >;
    <Tooltip;
    arrow;
    title={afterProps.toolTipTitle}
    description={afterProps.toolTipDescription}
    placement="left";
    >;
    <DateTimePicker;
    onDateChange={(e) => setSearchAfterDate(e)}
    label={afterProps.label}
    disabled={disabled}
    value={searchAfterDate}
    />;
    </Tooltip>;
    <Tooltip;
    arrow;
    title={beforeProps.toolTipTitle}
    description={beforeProps.toolTipDescription}
    placement="right";
    >;
    <DateTimePicker;
    onDateChange={(e) => setSearchBeforeDate(e)}
    label={beforeProps.label}
    disabled={disabled}
    value={searchBeforeDate}
    />;
    </Tooltip>;
    </Stack>;
    );

}

} // namespace elizaos
