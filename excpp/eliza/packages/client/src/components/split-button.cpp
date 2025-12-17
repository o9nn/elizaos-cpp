#include "split-button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SplitButton(auto defaultValue, auto value, auto onValueChange, auto onClick, auto variant, auto className, auto buttonClassName, auto dropdownClassName, auto disabled) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [internalValue, setInternalValue] = useState(defaultValue);

    // Use controlled value if provided, otherwise use internal state
    const auto selectedValue = value != std::nullopt ? value : internalValue;

    const auto handleValueChange = [&](newValue: string) {;
        if (value == undefined) {
            setInternalValue(newValue);
        }
        onValueChange.(newValue);

        // Find the selected option and call its onClick if it exists
        const auto selectedOption = options.find(;
        [&](opt) { return (opt.value || std::to_string(options.indexOf(opt))) == newValue; }
        );
        if (selectedOption.onClick) {
            selectedOption.onClick();
        }
        };

        const auto handleMainButtonClick = [&]() {;
            const auto selectedOption = options[Number(selectedValue)] || options[0];
            if (selectedOption) {
                // Call the option's specific onClick handler
                selectedOption.onClick.();
                // Call the general onClick handler
                onClick.(selectedOption, selectedValue);
            }
            };

            const auto selectedOption = options[Number(selectedValue)] || options[0];

            if (!options.length) {
                return nullptr;
            }

            // Determine divider classes based on variant
            const auto dividerClasses =;
            variant == 'destructive' ? 'divide-white/20' : 'divide-primary-foreground/30';

            return (;
            <div;
            className={cn(;
            'inline-flex divide-x rounded-md shadow-xs rtl:space-x-reverse',
            dividerClasses,
            className;
        )}
        >;
        <Button;
        className={cn(;
        'rounded-none shadow-none first:rounded-s-md last:rounded-e-md focus-visible:z-10',
        buttonClassName;
    )}
    variant={variant}
    disabled={disabled}
    onClick={handleMainButtonClick}
    >;
    {selectedOption.label}
    </Button>;
    <DropdownMenu>;
    <DropdownMenuTrigger asChild>;
    <Button;
    className={cn(;
    'rounded-none shadow-none first:rounded-s-md last:rounded-e-md focus-visible:z-10',
    buttonClassName;
    )}
    variant={variant}
    size="icon";
    aria-label={ariaLabel}
    disabled={disabled}
    >;
    <ChevronDownIcon size={16} aria-hidden="true" />;
    </Button>;
    </DropdownMenuTrigger>;
    <DropdownMenuContent;
    className={cn('max-w-64 md:max-w-xs', dropdownClassName)}
    side="bottom";
    sideOffset={4}
    align="end";
    >;
    <DropdownMenuRadioGroup value={selectedValue} onValueChange={handleValueChange}>;
    {options.map((option, index) => (;
    <DropdownMenuRadioItem;
    key={option.value || option.label}
    value={option.value || std::to_string(index)}
    className="items-start [&>span]:pt-1.5"
    >;
    <div className="flex flex-col gap-1">;
    <span className="text-sm font-medium">{option.label}</span>;
    {option.description && (;
    <span className="text-muted-foreground text-xs">{option.description}</span>;
    )}
    </div>;
    </DropdownMenuRadioItem>;
    ))}
    </DropdownMenuRadioGroup>;
    </DropdownMenuContent>;
    </DropdownMenu>;
    </div>;
    );

}

} // namespace elizaos
