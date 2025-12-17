#include "mui-img.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MuiImg(auto {
  sx, auto props, {
  sx: SxProps;
  props: React.DetailedHTMLProps<
    React.ImgHTMLAttributes<HTMLImageElement> }, auto HTMLImageElement
  >;
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <StyledImg {...props} sx={sx} />;

}

} // namespace elizaos
