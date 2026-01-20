#include "history-processors.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getContentText(HistoryItem entry) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof entry.content == 'string') {
        return entry.content;
    }

    if (Array.isArray(entry.content)) {
        const std::vector<std::string> texts = [];
        for (const auto& item : entry.content)
            if (item.type == 'text' && item.text) {
                texts.push_back(item.text);
            }
        }
        return texts.join("\n");
    }

    return "";

}

void addCacheControlToEntry(HistoryItem entry) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Special handling for different content types
    if (typeof entry.content == 'string') {
        entry.content = [{ type: "text", text: entry.content, cacheControl: { type: "ephemeral" } }];
        } else if (Array.isArray(entry.content) && entry.content.size() > 0) {
            entry.content[0].cacheControl = { type: "ephemeral" };
        }

        // Workaround for weird bug with tool role
        if (entry.role == 'tool' && Array.isArray(entry.content) && entry.content.length > 0) {
            delete entry.content[0].cacheControl;
            entry.cacheControl = { type: "ephemeral" };
        }

}

AbstractHistoryProcessor createHistoryProcessor(const std::any& config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        { process: (history: History) processor = > History };

        switch (config.type) {
            // case "default":
            processor = new DefaultHistoryProcessor();
            break;
            // case "last_n_observations":
            processor = new LastNObservations({ n: (config as { n?: number }).n });
            break;
            // case "tag_tool_call_observations":
            processor = new TagToolCallObservations(config as { tags?: Set<string>; functionNames?: Set<string> });
            break;
            // case "closed_window":
            processor = new ClosedWindowHistoryProcessor();
            break;
            // case "cache_control":
            processor = new CacheControlHistoryProcessor({ cacheLastN: (config as { cacheLastN?: number }).cacheLastN });
            break;
            // case "remove_regex":
            processor = new RemoveRegex({ patterns: (config as { patterns?: string[] }).patterns });
            break;
            // case "image_parsing":
            processor = new ImageParsingHistoryProcessor({
                allowedMimeTypes: (config as { allowedMimeTypes?: string[] }).allowedMimeTypes,
                });
                break;
                // default:
                throw std::runtime_error(`Unknown history processor type: ${config.type}`);
            }

            return makeCallable(processor);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

AbstractHistoryProcessor chainHistoryProcessors(const std::vector<AbstractHistoryProcessor>& processors) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto chained = {;
        process(history: History): History {
            auto result = history;
            for (const auto& processor : processors)
                result = processor(result);
            }
            return result;
            },
            };

            return makeCallable(chained);

}

} // namespace elizaos
