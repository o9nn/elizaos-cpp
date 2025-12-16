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

const StyledImg = styled("img", { shouldForwardProp: (prop) => prop !== "sx" })(
  {}
);

default : {
  sx: SxProps;
  props: React.DetailedHTMLProps<
    React.ImgHTMLAttributes<HTMLImageElement>,
    HTMLImageElement
  >;
}) {
  return <StyledImg {...props} sx={sx} />;
}

} // namespace elizaos
