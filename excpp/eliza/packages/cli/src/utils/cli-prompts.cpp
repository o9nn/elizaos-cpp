#include "cli-prompts.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> promptForMultipleItems(const std::string& fieldName, const std::vector<string[] =>& initial) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto items = [...initial];
    std::cout << "\n" + std::to_string(fieldName) << std::endl;
    if (initial.length > 0) {
        std::cout << 'Current values:' << std::endl;
        initial.forEach((item, i) => logger.info(`  ${i + 1}. ${item}`));
        std::cout << '\nPress Enter to keep existing values << or start typing new ones:' << std::endl;
    }

    while (true) {
        const auto val = "> " + std::to_string(fieldName) + ":";
        if (val == NAV_NEXT) break;
        if (val == NAV_BACK) {
            if (items.length == initial.length) return initial; // Return original if no change
            break;
        }
        if (val == 'cancel') return initial;
        items.push(val);
    }
    return items;

}

std::future<bool> confirmAction(const std::string& message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto response = clack.confirm({;
        message,
        initialValue: false,
        });

        if (clack.isCancel(response)) {
            clack.cancel('Operation cancelled.');
            process.exit(0);
        }

        return Boolean(response);

}

} // namespace elizaos
