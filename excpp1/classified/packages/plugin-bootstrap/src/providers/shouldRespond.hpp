#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Configuration for name generation

// Example messages to determine if the agent should respond
/**
 * Array of message examples that the agent should respond to, ignore, or stop based on the content.
 * Each message example includes the sender's name, agent's name, and the expected response type.
 * Examples can include requests for help, questions, stories, or simple interactions like saying "marco".
 */
/**
 * Array of message examples to determine the agent response.
 * Each message example includes a conversation between the user and the agent,
 * as well as the expected response action for the agent (RESPOND, IGNORE, STOP).
 */
  // Examples where agent should RESPOND
// Response: RESPOND`,

// {{agentName}}: Sure, what is it
// {{name1}}: can you help me create a basic react module that demonstrates a counter
// Response: RESPOND`,

// {{name1}}: about a girl named {{characterName}}
// {{agentName}}: Sure.
// {{agentName}}: Once upon a time, in a quaint little village, there was a curious girl named {{characterName}}.
// {{agentName}}: {{characterName}} was known for her adventurous spirit and her knack for finding beauty in the mundane.
// {{name1}}: I'm loving it, keep going
// Response: RESPOND`,

// {{agentName}}: marco
// {{name1}}: great. okay, now do it again
// Response: RESPOND`,

// Response: RESPOND`,

  // Examples where agent should IGNORE
// {{name2}}: Oh? Which movie?
// Response: IGNORE`,

// {{agentName}}: how can I help you?
// {{name1}}: no. i need help from {{name2}}
// Response: IGNORE`,

// Response: IGNORE`,

// {{name1}}: sick
// {{name2}}: wait, why is it your favorite scene
// Response: RESPOND`,

  // Examples where agent should STOP
// Response: STOP`,

// Response: STOP`,

// Response: STOP`,

/**
 * Represents a provider that generates response examples for the agent.
 * @type {Provider}
 */
    // Get agent name

    // Create random user names and character name

    // Shuffle the message examples array

    // Replace placeholders with generated names

    // Join examples with newlines


} // namespace elizaos
