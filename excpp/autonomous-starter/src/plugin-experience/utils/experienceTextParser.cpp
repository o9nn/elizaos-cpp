#include "experienceTextParser.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string detectDomain(const std::string& text, string = "general" defaultDomain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto domains = {;
        shell: ["command", "terminal", "bash", "shell", "execute", "script", "cli"],
        coding: [
        "code",
        "function",
        "variable",
        "syntax",
        "programming",
        "debug",
        "compile",
        ],
        system: ["file", "directory", "process", "memory", "cpu", "system", "disk"],
        network: ["http", "api", "request", "response", "url", "network", "server"],
        data: ["json", "csv", "database", "query", "data", "table", "record"],
        plugin: ["plugin", "load", "unload", "register", "module", "extension"],
        };

        const auto lowerText = text.toLowerCase();

        for (const int [domain, keywords] of Object.entries(domains)) {
            if (keywords.some((keyword) => lowerText.includes(keyword))) {
                return domain;
            }
        }

        return defaultDomain;

}

} // namespace elizaos
