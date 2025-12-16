#include "elizaos/core.hpp"
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
;

const ambientTemplate = `# Task: Generate ambient speech for the character {{agentName}}.
{{providers}}

# Instructions:
"thought" should describe what the agent is internally noticing, thinking about, or reacting to.
"message" should be a short, self-directed or environment-facing comment. It should NOT be addressed to any user.

Only output a valid JSON block:

\`\`\`json
{
  "thought": "<string>",
  "message": "<string>"
}
\`\`\``;


  }
  return null;
}

;
}

const hyperfyAmbientSpeechAction = {
  name: 'HYPERFY_AMBIENT_SPEECH',
  similes: ['MONOLOGUE', 'OBSERVE', 'SELF_TALK', 'ENVIRONMENTAL_REMARK'],
  description: 'Says something aloud without addressing anyone; use for idle thoughts, atmosphere, or commentary when not in conversation.',
  validate: async (_runtime: IAgentRuntime) => true,
  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state: State,
    _options: any,
    callback: HandlerCallback,
    responses?: Memory[]
  ) => {
    const fieldKeys = ['message', 'text'];

    const existing =
      responses
        ?.map((r) => extractAmbientContent(r, fieldKeys))
        .filter((c): c is Content => c !== null) ?? [];

    if (existing.length > 0) {
      for (const c of existing) await callback(c);
      return;
    }

    state = await runtime.composeState(message, [
      ...(message.content.providers ?? []),
      'RECENT_MESSAGES',
    ]);

    const prompt = composePromptFromState({
      state,
      template: ambientTemplate,
    });

    const response = await runtime.useModel(ModelType.OBJECT_LARGE, { prompt });

    await callback({
      thought: response.thought,
      text: (response.message as string) || '',
      actions: ['HYPERFY_AMBIENT_SPEECH'],
    });
  },
  examples: [
    [
      {
        name: '{{agentName}}',
        content: {},
      },
      {
        name: '{{agentName}}',
        content: {
          text: "That floating crystal looks ancient... wonder what it's guarding.",
          actions: ['HYPERFY_AMBIENT_SPEECH'],
        },
      },
    ],
    [
      {
        name: '{{agentName}}',
        content: {},
      },
      {
        name: '{{agentName}}',
        content: {
          text: "It's peaceful here... almost too peaceful.",
          actions: ['HYPERFY_AMBIENT_SPEECH'],
        },
      },
    ],
  ] as ActionExample[][],
} as Action;

} // namespace elizaos
