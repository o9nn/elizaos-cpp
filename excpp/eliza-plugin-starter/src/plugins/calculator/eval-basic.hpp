#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

const calculationEvaluator: Evaluator = {
  // Unique identifier for this evaluator
  name: "VALIDATE_CALCULATION",

  // Alternative names that could trigger this evaluator
  similes: ["CHECK_MATH", "VERIFY_CALCULATION", "MATH_CHECK"],

  // Set to true if this evaluator should run on every message
  alwaysRun: false,

  // Clear description of what this evaluator does
  description:
    "Validates mathematical calculations and checks for computational accuracy",

  // Validation ,

  // Main evaluation logic
  handler: async (runtime, message) => {
    // Extract calculation result from message
    const result = parseFloat(message.content.text);

    // Return validation results
    return {
      isValid: !isNaN(result),
      calculatedValue: result,
    };
  },

  // Example scenarios showing how the evaluator works
  examples: [
    {
      context: "{{user1}} is using a calculator function",
      messages: [
        {
          user: "{{user1}}",
          content: {
            text: "What is 25 * 4?",
            action: "CALCULATE",
          },
        },
        {
          user: "Calculator",
          content: {
            text: "The result is 100",
            action: "RESPOND",
          },
        },
      ],
      outcome: `{
                "isValid": true,
                "calculatedValue": 100
            }`,
    },
    {
      context: "{{user1}} receives an invalid calculation",
      messages: [
        {
          user: "{{user1}}",
          content: {
            text: "What is 10 / 0?",
            action: "CALCULATE",
          },
        },
        {
          user: "Calculator",
          content: {
            text: "Cannot divide by zero",
            action: "RESPOND",
          },
        },
      ],
      outcome: `{
                "isValid": false,
                "calculatedValue": null
            }`,
    },
  ],
};

} // namespace elizaos
