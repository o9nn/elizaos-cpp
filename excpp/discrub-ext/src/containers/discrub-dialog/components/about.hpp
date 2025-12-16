#include "...changelog.json.hpp"
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

}
    >
      <Stack sx={{ maxHeight: "600px", overflow: "auto" }}>
        <Stack sx={{ padding: 3, spacing: 2 }}>
          <Accordion expanded>
            <AccordionSummary sx={{ cursor: "default !important" }}>
              <Stack
                direction="row"
                justifyContent="flex-start"
                alignItems="center"
                spacing={1}
              >
                <ChangeCircleIcon />
                <Typography sx={{ userSelect: "none !important" }}>
                  Change-log
                </Typography>
              </Stack>
            </AccordionSummary>
            <AccordionDetails>
              <Stack spacing={3}>
                {changelog.map((change) => {
                  return (
                    <Stack>
                      <Typography variant="body2">
                        Version {change.version}
                      </Typography>
                      <ul>
                        {change.changes.map((innerChange) => {
                          return (
                            <Typography sx={{ opacity: 0.5 }} variant="caption">
                              <li>{innerChange}</li>
                            </Typography>
                          );
                        })}
                      </ul>
                    </Stack>
                  );
                })}
              </Stack>
            </AccordionDetails>
          </Accordion>
        </Stack>
      </Stack>
    </Stack>
  );
}

default About;

} // namespace elizaos
