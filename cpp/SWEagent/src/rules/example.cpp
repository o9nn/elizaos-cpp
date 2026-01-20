#include "example.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> validatePythonExample() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "== Python Validation Example ==" << std::endl;

    const auto pythonCode = `;
    def process_data(data):
    # This function needs type annotations;
    with open("data.txt"):
    content = f.read();
    return content;
    `;

    const auto validator = new PythonValidator();
    const auto result = validator.validate(pythonCode, "example.py");

    std::cout << "Valid:" << result.valid << std::endl;
    std::cout << "Violations:" << result.violations << std::endl;
    std::cout << "Warnings:" << result.warnings << std::endl;

}

std::future<void> validateTypeScriptExample() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n== TypeScript Validation Example ==" << std::endl;

    const auto tsCode = `;
    function processData(data: unknown) {
        const auto fs = require("fs");
        const auto content = fs.readFileSync("data.txt", "utf-8");
        return content;
    }
    `;

    const auto validator = new TypeScriptValidator();
    const auto result = validator.validate(tsCode, "example.ts");

    std::cout << "Valid:" << result.valid << std::endl;
    std::cout << "Violations:" << result.violations << std::endl;
    std::cout << "Warnings:" << result.warnings << std::endl;

}

void projectStructureExample() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n== Project Structure Example ==" << std::endl;

    // Get information about a specific component
    const auto agentInfo = getComponentByPath("sweagent/agent/agents.py");
    std::cout << "Agent component:" << agentInfo << std::endl;

    // Access project structure directly
    std::cout << "\nMain entry points:" << std::endl;
    PROJECT_STRUCTURE.mainEntryPoints.forEach((entry) => {
        std::cout << "  - " + entry.path + ": " + entry.description << std::endl;
        });

        std::cout << "\nInspectors:" << std::endl;
        PROJECT_STRUCTURE.inspectors.forEach((inspector) => {
            std::cout << "  - " + inspector.name + " (" + inspector.type + "): " + inspector.path << std::endl;
            });

}

void rulesExample() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n== Rules Example ==" << std::endl;

    std::cout << "Python rules:" << std::endl;
    const auto pythonRules = getApplicableRules("example.py");
    pythonRules.forEach((rule) => {
        std::cout << "  - " + rule.id + ": " + rule.rule << std::endl;
        });

        std::cout << "\nTypeScript rules:" << std::endl;
        const auto tsRules = getApplicableRules("example.ts");
        tsRules.forEach((rule) => {
            std::cout << "  - " + rule.id + ": " + rule.rule << std::endl;
            });

}

void exportExample() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n== Export Example ==" << std::endl;

    const auto cursorRules = exportAllRulesToCursor();

    std::cout << "Exported rules:" << std::endl;
    Object.keys(cursorRules).forEach((filename) => {
        std::cout << "  - " + filename + " (" + std::to_string(cursorRules[filename].size()) + " bytes)" << std::endl;
        });

}

std::future<void> runExamples() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    validatePythonExample();
    validateTypeScriptExample();
    projectStructureExample();
    rulesExample();
    exportExample();

}

} // namespace elizaos
