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
;
;
;

struct SplitButtonAction {
    React.ReactNode label;
    () => void onClick;
    std::optional<React.ReactNode> icon;
    std::optional<bool> disabled;
    std::optional<'default' | 'destructive'> variant;
};


struct SplitButtonProps {
    SplitButtonAction mainAction;
    std::vector<SplitButtonAction> actions;
    std::optional<'default' | 'outline' | 'secondary' | 'ghost' | 'link' | 'destructive'> variant;
    std::optional<'default' | 'sm' | 'lg' | 'icon'> size;
    std::optional<bool> disabled;
    std::optional<std::string> className;
};


const SplitButton = React.forwardRef<HTMLDivElement, SplitButtonProps>(
  ({ mainAction, actions, variant = 'default', size = 'default', disabled, className }, ref) => {
    return (
      <div ref={ref} className={cn('flex w-full', className)}>
        <Button
          type="button"
          variant={variant}
          size={size}
          onClick={mainAction.onClick}
          disabled={disabled || mainAction.disabled}
          className={cn(
            'rounded-r-none flex-1',
            variant === 'destructive' ? 'border-r border-red-700' : 'border-r-0'
          )}
        >
          {mainAction.icon && <span className="mr-2">{mainAction.icon}</span>}
          {mainAction.label}
        </Button>

        <DropdownMenu>
          <DropdownMenuTrigger asChild>
            <Button
              type="button"
              variant={variant}
              size={size}
              disabled={disabled}
              className="rounded-l-none px-2 flex-shrink-0"
            >
              <ChevronDown className="h-4 w-4" />
              <span className="sr-only">More options</span>
            </Button>
          </DropdownMenuTrigger>
          <DropdownMenuContent align="end">
            {actions.map((action, index) => (
              <React.Fragment key={index}>
                <DropdownMenuItem
                  onClick={action.onClick}
                  disabled={action.disabled}
                  className={cn(
                    action.variant === 'destructive' &&
                      'text-destructive focus:text-destructive hover:bg-red-50 dark:hover:bg-red-950/50'
                  )}
                >
                  {action.icon && <span className="mr-2">{action.icon}</span>}
                  {action.label}
                </DropdownMenuItem>
                {index < actions.length - 1 && <DropdownMenuSeparator />}
              </React.Fragment>
            ))}
          </DropdownMenuContent>
        </DropdownMenu>
      </div>
    );
  }
);

SplitButton.displayName = 'SplitButton';

} // namespace elizaos
