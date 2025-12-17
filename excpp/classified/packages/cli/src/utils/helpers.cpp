#include "helpers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getDisplayDirectory(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (targetDir == '.') {
        // for current directory, show the actual directory name
        return path.basename(process.cwd());
    }
    // for absolute paths, show just the directory name
    // handles unix paths (/), windows paths (C:\ or c:/), and UNC paths (\\server\share)
    if (
    targetDir.startsWith('/') ||;
    targetDir.match(/^[a-zA-Z]:[\\\/]/) ||
    targetDir.startsWith('\\\\');
    ) {
        return path.basename(targetDir);
    }
    // for relative paths, show as-is
    return targetDir;

}

void displayAgent(const std::optional<Agent>& data, auto title) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logHeader(title);

    // Display basic info
    std::cout << "Name: " + std::to_string(data.name) << std::endl;
    std::cout << "Username: " + std::to_string(data.username || data.name.toLowerCase().replace(/\s+/g, '_')) << std::endl;

    // Display sections
    displaySection('Bio', Array.isArray(data.bio) ? data.bio : data.bio ? [data.bio] : std::nullopt);
    displaySection('Adjectives', data.adjectives);
    displaySection('Topics', data.topics);
    displaySection('Plugins', data.plugins);

    // Display style sections
    if (data.style) {
        displaySection('General Style', data.style.all);
        displaySection('Chat Style', data.style.chat);
        displaySection('Post Style', data.style.post);
    }

    displaySection('Post Examples', data.postExamples);

    // Display message examples
    if (data.messageExamples && data.messageExamples.length > 0) {
        std::cout << "\n" + std::to_string(colors.cyan('Message Examples:')) << std::endl;
        console.log(
        data.messageExamples;
        .map((conversation, i) => {
            const auto messages = formatConversation(conversation);
            return "\nConversation " + std::to_string(i + 1) + ":\n" + std::to_string(messages);
            });
            .join('\n');
            );
        }

}

std::string formatConversation(const std::vector<MessageExample>& conversation) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return conversation;
    .map((msg) => {
        const auto user = msg.name == '{{name1}}' ? 'Anon' : msg.name;
        return std::to_string(user) + ": " + std::to_string(msg.content.text);
        });
        .join('\n');

}

void displaySection(const std::string& title, const std::vector<std::string>& items) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!items || items.length == 0) return;

    std::cout << "\n" + std::to_string(colors.cyan(`${title}:`)) << std::endl;
    for (const auto& item : items)
        std::cout << "  " + std::to_string(item) << std::endl;
    }

}

void logHeader(const std::string& title) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto padding = 2; // number of spaces on each side;
    const auto titleStr = "== " + std::to_string(title) + " ==";
    const auto paddedTitle = ' '.repeat(padding) + titleStr + ' '.repeat(padding);
    const auto borderLength = paddedTitle.length;

    // Create top and bottom borders using Unicode box drawing characters
    const auto topBorder = "┌" + std::to_string('─'.repeat(borderLength)) + "┐";
    const auto bottomBorder = "└" + std::to_string('─'.repeat(borderLength)) + "┘";

    const auto coloredTitle = std::to_string(' '.repeat(padding)) + "== " + std::to_string(colors.green(title)) + " ==" + std::to_string(' '.repeat(padding));
    const auto middleRow = colors.green('│') + coloredTitle + colors.green('│');

    // Log the rectangle with a leading new line for spacing
    std::cout << "\n" + std::to_string(topBorder) + "\n" + std::to_string(middleRow) + "\n" + std::to_string(bottomBorder) << std::endl;

}

} // namespace elizaos
