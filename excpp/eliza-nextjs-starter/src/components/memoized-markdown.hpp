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

const Markdown = dynamic(() => import("markdown-to-jsx"), {
  ssr: true,
});



const MemoizedMarkdownBlock = memo(
  ({ content, options }: { content: string; options: any }) => {
    return <Markdown options={options}>{content}</Markdown>;
  },
  (prevProps, nextProps) => prevProps.content === nextProps.content,
);

MemoizedMarkdownBlock.displayName = "MemoizedMarkdownBlock";

const MemoizedMarkdown = memo(
  ({ content, id, options }: { content: string; id: string; options: any }) => {
    const blocks = useMemo(() => parseMarkdownIntoBlocks(content), [content]);

    return blocks.map((block, index) => (
      <MemoizedMarkdownBlock
        content={block}
        options={options}
        key={`${id}-block_${index}`}
      />
    ));
  },
);

MemoizedMarkdown.displayName = "MemoizedMarkdown";

} // namespace elizaos
