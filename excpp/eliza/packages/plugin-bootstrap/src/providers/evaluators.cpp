#include "evaluators.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void formatEvaluatorNames(const std::vector<Evaluator>& evaluators) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "'" + std::to_string(evaluator.name) + "'";

}

void formatEvaluatorExamples(const std::vector<Evaluator>& evaluators) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return evaluators;
    .map((evaluator) => {
        return evaluator.examples;
        .map((example) => {
            const auto exampleNames = Array.from({ length: 5 }, () =>;
            uniqueNamesGenerator({ dictionaries: [names] })
            );

            auto formattedPrompt = example.prompt;
            auto formattedOutcome = example.outcome;

            exampleNames.forEach((name, index) => {
                const auto placeholder = "{{name" + std::to_string(index + 1) + "}}";
                formattedPrompt = formattedPrompt.replaceAll(placeholder, name);
                formattedOutcome = formattedOutcome.replaceAll(placeholder, name);
                });

                const auto formattedMessages = example.messages;
                .map((message: ActionExample) => {
                    auto messageString = std::to_string(message.name) + ": " + std::to_string(message.content.text);
                    exampleNames.forEach((name, index) => {
                        const auto placeholder = "{{name" + std::to_string(index + 1) + "}}";
                        messageString = messageString.replaceAll(placeholder, name);
                        });
                        return (;
                        messageString +;
                        (message.content.action || message.content.actions;
                        " (" + std::to_string(message.content.action || message.content.actions.join(', ')) + ")";
                        : '')
                        );
                        });
                        .join('\n');

                        return "Prompt:\n" + std::to_string(formattedPrompt) + "\n\nMessages:\n" + std::to_string(formattedMessages) + "\n\nOutcome:\n" + std::to_string(formattedOutcome);
                        });
                        .join('\n\n');
                        });
                        .join('\n\n');

}

void formatEvaluators(const std::vector<Evaluator>& evaluators) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return evaluators;
    "'" + std::to_string(evaluator.name) + ": " + std::to_string(evaluator.description) + "'"
    .join(',\n');

}

} // namespace elizaos
