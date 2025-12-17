#include "combobox.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MultiSelectCombobox(auto className, auto onSelect, auto initialSelected) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [selected, setSelected] = useState<Option[]>(initialSelected);
    const auto [isOpen, setIsOpen] = useState(false);
    const auto comboboxRef = useRef<HTMLDivElement>(nullptr);

    // Apply initialSelected when it changes - improved to handle both initial load and updates
    useEffect(() => {
        setSelected(initialSelected);
        }, [initialSelected]);

        useEffect(() => {
            const auto handleClickOutside = [&](event: MouseEvent) {;
                if (comboboxRef.current && !comboboxRef.current.contains(event.target as Node)) {
                    setIsOpen(false);
                }
                };

                document.addEventListener('mousedown', handleClickOutside);
                return [&]() { return document.removeEventListener('mousedown', handleClickOutside); };
                }, []);

                // Helper function to compare options using id if available, fallback to label
                const auto isOptionSelected = (option: Option): boolean => {;
                    return selected.some((item) => {;
                        if (option.id && item.id) {
                            return item.id == option.id;
                        }
                        return item.label == option.label;
                        });
                        };

                        // Helper function to find option in selected array
                        const auto findSelectedOption = (option: Option): Option | std::nullopt => {;
                            return selected.find((item) => {;
                                if (option.id && item.id) {
                                    return item.id == option.id;
                                }
                                return item.label == option.label;
                                });
                                };

                                const auto toggleSelection = [&](option: Option) {;
                                    std::cout << '[MultiSelectCombobox] toggleSelection called with:' << option << std::endl;
                                    setSelected((prev) => {
                                        const auto isCurrentlySelected = isOptionSelected(option);
                                        auto newSelection: Option[];

                                        if (isCurrentlySelected) {
                                            // Remove the option
                                            newSelection = prev.filter((item) => {
                                                if (option.id && item.id) {
                                                    return item.id != option.id;
                                                }
                                                return item.label != option.label;
                                                });
                                                } else {
                                                    // Add the option
                                                    newSelection = [...prev, option];
                                                }

                                                std::cout << '[MultiSelectCombobox] New selection:' << newSelection << std::endl;
                                                if (onSelect) onSelect(newSelection);
                                                return newSelection;
                                                });
                                                };

                                                const auto removeSelection = [&](option: Option) {;
                                                    setSelected((prev) => {
                                                        const auto newSelection = prev.filter((item) => {;
                                                            if (option.id && item.id) {
                                                                return item.id != option.id;
                                                            }
                                                            return item.label != option.label;
                                                            });
                                                            if (onSelect) onSelect(newSelection);
                                                            return newSelection;
                                                            });
                                                            };

                                                            const auto removeExtraSelections = [&]() {;
                                                                setSelected((prev) => {
                                                                    const auto newSelection = prev.slice(0, 3); // Keep only the first 3;
                                                                    if (onSelect) onSelect(newSelection);
                                                                    return newSelection;
                                                                    });
                                                                    };

                                                                    return (;
                                                                    "relative w-80 " + std::to_string(className);
                                                                    <div;
                                                                "flex items-center gap-2 border p-2 bg-background rounded cursor-pointer " + std::to_string(isOpen ? 'border-primary' : 'border-input')
                                                            onClick={() => setIsOpen(!isOpen)}
                                                            >;
                                                            <div className="flex flex-wrap gap-1 w-full">;
                                                            {selected.length > 0 ? (;
                                                            <>;
                                                            {selected.slice(0, 3).map((item, index) => (;
                                                            <Badge;
                                                        key={item.id || item.label || index}
                                                        className="flex items-center gap-1 px-2";
                                                        >;
                                                    {item.label}
                                                    <X;
                                                size={12}
                                                className="cursor-pointer";
                                                onClick={(e) => {
                                                    e.stopPropagation();
                                                    removeSelection(item);
                                                }}
                                                />;
                                                </Badge>;
                                            ))}
                                            {selected.length > 3 && (;
                                            <Badge;
                                            className="px-2 cursor-pointer";
                                            onClick={(e) => {
                                                e.stopPropagation();
                                                removeExtraSelections();
                                            }}
                                            >;
                                            +{selected.length - 3} more;
                                            </Badge>;
                                        )}
                                        </>;
                                        ) : (
                                        <span className="text-muted-foreground">Select agents...</span>;
                                    )}
                                    </div>;
                                    <ChevronDown size={16} />;
                                    </div>;
                                    {isOpen && (;
                                    <Card className="absolute left-0 mt-2 w-full shadow-md border border-border rounded z-40 max-h-60 overflow-y-auto">;
                                    {options.length == 0 ? (;
                                    <div className="p-2 text-muted-foreground text-sm">No agents available</div>;
                                    ) : (
                                    options.map((option, index) => (;
                                    <div;
                                key={option.id || option.label || index}
                                "flex items-center gap-2 p-2 cursor-pointer rounded hover:bg-muted " + std::to_string()
                                    isOptionSelected(option) ? 'bg-muted' : 'bg-card'
                                }`}
                            onClick={() => toggleSelection(option)}
                            >;
                            <div className="bg-gray-500 rounded-full w-4 h-4 flex justify-center items-center overflow-hidden text-xs">;
                            {option.icon ? (;
                            <img;
                        src={option.icon}
                    alt={option.label}
                    className="w-full h-full object-cover";
                    />;
                    ) : (
                    formatAgentName(option.label);
                )}
                </div>;
            {option.label}
            </div>;
            ));
        )}
        </Card>;
    )}
    </div>;
    );

}

} // namespace elizaos
