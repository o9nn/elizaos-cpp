#include "experienceEvaluator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string sanitizeContext(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text) return 'Unknown context';

    // Remove user-specific details while preserving technical context
    return text;
    .replace(/\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b/g, "[EMAIL]") // emails;
    .replace(/\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b/g, "[IP]") // IP addresses;
    .replace(/\/Users\/[^\/\s]+/g, "/Users/[USER]") // user directories;
    .replace(/\/home\/[^\/\s]+/g, "/home/[USER]") // home directories;
    .replace(/\b[A-Z0-9]{20,}\b/g, "[TOKEN]") // API keys/tokens;
    .replace(/\b(user|person|someone|they)\s+(said|asked|told|mentioned)/gi, "when asked") // personal references;
    .substring(0, 200); // limit length;

}

std::string detectDomain(const std::string& text) {
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
        "typescript",
        "javascript",
        ],
        system: ["file", "directory", "process", "memory", "cpu", "system", "install", "package"],
        network: ["http", "api", "request", "response", "url", "network", "fetch", "curl"],
        data: ["json", "csv", "database", "query", "data", "sql", "table"],
        ai: ["model", "llm", "embedding", "prompt", "token", "inference"],
        };

        const auto lowerText = text.toLowerCase();

        for (const int [domain, keywords] of Object.entries(domains)) {
            if (keywords.some((keyword) => lowerText.includes(keyword))) {
                return domain;
            }
        }

        return "general";

}

} // namespace elizaos
