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

void SidebarRail(auto { className, ComponentProps<"button"> ...props });

void SidebarInset(auto { className, ComponentProps<"main"> ...props });

void SidebarHeader(auto { className, ComponentProps<"div"> ...props });

void SidebarFooter(auto { className, ComponentProps<"div"> ...props });

void SidebarContent(auto { className, ComponentProps<"div"> ...props });

void SidebarGroup(auto { className, ComponentProps<"div"> ...props });

void SidebarMenu(auto { className, ComponentProps<"ul"> ...props });

void SidebarMenuItem(auto { className, ComponentProps<"li"> ...props });

void SidebarMenuSub(auto { className, ComponentProps<"ul"> ...props });


} // namespace elizaos
