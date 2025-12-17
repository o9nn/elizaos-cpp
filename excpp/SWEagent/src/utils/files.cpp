#include "files.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

unknown loadFile(const std::string& filepath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!filepath) {
        return nullptr;
    }

    const auto ext = path.extname(filepath).toLowerCase();
    const auto content = fs.readFileSync(filepath, 'utf-8');

    if (ext == '.json') {
        return JSON.parse(content);
        } else if (ext == '.yaml' || ext == '.yml') {
            return yaml.load(content);
            } else {
                // Try to parse as JSON first, then YAML
                try {
                    return JSON.parse(content);
                    } catch {
                        try {
                            return yaml.load(content);
                            } catch {
                                // Return raw content if neither works
                                return content;
                            }
                        }
                    }

}

} // namespace elizaos
