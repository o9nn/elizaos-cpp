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

const ModalDebugMessage = ({
  debugMessage,
}: {
  debugMessage: string | Maybe;
}) => {
  const theme = useTheme();
  return (
    <Box
      my={1}
      sx={{
        alignItems: "center",
        justifyContent: "center",
        display: "flex",
        opacity: debugMessage?.length ? 1 : 0,
      }}
    >
      <Typography variant="caption" sx={{ color: theme.palette.warning.main }}>
        {debugMessage || "An error occurred"}
      </Typography>
    </Box>
  );
};
default ModalDebugMessage;

} // namespace elizaos
