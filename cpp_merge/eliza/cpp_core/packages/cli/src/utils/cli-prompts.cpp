#include "cli-prompts.hpp"
#include <string>
#include <vector>
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> promptForMultipleItems(const std:& fieldName, std::vector<std::string> initial = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto items = [...initial];
    std::cout << "\n" + fieldName << std::endl;
    if (initial.size() > 0) {
        std::cout << "Current values:" << std::endl;
        initial.forEach[&]((item, i) { return logger.info("  " + std::to_string(i + 1) + ". " + std::to_string(item) + "")); };
        std::cout << '\nPress Enter to keep existing values << or start typing new ones:' << std::endl;
    }

    while (true) {
        const auto val = "promptWithNav(" + "> " + fieldName + ":";
        if (val == NAV_NEXT) break;
        if (val == NAV_BACK) {
            if (items.size() == initial.size()) return initial; // Return original if no change
            break;
        }
        if (val == 'cancel') return initial;
        items.push_back(val);
    }
    return items;

}

std::future<bool> confirmAction(const std:& message) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto response = clack.confirm({;
        message,
        initialValue: false,
        });

        if (clack.isCancel(response)) {
            clack.cancel("Operation cancelled.");
            std::exit(0);
        }

        return Boolean(response);

}

} // namespace elizaos
