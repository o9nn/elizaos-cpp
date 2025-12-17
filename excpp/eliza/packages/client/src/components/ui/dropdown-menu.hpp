#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use




void DropdownMenu();

void DropdownMenuPortal();

void DropdownMenuTrigger();

void DropdownMenuContent(auto sideOffset, auto onPointerDown, auto onPointerDownOutside, auto onCloseAutoFocus);

void DropdownMenuGroup();

void DropdownMenuItem(auto inset, auto variant);

void DropdownMenuCheckboxItem(auto children, auto checked);

void DropdownMenuRadioGroup();

void DropdownMenuRadioItem(auto children);

void DropdownMenuLabel(auto inset);

void DropdownMenuSeparator();

void DropdownMenuShortcut();

void DropdownMenuSub();

void DropdownMenuSubTrigger(auto inset, auto children);

void DropdownMenuSubContent();


} // namespace elizaos
