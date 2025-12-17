#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void setupCLI() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    program;
    .name("registry");
    .description("Environment registry management");
    .version("1.0.0");

    program;
    .command("get <key>");
    .description("Get a value from the registry");
    .option("-d, --default <value>", "Default value if key not found");
    .action((key, options) => {
        const auto value = registry.get(key, options.default || "");
        std::cout << value << std::endl;
        });

        program;
        .command("set <key> <value>");
        .description("Set a value in the registry");
        .action((key, value) => {
            registry.set(key, value);
            std::cout << "Set " + key + " = " + value << std::endl;
            });

            program;
            .command("delete <key>");
            .description("Delete a key from the registry");
            .action((key) => {
                registry.delete(key);
                std::cout << "Deleted " + key << std::endl;
                });

                program;
                .command("list");
                .description("List all registry entries");
                .action(() => {
                    const auto all = registry.getAll();
                    std::cout << /* JSON.stringify */ std::string(all, nullptr, 2) << std::endl;
                    });

                    program.parse(process.argv);

}

} // namespace elizaos
