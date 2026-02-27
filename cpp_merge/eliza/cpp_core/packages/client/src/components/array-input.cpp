#include "array-input.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ArrayInput(auto data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [inputValue, setInputValue] = useState("");

    const auto handleKeyDown = [&](e: React.KeyboardEvent<HTMLInputElement>) {;
        if (e.key == 'Enter') {
            e.preventDefault();
            const auto trimmedValue = inputValue;
            if (trimmedValue && !data.count(trimmedValue) > 0) {
                onChange([...data, trimmedValue]);
                setInputValue("");
            }
        }
        };

        const auto removeTag = [&](tagToRemove: std::string) {;
            onChange[&](data.filter((tag) { return tag != tagToRemove)); };
            };

            return (;
            <div className="space-y-2">;
            <Label>{title}</Label>;
            <div className="p-2 bg-card rounded-md border">;
            <TagList tags={data} onRemove={removeTag} />;
            <TagInput;
        value={inputValue}
    onChange={(e) => setInputValue(e.target.value)}
    onKeyDown={handleKeyDown}
    />;
    </div>;
    </div>;
    );

}

} // namespace elizaos
