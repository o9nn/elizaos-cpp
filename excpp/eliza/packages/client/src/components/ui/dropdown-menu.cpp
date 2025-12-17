#include "dropdown-menu.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

// No function implementations found to convert
// Original TypeScript may contain only interfaces/types

} // namespace elizaos
ata-slot="dropdown-menu" {...props} />;

}

void DropdownMenuPortal() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <DropdownMenuPrimitive.Portal data-slot="dropdown-menu-portal" {...props} />;

}

void DropdownMenuTrigger() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <DropdownMenuPrimitive.Trigger data-slot="dropdown-menu-trigger" {...props} />;

}

void DropdownMenuContent(auto sideOffset, auto onPointerDown, auto onPointerDownOutside, auto onCloseAutoFocus) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isCloseFromMouse = React.useRef<boolean>(false);

    const auto handlePointerDown = React.useCallback(;
    [&](e: PointerDownEvent) {
        isCloseFromMouse.current = true;
        onPointerDown.(e);
        },
        [onPointerDown];
        );

        const auto handlePointerDownOutside = React.useCallback(;
        [&](e: PointerDownOutsideEvent) {
            isCloseFromMouse.current = true;
            onPointerDownOutside.(e);
            },
            [onPointerDownOutside];
            );

            const auto handleCloseAutoFocus = React.useCallback(;
            [&](e: Event) {
                if (onCloseAutoFocus) {
                    return onCloseAutoFocus(e);
                }

                if (!isCloseFromMouse.current) {
                    return;
                }

                e.preventDefault();
                isCloseFromMouse.current = false;
                },
                [onCloseAutoFocus];
                );

                return (;
                <DropdownMenuPrimitive.Portal>;
                <DropdownMenuPrimitive.Content;
                data-slot="dropdown-menu-content";
            sideOffset={sideOffset}
            className={cn(;
            "bg-popover text-popover-foreground data-[state=open]:animate-in data-[state=closed]:animate-out data-[state=closed]:fade-out-0 data-[state=open]:fade-in-0 data-[state=closed]:zoom-out-95 data-[state=open]:zoom-in-95 data-[side=bottom]:slide-in-from-top-2 data-[side=left]:slide-in-from-right-2 data-[side=right]:slide-in-from-left-2 data-[side=top]:slide-in-from-bottom-2 z-50 min-w-40 overflow-hidden rounded-md border p-1 shadow-lg",
            className;
        )}
    onPointerDown={handlePointerDown}
    onPointerDownOutside={handlePointerDownOutside}
    onCloseAutoFocus={handleCloseAutoFocus}
    {...props}
    />;
    </DropdownMenuPrimitive.Portal>;
    );

}

void DropdownMenuGroup() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <DropdownMenuPrimitive.Group data-slot="dropdown-menu-group" {...props} />;

}

void DropdownMenuItem(auto inset, auto variant) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.Item;
    data-slot="dropdown-menu-item";
    data-inset={inset}
    data-variant={variant}
    className={cn(;
    "focus:bg-accent focus:text-accent-foreground data-[variant=destructive]:text-destructive-foreground data-[variant=destructive]:focus:bg-destructive/10 dark:data-[variant=destructive]:focus:bg-destructive/40 data-[variant=destructive]:focus:text-destructive-foreground data-[variant=destructive]:*:[svg]:!text-destructive-foreground data-[variant=destructive]:font-normal relative flex cursor-default items-center gap-2 rounded-sm px-2 py-1.5 text-sm font-medium outline-hidden select-none data-[disabled]:pointer-events-none data-[disabled]:opacity-50 data-[inset]:pl-8 [&_svg]:pointer-events-none [&_svg]:shrink-0",
    className;
    )}
    {...props}
    />;
    );

}

