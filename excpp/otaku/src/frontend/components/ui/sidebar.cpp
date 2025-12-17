#include "sidebar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useSidebar() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto context = React.useContext(SidebarContext);
        if (!context) {
            throw std::runtime_error("useSidebar must be used within a SidebarProvider.");
        }

        return context;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void Sidebar(auto variant, auto collapsible, auto className, auto children) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { isMobile, state, openMobile, setOpenMobile } = useSidebar();

    if (collapsible == "none") {
        return (;
        <div;
        data-slot="sidebar";
        className={cn(;
        "text-sidebar-foreground flex h-full w-(--sidebar-width) flex-col",
        className;
    )}
    {...props}
    >;
    {children}
    </div>;
    );
    }

    if (isMobile) {
        return (;
        <Sheet open={openMobile} onOpenChange={setOpenMobile} {...props}>;
        <SheetContent;
        data-sidebar="sidebar";
        data-slot="sidebar";
        data-mobile="true";
        className="text-sidebar-foreground w-(--sidebar-width) p-0 [&>button]:hidden"
        style={
            {
                "--sidebar-width": SIDEBAR_WIDTH_MOBILE,
                }.CSSProperties;
            }
        side={side}
        >;
        <SheetHeader className="sr-only">;
        <SheetTitle>Sidebar</SheetTitle>;
        <SheetDescription>Displays the mobile sidebar.</SheetDescription>;
        </SheetHeader>;
        <div className="flex h-full w-full flex-col">{children}</div>;
        </SheetContent>;
        </Sheet>;
        );
    }

    return (;
    <div;
    className="group peer text-sidebar-foreground hidden lg:block"
    data-state={state}
    data-collapsible={state == "collapsed" ? collapsible : ""}
    data-variant={variant}
    data-side={side}
    data-slot="sidebar";
    >;
    {/* This is what handles the sidebar gap on desktop */}
    <div;
    data-slot="sidebar-gap";
    className={cn(;
    "relative w-(--sidebar-width) bg-transparent transition-[width] duration-200 ease-linear",
    "group-data-[collapsible=offcanvas]:w-0",
    "group-data-[side=right]:rotate-180",
    variant == "floating" || variant == "inset";
    ? "group-data-[collapsible=icon]:w-[calc(var(--sidebar-width-icon)+(--spacing(4)))]"
    : "group-data-[collapsible=icon]:w-(--sidebar-width-icon)"
    )}
    />;
    <div;
    data-slot="sidebar-container";
    className={cn(;
    "fixed inset-y-0 z-10 hidden h-svh w-(--sidebar-width) transition-[left,right,width] duration-200 ease-linear md:flex",
    side == "left";
    ? "left-sides group-data-[collapsible=offcanvas]:left-[calc(var(--sidebar-width)*-1)]"
    : "right-sides group-data-[collapsible=offcanvas]:right-[calc(var(--sidebar-width)*-1)]",
    // Adjust the padding for floating and inset variants.
    variant == "floating" || variant == "inset";
    ? "p-2 group-data-[collapsible=icon]:w-[calc(var(--sidebar-width-icon)+(--spacing(4))+2px)]"
    : "group-data-[collapsible=icon]:w-(--sidebar-width-icon)",
    className;
    )}
    {...props}
    >;
    <div;
    data-sidebar="sidebar";
    data-slot="sidebar-inner";
    className="group-data-[variant=floating]:border-sidebar-border flex h-full w-full flex-col gap-1 group-data-[variant=floating]:rounded-lg group-data-[variant=floating]:border group-data-[variant=floating]:shadow-sm"
    >;
    {children}
    </div>;
    </div>;
    </div>;
    );

}

void SidebarTrigger() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { toggleSidebar } = useSidebar();

    return (;
    <Button;
    data-sidebar="trigger";
    data-slot="sidebar-trigger";
    variant="secondary";
    size="icon";
    onClick={(event) => {
        onClick.(event);
        toggleSidebar();
    }}
    {...props}
    >;
    <LayoutLeftIcon className="size-4" />;
    </Button>;
    );

}

