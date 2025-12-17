#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".icons/layout.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using SidebarContextProps = std::variant<{

void useSidebar();

  // This is the internal state of the sidebar.
  // We use openProp and setOpenProp for control from outside the component.

      // This sets the cookie to keep the sidebar state.

  // Helper to toggle the sidebar.

  // Adds a keyboard shortcut to toggle the sidebar.

  // We add a state so that we can do data-state="expanded" or "collapsed".
  // This makes it easier to style the sidebar with Tailwind classes.

void Sidebar(auto variant, auto collapsible, auto className, auto children);

void SidebarTrigger();

void SidebarRail();

void SidebarInset();

void SidebarInput();

void SidebarHeader();

void SidebarFooter();

void SidebarSeparator();

void SidebarContent();

void SidebarGroup();

void SidebarGroupLabel(auto asChild);

void SidebarGroupAction(auto asChild);

void SidebarGroupContent();

void SidebarMenu();

void SidebarMenuItem();

void SidebarMenuButton(auto isActive, auto variant, auto size, auto tooltip, auto className);

void SidebarMenuAction(auto asChild, auto showOnHover);

void SidebarMenuBadge();

void SidebarMenuSkeleton(auto showIcon);

void SidebarMenuSub();

void SidebarMenuSubItem();

void SidebarMenuSubButton(auto size, auto isActive, auto className);


} // namespace elizaos
