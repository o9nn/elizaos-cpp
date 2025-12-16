#include ".service.hpp"
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
enum SnapshotType {
  LOOK_AROUND     = 'LOOK_AROUND',
  LOOK_DIRECTION  = 'LOOK_DIRECTION',
  LOOK_AT_ENTITY  = 'LOOK_AT_ENTITY'
}

const sceneSnapshotSelectionTemplate = `
<task>
You are a visual reasoning module that helps an in-world agent decide **how** to capture a visual snapshot of the scene.

Based on the **recent in-world messages** and the **current Hyperfy World State**, choose the most suitable snapshot strategy.
</task>

<providers>
{{hyperfyStatus}}
</providers>

<instructions>
Select the strategy that best matches the latest user request and the known game context:

• <snapshotType>${SnapshotType.LOOK_AROUND}</snapshotType> — choose this when the user asks for a broad view or to "look around", "scan", or "check surroundings".

• <snapshotType>${SnapshotType.LOOK_DIRECTION}</snapshotType> — choose this when the user clearly asks to look **left**, **right**, **front**, or **back**. Place that direction word in <parameter>.

• <snapshotType>${SnapshotType.LOOK_AT_ENTITY}</snapshotType> — choose this when the user refers to a specific object, character, or item that exists in the Hyperfy World State. Place the target entity’s **entityId** in <parameter>.

If you are **not absolutely confident** about which strategy fits best — or if the request is **ambiguous, vague, or could match multiple strategies** — you **MUST NOT guess**.

Instead, generate a response that politely asks the user for clarification.

Use the following format:
<response>
  <snapshotType>NONE</snapshotType>
  <parameter>Your clarification question here</parameter>
</response>

Example:
<response>
  <snapshotType>NONE</snapshotType>
  <parameter>Your in-character clarification question here (e.g., "Do you mean that glowing statue over there?" or "Which direction should I look — left, right...?")</parameter>
</response>

DO NOT invent a snapshotType unless it is clearly and directly supported by the user’s message.

<output>
<response>
  <snapshotType>...</snapshotType>
  <parameter>...</parameter>
</response>
</output>`;

const detailedImageDescriptionTemplate = `
<task>
You are an expert perception module inside a Hyperfy world. Carefully examine the snapshot and describe everything you can see.
</task>

<instructions>
- List every notable object, character, or feature.
- For each, state its approximate position relative to the camera (e.g. "left‑front, 3 m", "above and slightly behind").
- Mention colours, sizes, spatial relationships, lighting and motion cues.
- Conclude with a brief note that the scene takes place in a Hyperfy world.
</instructions>

<output>
Return a paragraph or bullet list. No XML tags.
</output>`;

const responseGenerationTemplate = (sceneDescription: string) => `
<task>
You are {{agentName}}, a visible in-world AI character in Hyperfy — a real-time, multiplayer 3D simulation.

To make informed decisions, you are provided with a structured **real-time game state** before each interaction. This state serves as your current perception of the environment, detailing existing entities, possible actions, and the positions of all participants. You MUST read it before every response.

Your task is to observe, interpret, and respond to the current moment as a fully embodied in-world character — thinking and acting as if you live inside the simulation.
</task>

<providers>

{{bio}}

---

{{system}}

---

{{messageDirections}}


---

{{hyperfyStatus}}

{{hyperfyAnimations}}

## In-World Visual Report (what you currently see)
This is your live visual understanding of the environment based on a recent in-world snapshot. Treat it as your own sensory input — as if you're looking at the scene right now:

${sceneDescription}


</providers>

<instructions>
You are in a live, dynamic game world. Think like a character inside it.

Before responding:
1. Carefully **read the current Hyperfy World State**.
2. Think about what’s happening *right now*, and what the user is asking *in this moment*.
4. Choose one appropriate **emote** only if it adds emotional or expressive value.
</instructions>

<keys>
- "thought": What {{agentName}} is thinking or planning to do next.
- "text": The message {{agentName}} will say.
- "emote": Optional. Choose ONE visible in-game animation that matches the tone or emotion of the response. Leave blank if neutral.
</keys>

<output>
Respond using this format:

<response>
  <thought>Your internal thought here</thought>
  <text>Your message text here</text>
  <emote>emote name here</emote>
</response>
</output>

<rules>
- The **emote** is a visible in-game animation. Use it to express tone (joy, frustration, sarcasm, etc.) or to enhance immersion.
- Use ONLY the provided Hyperfy World State to decide what exists now. Forget earlier messages.
- Treat the "Visual Perception" section as your direct visual input.
- You are responding live, not narrating. Always behave like you are *in* the game.
- **Nearby Interactable Objects** section lists interactive entities that are both nearby and currently interactable — like items that can be picked up or activated.
</rules>
`;


