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

using DateProps = { longDateTime: string; shortDateTime: string };

const Date = ({ longDateTime, shortDateTime }: DateProps) => {
  const theme = useTheme();

  return (
    <Typography
      title={longDateTime}
      sx={{ color: theme.palette.text.disabled, lineHeight: 0 }}
      variant="caption"
    >
      {shortDateTime}
    </Typography>
  );
};
default Date;

} // namespace elizaos
