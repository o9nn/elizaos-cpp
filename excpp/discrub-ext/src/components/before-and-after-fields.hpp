#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".common-components/date-time-picker/date-time-picker.hpp"
#include ".common-components/tooltip/tooltip.hpp"
#include ".features/message/use-message-slice.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using FieldProps = {

using BeforeAndAfterFieldsProps = {

void BeforeAndAfterFields(auto afterProps, "Search messages from after the provided date" toolTipDescription, "Messages After" label, auto beforeProps, "Search messages from before the provided date" toolTipDescription, "Messages Before" label);


} // namespace elizaos
