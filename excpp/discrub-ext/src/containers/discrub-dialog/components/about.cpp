#include "about.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void About() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Stack;
    sx={{ pt: "5px", maxHeight: "85%", maxWidth: "100%", overflow: "hidden" }}
    >;
    <Stack sx={{ maxHeight: "600px", overflow: "auto" }}>
    <Stack sx={{ padding: 3, spacing: 2 }}>
    <Accordion expanded>;
    <AccordionSummary sx={{ cursor: "default !important" }}>
    <Stack;
    direction="row";
    justifyContent="flex-start";
    alignItems="center";
    spacing={1}
    >;
    <ChangeCircleIcon />;
    <Typography sx={{ userSelect: "none !important" }}>
    Change-log;
    </Typography>;
    </Stack>;
    </AccordionSummary>;
    <AccordionDetails>;
    <Stack spacing={3}>;
    {changelog.map((change) => {
        return (;
        <Stack>;
        <Typography variant="body2">;
    Version {change.version}
    </Typography>;
    <ul>;
    {change.changes.map((innerChange) => {
        return (;
        <Typography sx={{ opacity: 0.5 }} variant="caption">
        <li>{innerChange}</li>;
        </Typography>;
        );
    })}
    </ul>;
    </Stack>;
    );
    })}
    </Stack>;
    </AccordionDetails>;
    </Accordion>;
    </Stack>;
    </Stack>;
    </Stack>;
    );

}

} // namespace elizaos
