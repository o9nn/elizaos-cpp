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

const Tooltip = (
  props: {
    description?: string;
  } & TooltipProps
) => {
  const theme = useTheme();
  const { title, description, children, ...rest } = props;
  return (
    <MuiTooltip
      disableInteractive
      PopperProps={{ sx: { maxWidth: "200px" } }}
      TransitionProps={{
        timeout: {
          appear: 1,
          enter: 1,
          exit: 500,
        },
      }}
      {...rest}
      title={
        <Box
          sx={{
            display: "flex",
            flexDirection: "column",
            color: theme.palette.text.secondary,
          }}
        >
          <strong>{title}</strong>
          {description && (
            <Typography
              sx={{
                color: theme.palette.text.secondary,
                lineHeight: 1.1,
                fontWeight: 550,
              }}
              mt={0.5}
              variant="caption"
            >
              {description}
            </Typography>
          )}
        </Box>
      }
    >
      {children}
    </MuiTooltip>
  );
};

default Tooltip;

} // namespace elizaos