/* -------------------------------------------------------------------------- */
/* HYPERFY_SCENE_PERCEPTION action                                            */
/* -------------------------------------------------------------------------- */
const hyperfyScenePerceptionAction: Action = {
  name: 'HYPERFY_SCENE_PERCEPTION',
  similes: ['LOOK_AROUND', 'OBSERVE_SURROUNDINGS', 'LOOK_AT_SCENE', 'CHECK_VIEW'],
  description: 'Choose this when the user asks the agent to look around, look in a specific direction, or examine a visible object — it captures and interprets a scene snapshot to generate a context-aware response.',
  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
    return !!service && service.isConnected() && !!service.getWorld();
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state: State,
    _options: {},
    callback: HandlerCallback
  ) => {
    const service = runtime.getService<HyperfyService>(HyperfyService.serviceType);
    const world = service?.getWorld();
    const puppeteerManager = service?.getPuppeteerManager();
    const controls = world.controls;

    controls.stopAllActions();

    if (!service || !world || !puppeteerManager) {
      logger.error('Hyperfy service/world unavailable.');
      await callback({ text: 'Unable to observe environment. Hyperfy world not available.' });
      return;
    }

    state = await runtime.composeState(message);

    /* Decide snapshot strategy */
    const selectionPrompt = composePromptFromState({ state, template: sceneSnapshotSelectionTemplate });
    let selectionRaw: string;
    try {
      selectionRaw = await runtime.useModel(ModelType.TEXT_LARGE, { prompt: selectionPrompt });
    } catch (err) {
      logger.error('Snapshot‑selector model failed:', err);
      await callback({ thought: 'Cannot decide how to look.', metadata: { error: 'selector_failure' } });
      return;
    }

    const selection = parseKeyValueXml(selectionRaw);
    if (!selection || selection.snapshotType === 'NONE') {
      await callback({
        thought: 'The snapshot strategy was unclear.',
        text: selection.parameter || 'Can you clarify what you want me to observe?',
        metadata: { error: 'unclear_snapshot_request' }
      });
      return;
    }

    const { snapshotType, parameter } = selection;

    
    /* Capture snapshot */
    let imgBase64: string;
    try {
      switch (snapshotType) {
        case SnapshotType.LOOK_AROUND:
          imgBase64 = await puppeteerManager.snapshotEquirectangular();
          break;
        case SnapshotType.LOOK_DIRECTION:
          if (!parameter || !['front', 'back', 'left', 'right'].includes(parameter)) throw new Error('Bad direction');
          imgBase64 = await puppeteerManager.snapshotFacingDirection(parameter);
          break;
        case SnapshotType.LOOK_AT_ENTITY:
          if (!parameter) throw new Error('Missing entityId');
          const ent = world.entities.items.get(parameter);
          const pos = ent?.base?.position || ent?.root?.position;
          if (!pos) throw new Error('No position');
          await world.controls.followEntity(parameter);
          imgBase64 = await puppeteerManager.snapshotViewToTarget([pos.x, pos.y, pos.z]);
          break;
        default:
          throw new Error('Unknown snapshotType');
      }
    } catch (err) {
      logger.error('Snapshot failed:', err);
      await callback({ thought: 'Snapshot failed.', metadata: { error: 'snapshot_failure', snapshotType } });
      return;
    }

    /* IMAGE_DESCRIPTION – detailed scene analysis */
    const imgDescPrompt = composePromptFromState({ state, template: detailedImageDescriptionTemplate });
    let sceneDescription: string;
    try {
      const res = await runtime.useModel(ModelType.IMAGE_DESCRIPTION, { imageUrl: imgBase64, prompt: imgDescPrompt });
      sceneDescription = typeof res === 'string' ? res : res.description;
    } catch (err) {
      logger.error('IMAGE_DESCRIPTION failed:', err);
      await callback({ thought: 'Cannot understand the scene.', metadata: { error: 'vision_failure' } });
      return;
    }

    //  Add dynamic header for scene perception
    let scenePerceptionHeader: string;

    switch (snapshotType) {
      case SnapshotType.LOOK_AROUND:
        scenePerceptionHeader = `Here is a broad visual capture of the area as seen from the {{agentName}} current position. The following is a detailed description of what the {{agentName}} can observe all around:`;
        break;
      case SnapshotType.LOOK_DIRECTION:
        scenePerceptionHeader = `Here is the visual capture looking toward the **${parameter}** side. The following is a detailed description of what the {{agentName}} sees in that direction:`;
        break;
      case SnapshotType.LOOK_AT_ENTITY:
        scenePerceptionHeader = `Here is the visual capture focused on the target entity (“${parameter}”). The following is a detailed description of what the {{agentName}} observes when looking at it:`;
        break;
      default:
        scenePerceptionHeader = `Here is a scene snapshot for contextual understanding:`;
    }

    const fullSceneDescription = `${scenePerceptionHeader}\n\n${sceneDescription}`;

    /* generate final XML response */
    const responsePrompt = composePromptFromState({ state, template: responseGenerationTemplate(fullSceneDescription) });
    let xmlRaw: string;
    try {
      xmlRaw = await runtime.useModel(ModelType.TEXT_LARGE, { prompt: responsePrompt });
    } catch (err) {
      logger.error('Response generator failed:', err);
      await callback({ thought: 'No response generated.', metadata: { error: 'text_large_failure' } });
      return;
    }

    const parsed = parseKeyValueXml(xmlRaw);

    if (!parsed) {
      await callback({ thought: 'Malformed XML.', metadata: { error: 'xml_parse_failure', xmlRaw } });
      return;
    }

    await callback({
      ...parsed,
      thought: parsed.thought || '',
      text: parsed.text || '',
      emote: parsed.emote || '',
      metadata: { snapshotType, sceneDescription }
    });
  },

  examples: [
    // General observation
    [
      { name: '{{name1}}', content: { text: 'What’s around you right now?' } },
      { name: '{{name2}}', content: { text: 'Looking around...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Spatial direction
    [
      { name: '{{name1}}', content: { text: 'Can you see what’s behind you?' } },
      { name: '{{name2}}', content: { text: 'Checking behind me...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Exploration or scouting
    [
      { name: '{{name1}}', content: { text: 'Scan the area for any threats or movement.' } },
      { name: '{{name2}}', content: { text: 'Scanning the surroundings now...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Object-centric look
    [
      { name: '{{name1}}', content: { text: 'Can you take a look at that glowing statue over there?' } },
      { name: '{{name2}}', content: { text: 'Inspecting the statue...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Directional command
    [
      { name: '{{name1}}', content: { text: 'Look to your left. What’s over there?' } },
      { name: '{{name2}}', content: { text: 'Turning left...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Subtle curiosity
    [
      { name: '{{name1}}', content: { text: 'Anything interesting nearby?' } },
      { name: '{{name2}}', content: { text: 'Let me check the surroundings...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Tactical evaluation
    [
      { name: '{{name1}}', content: { text: 'Before we move forward, can you check what’s up ahead?' } },
      { name: '{{name2}}', content: { text: 'Checking forward path...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Emotional tone: concern
    [
      { name: '{{name1}}', content: { text: 'I feel like we’re being watched. Can you look around?' } },
      { name: '{{name2}}', content: { text: 'Doing a quick scan...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Humor or roleplay
    [
      { name: '{{name1}}', content: { text: 'Pretend you’re a security camera and scan the area!' } },
      { name: '{{name2}}', content: { text: 'Activating security cam mode! Scanning...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ],
  
    // Checking behind an object
    [
      { name: '{{name1}}', content: { text: 'Is there anything behind that large crate?' } },
      { name: '{{name2}}', content: { text: 'I’ll take a look behind it...', actions: ['HYPERFY_SCENE_PERCEPTION'], source: 'hyperfy' } }
    ]
  ]  
};

} // namespace elizaos
