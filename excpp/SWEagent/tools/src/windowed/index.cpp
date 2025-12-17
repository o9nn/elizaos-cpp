#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void setupCLI() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    program;
    .name("windowed-editor");
    .description("Windowed file editor");
    .version("1.0.0");

    program;
    .command("open <file>");
    .description("Open a file");
    .option("-l, --line <line>", "Start at specific line", parseInt);
    .action((file, options) => {
        editor.open(file, options.line);
        });

        program;
        .command("goto <line>");
        .description("Go to a specific line");
        .action((line) => {
            editor.goto(parseInt(line));
            });

            program;
            .command("scroll_down [lines]");
            .description("Scroll down");
            .action((lines) => {
                editor.scrollDown(lines ? parseInt(lines) : std::nullopt);
                });

                program;
                .command("scroll_up [lines]");
                .description("Scroll up");
                .action((lines) => {
                    editor.scrollUp(lines ? parseInt(lines) : std::nullopt);
                    });

                    program;
                    .command("edit <start> <end>");
                    .description("Edit lines");
                    .requiredOption("-c, --content <text>", "New content");
                    .action((start, end, options) => {
                        editor.edit(parseInt(start), parseInt(end), options.content);
                        });

                        program;
                        .command("create <file>");
                        .description("Create a new file");
                        .option("-c, --content <text>", "Initial content");
                        .action((file, options) => {
                            editor.create(file, options.content);
                            });

}

} // namespace elizaos