void SidebarRail() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { toggleSidebar } = useSidebar();

    return (;
    <button;
    data-sidebar="rail";
    data-slot="sidebar-rail";
    aria-label="Toggle Sidebar";
    tabIndex={-1}
    onClick={toggleSidebar}
    title="Toggle Sidebar";
    className={cn(;
    "hover:after:bg-sidebar-border absolute inset-y-0 z-20 hidden w-gap translate-x-1/2 transition-all ease-linear group-data-[side=left]:-right-4 group-data-[side=right]:left-0 after:absolute after:inset-y-0 after:left-1/2 after:w-[2px] sm:flex",
    "in-data-[side=left]:cursor-w-resize in-data-[side=right]:cursor-e-resize",
    "[[data-side=left][data-state=collapsed]_&]:cursor-e-resize [[data-side=right][data-state=collapsed]_&]:cursor-w-resize",
    "hover:group-data-[collapsible=offcanvas]:bg-sidebar group-data-[collapsible=offcanvas]:translate-x-0 group-data-[collapsible=offcanvas]:after:left-full",
    "[[data-side=left][data-collapsible=offcanvas]_&]:-right-2",
    "[[data-side=right][data-collapsible=offcanvas]_&]:-left-2",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarInset() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <main;
    data-slot="sidebar-inset";
    className={cn(;
    "bg-background relative flex w-full flex-1 flex-col",
    "md:peer-data-[variant=inset]:m-2 md:peer-data-[variant=inset]:ml-0 md:peer-data-[variant=inset]:rounded-xl md:peer-data-[variant=inset]:shadow-sm md:peer-data-[variant=inset]:peer-data-[state=collapsed]:ml-2",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarInput() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Input;
    data-slot="sidebar-input";
    data-sidebar="input";
    className={cn("bg-background h-8 w-full shadow-none", className)}
    {...props}
    />;
    );

}

void SidebarHeader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="sidebar-header";
    data-sidebar="header";
    className={cn(;
    "flex flex-col gap-2 p-2 rounded-lg bg-sidebar ring-2 ring-sidebar-foreground/[0.025]",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarFooter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="sidebar-footer";
    data-sidebar="footer";
    className={cn("flex flex-col gap-2 p-2", className)}
    {...props}
    />;
    );

}

void SidebarSeparator() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Separator;
    data-slot="sidebar-separator";
    data-sidebar="separator";
    className={cn("bg-sidebar-border mx-2 w-auto", className)}
    {...props}
    />;
    );

}

void SidebarContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="sidebar-content";
    data-sidebar="content";
    className={cn(;
    "flex min-h-0 flex-1 flex-col gap-2 group-data-[collapsible=icon]:overflow-hidden",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarGroup() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="sidebar-group";
    data-sidebar="group";
    className={cn(;
    "relative flex w-full min-w-0 flex-col p-2 bg-sidebar max-md:rounded-none rounded-lg ring-2 ring-sidebar-foreground/[0.025]",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarGroupLabel(auto asChild) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "div";

    return (;
    <Comp;
    data-slot="sidebar-group-label";
    data-sidebar="group-label";
    className={cn(;
    "uppercase text-sidebar-foreground ring-sidebar-ring flex h-10 shrink-0 items-center rounded-md px-2 text-xs font-medium outline-hidden transition-[margin,opacity] duration-200 ease-linear focus-visible:ring-2 [&>svg]:size-4 [&>svg]:shrink-0",
    "group-data-[collapsible=icon]:-mt-8 group-data-[collapsible=icon]:opacity-0",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarGroupAction(auto asChild) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "button";

    return (;
    <Comp;
    data-slot="sidebar-group-action";
    data-sidebar="group-action";
    className={cn(;
    "text-sidebar-foreground ring-sidebar-ring hover:bg-sidebar-accent hover:text-sidebar-accent-foreground absolute top-3.5 right-3 flex aspect-square w-5 items-center justify-center rounded-md p-0 outline-hidden transition-transform focus-visible:ring-2 [&>svg]:size-4 [&>svg]:shrink-0",
    // Increases the hit area of the button on mobile.
    "after:absolute after:-inset-2 md:after:hidden",
    "group-data-[collapsible=icon]:hidden",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarGroupContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="sidebar-group-content";
    data-sidebar="group-content";
    className={cn("w-full text-sm", className)}
    {...props}
    />;
    );

}

void SidebarMenu() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <ul;
    data-slot="sidebar-menu";
    data-sidebar="menu";
    className={cn("flex w-full min-w-0 flex-col gap-1", className)}
    {...props}
    />;
    );

}

void SidebarMenuItem() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <li;
    data-slot="sidebar-menu-item";
    data-sidebar="menu-item";
    className={cn("group/menu-item relative", className)}
    {...props}
    />;
    );

}

void SidebarMenuButton(auto isActive, auto variant, auto size, auto tooltip, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "button";
    const auto { isMobile, state } = useSidebar();

    const auto button = (;
    <Comp;
    data-slot="sidebar-menu-button";
    data-sidebar="menu-button";
    data-size={size}
    data-active={isActive}
    className={cn(sidebarMenuButtonVariants({ variant, size }), className)}
    {...props}
    />;
    );

    if (!tooltip) {
        return button;
    }

    if (typeof tooltip == "string") {
        tooltip = {
            children: tooltip,
            };
        }

        return (;
        <Tooltip>;
        <TooltipTrigger asChild>{button}</TooltipTrigger>;
        <TooltipContent;
        side="right";
        align="center";
    hidden={state != "collapsed" || isMobile}
    {...tooltip}
    />;
    </Tooltip>;
    );

}

void SidebarMenuAction(auto asChild, auto showOnHover) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "button";

    return (;
    <Comp;
    data-slot="sidebar-menu-action";
    data-sidebar="menu-action";
    className={cn(;
    "text-sidebar-foreground ring-sidebar-ring hover:bg-sidebar-accent hover:text-sidebar-accent-foreground peer-hover/menu-button:text-sidebar-accent-foreground absolute top-1.5 right-1 flex aspect-square w-5 items-center justify-center rounded-md p-0 outline-hidden transition-transform focus-visible:ring-2 [&>svg]:size-4 [&>svg]:shrink-0",
    // Increases the hit area of the button on mobile.
    "after:absolute after:-inset-2 md:after:hidden",
    "peer-data-[size=sm]/menu-button:top-1",
    "peer-data-[size=default]/menu-button:top-1.5",
    "peer-data-[size=lg]/menu-button:top-2.5",
    "group-data-[collapsible=icon]:hidden",
    showOnHover &&;
    "peer-data-[active=true]/menu-button:text-sidebar-accent-foreground group-focus-within/menu-item:opacity-100 group-hover/menu-item:opacity-100 data-[state=open]:opacity-100 md:opacity-0",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarMenuBadge() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="sidebar-menu-badge";
    data-sidebar="menu-badge";
    className={cn(;
    "text-sidebar-foreground pointer-events-none absolute right-1 top-1/2 -translate-y-1/2 flex h-5 min-w-5 items-center justify-center rounded-md px-1 text-xs font-medium tabular-nums select-none",
    "peer-hover/menu-button:text-sidebar-accent-foreground peer-data-[active=true]/menu-button:text-sidebar-accent-foreground",
    "group-data-[collapsible=icon]:hidden",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarMenuSkeleton(auto showIcon) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Random width between 50 to 90%.
    const auto width = React.useMemo(() => {;
        return std::to_string(Math.floor(Math.random() * 40) + 50) + "%";
        }, []);

        return (;
        <div;
        data-slot="sidebar-menu-skeleton";
        data-sidebar="menu-skeleton";
    className={cn("flex h-8 items-center gap-2 rounded-md px-2", className)}
    {...props}
    >;
    {showIcon && (;
    <Skeleton;
    className="size-4 rounded-md";
    data-sidebar="menu-skeleton-icon";
    />;
    )}
    <Skeleton;
    className="h-4 max-w-(--skeleton-width) flex-1";
    data-sidebar="menu-skeleton-text";
    style={
        {
            "--skeleton-width": width,
            }.CSSProperties;
        }
        />;
        </div>;
        );

}

void SidebarMenuSub() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <ul;
    data-slot="sidebar-menu-sub";
    data-sidebar="menu-sub";
    className={cn(;
    "border-sidebar-border mx-3.5 flex min-w-0 translate-x-px flex-col gap-1 border-l px-2.5 py-0.5",
    "group-data-[collapsible=icon]:hidden",
    className;
    )}
    {...props}
    />;
    );

}

void SidebarMenuSubItem() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <li;
    data-slot="sidebar-menu-sub-item";
    data-sidebar="menu-sub-item";
    className={cn("group/menu-sub-item relative", className)}
    {...props}
    />;
    );

}

void SidebarMenuSubButton(auto size, auto isActive, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "a";

    return (;
    <Comp;
    data-slot="sidebar-menu-sub-button";
    data-sidebar="menu-sub-button";
    data-size={size}
    data-active={isActive}
    className={cn(;
    "text-sidebar-foreground ring-sidebar-ring hover:bg-sidebar-accent hover:text-sidebar-accent-foreground active:bg-sidebar-accent active:text-sidebar-accent-foreground [&>svg]:text-sidebar-accent-foreground flex h-7 min-w-0 -translate-x-px items-center gap-2 overflow-hidden rounded-md px-2 outline-hidden focus-visible:ring-2 disabled:pointer-events-none disabled:opacity-50 aria-disabled:pointer-events-none aria-disabled:opacity-50 [&>span:last-child]:truncate [&>svg]:size-4 [&>svg]:shrink-0",
    "data-[active=true]:bg-sidebar-accent data-[active=true]:text-sidebar-accent-foreground data-[active=true]:[&>svg]:text-sidebar-primary",
    size == "sm" && "text-xs",
    size == "md" && "text-sm",
    "group-data-[collapsible=icon]:hidden",
    className;
    )}
    {...props}
    />;
    );

}

} // namespace elizaos