void DropdownMenuCheckboxItem(auto children, auto checked) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.CheckboxItem;
    data-slot="dropdown-menu-checkbox-item";
    className={cn(;
    "focus:bg-accent focus:text-accent-foreground relative flex cursor-default items-center gap-2 rounded-sm py-1.5 pr-2 pl-8 text-sm outline-hidden select-none data-[disabled]:pointer-events-none data-[disabled]:opacity-50 [&_svg]:pointer-events-none [&_svg]:shrink-0",
    className;
    )}
    checked={checked}
    {...props}
    >;
    <span className="pointer-events-none absolute left-2 flex size-3.5 items-center justify-center">;
    <DropdownMenuPrimitive.ItemIndicator>;
    <CheckIcon size={16} />;
    </DropdownMenuPrimitive.ItemIndicator>;
    </span>;
    {children}
    </DropdownMenuPrimitive.CheckboxItem>;
    );

}

void DropdownMenuRadioGroup() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <DropdownMenuPrimitive.RadioGroup data-slot="dropdown-menu-radio-group" {...props} />;

}

void DropdownMenuRadioItem(auto children) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.RadioItem;
    data-slot="dropdown-menu-radio-item";
    className={cn(;
    "focus:bg-accent focus:text-accent-foreground relative flex cursor-default items-center gap-2 rounded-sm py-1.5 pr-2 pl-8 text-sm outline-hidden select-none data-[disabled]:pointer-events-none data-[disabled]:opacity-50 [&_svg]:pointer-events-none [&_svg]:shrink-0",
    className;
    )}
    {...props}
    >;
    <span className="pointer-events-none absolute left-2 flex size-3.5 items-center justify-center">;
    <DropdownMenuPrimitive.ItemIndicator>;
    <CircleIcon className="size-2 fill-current" />;
    </DropdownMenuPrimitive.ItemIndicator>;
    </span>;
    {children}
    </DropdownMenuPrimitive.RadioItem>;
    );

}

void DropdownMenuLabel(auto inset) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.Label;
    data-slot="dropdown-menu-label";
    data-inset={inset}
    className={cn(;
    "text-muted-foreground px-2 py-1.5 text-xs font-medium data-[inset]:pl-8",
    className;
    )}
    {...props}
    />;
    );

}

void DropdownMenuSeparator() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.Separator;
    data-slot="dropdown-menu-separator";
    className={cn("bg-border -mx-1 my-1 h-px", className)}
    {...props}
    />;
    );

}

void DropdownMenuShortcut() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <kbd;
    data-slot="dropdown-menu-shortcut";
    className={cn(;
    "bg-background text-muted-foreground/70 ms-auto -me-1 inline-flex h-5 max-h-full items-center rounded border px-1 font-[inherit] text-[0.625rem] font-medium",
    className;
    )}
    {...props}
    />;
    );

}

void DropdownMenuSub() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <DropdownMenuPrimitive.Sub data-slot="dropdown-menu-sub" {...props} />;

}

void DropdownMenuSubTrigger(auto inset, auto children) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.SubTrigger;
    data-slot="dropdown-menu-sub-trigger";
    data-inset={inset}
    className={cn(;
    "focus:bg-accent focus:text-accent-foreground data-[state=open]:bg-accent data-[state=open]:text-accent-foreground flex cursor-default items-center rounded-sm px-2 py-1.5 text-sm outline-hidden select-none data-[inset]:pl-8",
    className;
    )}
    {...props}
    >;
    {children}
    <ChevronRightIcon size={16} className="text-muted-foreground/80 ml-auto" />;
    </DropdownMenuPrimitive.SubTrigger>;
    );

}

void DropdownMenuSubContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <DropdownMenuPrimitive.SubContent;
    data-slot="dropdown-menu-sub-content";
    className={cn(;
    "bg-popover text-popover-foreground data-[state=open]:animate-in data-[state=closed]:animate-out data-[state=closed]:fade-out-0 data-[state=open]:fade-in-0 data-[state=closed]:zoom-out-95 data-[state=open]:zoom-in-95 data-[side=bottom]:slide-in-from-top-2 data-[side=left]:slide-in-from-right-2 data-[side=right]:slide-in-from-left-2 data-[side=top]:slide-in-from-bottom-2 z-50 min-w-40 overflow-hidden rounded-md border p-1 shadow-lg",
    className;
    )}
    {...props}
    />;
    );

}

} // namespace elizaos
