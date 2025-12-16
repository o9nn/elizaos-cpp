#include "ui/badge.hpp"
#include "ui/button.hpp"
#include "ui/input.hpp"
#include "ui/label.hpp"
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
;
;

using TagProps = {
  tag: string;
  onRemove: (tag: string) => void;
};

const Tag = ({ tag, onRemove }: TagProps) => (
  <Badge
    variant="outline"
    className="flex items-center gap-1.5 pr-1.5 text-sm py-1 px-2 transition-colors hover:bg-white hover:text-black dark:hover:text-black"
  >
    {tag}
    <Button
      type="button"
      variant="ghost"
      size="icon"
      onClick={() => onRemove(tag)}
      className="hover:bg-muted/20 rounded-full p-0.5 transition-colors h-auto w-auto min-w-0 min-h-0"
    >
      <X className="h-3 w-3" />
      <span className="sr-only">Remove {tag}</span>
    </Button>
  </Badge>
);

using TagListProps = std::vector<{
  tags: string>;
  onRemove: (tag: string) => void;
};

const TagList = ({ tags, onRemove }: TagListProps) => (
  <div className="flex flex-wrap gap-2 mb-2">
    {tags.map((tag) => (
      <Tag key={tag} tag={tag} onRemove={onRemove} />
    ))}
  </div>
);

using TagInputProps = {
  value: string;
  onChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
  onKeyDown: (e: React.KeyboardEvent<HTMLInputElement>) => void;
};

const TagInput = ({ value, onChange, onKeyDown }: TagInputProps) => (
  <Input
    value={value}
    onChange={onChange}
    onKeyDown={onKeyDown}
    placeholder="Type and press Enter to add..."
    className={cn('bg-background', !value && 'text-muted-foreground')}
  />
);

using ArrayInputProps = {
  title?: string;
  data: string[];
  onChange: (newData: string[]) => void;
};

default : ArrayInputProps) {
  const [inputValue, setInputValue] = useState('');

  const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key === 'Enter') {
      e.preventDefault();
      const trimmedValue = inputValue.trim();
      if (trimmedValue && !data.includes(trimmedValue)) {
        onChange([...data, trimmedValue]);
        setInputValue('');
      }
    }
  };

  const removeTag = (tagToRemove: string) => {
    onChange(data.filter((tag) => tag !== tagToRemove));
  };

  return (
    <div className="space-y-2">
      <Label>{title}</Label>
      <div className="p-2 bg-card rounded-md border">
        <TagList tags={data} onRemove={removeTag} />
        <TagInput
          value={inputValue}
          onChange={(e) => setInputValue(e.target.value)}
          onKeyDown={handleKeyDown}
        />
      </div>
    </div>
  );
}

} // namespace elizaos
