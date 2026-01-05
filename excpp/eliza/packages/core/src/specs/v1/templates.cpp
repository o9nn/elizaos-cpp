#include "templates.hpp"
#include <iostream>
#include <stdexcept>
#include <variant>
#include <type_traits>

namespace elizaos {

std::function<std::string(State)> createTemplateFunction(TemplateType template) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    // Check if template is a string or function using std::holds_alternative
    // Assuming TemplateType is std::variant<std::string, std::function<std::string(State)>>
    
    if (std::holds_alternative<std::string>(template)) {
        std::string templateStr = std::get<std::string>(template);
        // For string templates, return a function that returns the string
        return [templateStr](State state) { return templateStr; };
    } else {
        // For function templates, return the function itself
        return std::get<std::function<std::string(State)>>(template);
    }
}

std::string processTemplate(TemplateType template, State state) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    // Check if template is a string or function
    if (std::holds_alternative<std::string>(template)) {
        return std::get<std::string>(template);
    } else {
        auto templateFunc = std::get<std::function<std::string(State)>>(template);
        return templateFunc(state);
    }
}

} // namespace elizaos
