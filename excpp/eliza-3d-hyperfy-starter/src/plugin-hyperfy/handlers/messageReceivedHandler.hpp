#include ".templates.hpp"
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
;

const latestResponseIds = new Map<string, Map<string, string>>();

/**
 * Handles incoming messages and generates responses based on the provided runtime and message information.
 *
 * @param {MessageReceivedHandlerParams} params - The parameters needed for message handling, including runtime, message, and callback.
 * @returns {Promise<void>} - A promise that resolves once the message handling and response generation is complete.
 */
const messageReceivedHandler = async ({
  runtime,
  message,
  callback,
  onComplete,
}: MessageReceivedHandlerParams): Promise<void> => {
  // Set up timeout monitoring
  const timeoutDuration = 60 * 60 * 1000; // 1 hour
  let timeoutId: NodeJS.Timeout | undefined = undefined;
  try {
    logger.info(`[Bootstrap] Message received from ${message.entityId} in room ${message.roomId}`);
    // Generate a new response ID
    const responseId = v4();
    // Get or create the agent-specific map
    if (!latestResponseIds.has(runtime.agentId)) {
      latestResponseIds.set(runtime.agentId, new Map<string, string>());
    }
    const agentResponses = latestResponseIds.get(runtime.agentId);
    if (!agentResponses) {
      throw new Error('Agent responses map not found');
    }

    // Set this as the latest response ID for this agent+room
    agentResponses.set(message.roomId, responseId);

    // Generate a unique run ID for tracking this message handler execution
    const runId = asUUID(v4());
    const startTime = Date.now();

    // Emit run started event
    await runtime.emitEvent(EventType.RUN_STARTED, {
      runtime,
      runId,
      messageId: message.id,
      roomId: message.roomId,
      entityId: message.entityId,
      startTime,
      status: 'started',
      source: 'messageHandler',
    });

    const timeoutPromise = new Promise<never>((_, reject) => {
      timeoutId = setTimeout(async () => {
        await runtime.emitEvent(EventType.RUN_TIMEOUT, {
          runtime,
          runId,
          messageId: message.id,
          roomId: message.roomId,
          entityId: message.entityId,
          startTime,
          status: 'timeout',
          endTime: Date.now(),
          duration: Date.now() - startTime,
          error: 'Run exceeded 60 minute timeout',
          source: 'messageHandler',
        });
        reject(new Error('Run exceeded 60 minute timeout'));
      }, timeoutDuration);
    });

    const processingPromise = (async () => {
      try {
        if (message.entityId === runtime.agentId) {
          logger.debug(`[Bootstrap] Skipping message from self (${runtime.agentId})`);
          throw new Error('Message is from the agent itself');
        }

        logger.debug(
          `[Bootstrap] Processing message: ${truncateToCompleteSentence(message.content.text || '', 50)}...`
        );

        // First, save the incoming message
        logger.debug('[Bootstrap] Saving message to memory and embeddings');
        await Promise.all([
          runtime.addEmbeddingToMemory(message),
          runtime.createMemory(message, 'messages'),
        ]);

        const agentUserState = await runtime.getParticipantUserState(
          message.roomId,
          runtime.agentId
        );

        if (
          agentUserState === 'MUTED' &&
          !message.content.text?.toLowerCase().includes(runtime.character.name.toLowerCase())
        ) {
          logger.debug(`[Bootstrap] Ignoring muted room ${message.roomId}`);
          return;
        }

        let state = await runtime.composeState(
          message,
        );

        // Skip shouldRespond check for DM and VOICE_DM channels
        const room = await runtime.getRoom(message.roomId);
        logger.debug(
          `[Bootstrap] Skipping shouldRespond check for ${runtime.character.name} because ${room?.type} ${room?.source}`
        );

        // Handle shouldRespond
        const shouldRespondPrompt = composePromptFromState({
          state,
          template: hyperfyShouldRespondTemplate,
        });

        logger.debug(
          `[Bootstrap] Evaluating response for ${runtime.character.name}\nPrompt: ${shouldRespondPrompt}`
        );

        const response = await runtime.useModel(ModelType.TEXT_SMALL, {
          prompt: shouldRespondPrompt,
        });

        logger.debug(
          `[Bootstrap] Response evaluation for ${runtime.character.name}:\n${response}`
        );
        logger.debug(`[Bootstrap] Response type: ${typeof response}`);

        // Try to preprocess response by removing code blocks markers if present
        // let processedResponse = response.replace('```json', '').replaceAll('```', '').trim(); // No longer needed for XML

        const responseObject = parseKeyValueXml(response);
        console.log('[Bootstrap] Parsed response:', responseObject);

        const shouldRespond = responseObject?.action && responseObject.action === 'RESPOND';
        
        let responseMessages: Memory[] = [];

        if (shouldRespond) {
          state = await runtime.composeState(message);

          const prompt = composePromptFromState({
            state,
            template: hyperfyMessageHandlerTemplate,
          });

          let responseContent: Content | null = null;

          // Retry if missing required fields
          let retries = 0;
          const maxRetries = 3;

          while (retries < maxRetries && (!responseContent?.thought || !responseContent?.actions)) {
            let response = await runtime.useModel(ModelType.TEXT_LARGE, {
              prompt,
            });

            logger.debug('[Bootstrap] *** Raw LLM Response ***\n', response);

            // Attempt to parse the XML response
            const parsedXml = parseKeyValueXml(response);
            logger.debug('[Bootstrap] *** Parsed XML Content ***\n', parsedXml);

            // Map parsed XML to Content type, handling potential missing fields
            if (parsedXml) {
              let actions = parsedXml.actions || ['IGNORE'];

              // Reorder actions so 'REPLY' always comes first if present
              const replyIndex = actions.findIndex(a => a.toUpperCase() === 'REPLY');
              if (replyIndex !== -1) {
                actions = ['REPLY', ...actions.filter((a, i) => i !== replyIndex)];
              }

              // Deduplicate actions
              actions = Array.from(new Set(actions.map(a => a.toUpperCase())));

              responseContent = {
                ...parsedXml,
                thought: parsedXml.thought || '',
                actions,
                providers: parsedXml.providers || [],
                text: parsedXml.text || '',
                simple: parsedXml.simple || false,
              };
            } else {
              responseContent = null;
            }

            retries++;
            if (!responseContent?.thought || !responseContent?.actions) {
              logger.warn(
                '[Bootstrap] *** Missing required fields (thought or actions), retrying... ***'
              );
            }
          }

          // Check if this is still the latest response ID for this agent+room
          const currentResponseId = agentResponses.get(message.roomId);
          if (currentResponseId !== responseId) {
            logger.info(
              `Response discarded - newer message being processed for agent: ${runtime.agentId}, room: ${message.roomId}`
            );
            return;
          }

          if (responseContent && message.id) {
            responseContent.inReplyTo = createUniqueUuid(runtime, message.id);

            const responseMesssage = {
              id: asUUID(v4()),
              entityId: runtime.agentId,
              agentId: runtime.agentId,
              content: responseContent,
              roomId: message.roomId,
              createdAt: Date.now(),
            };

            responseMessages = [responseMesssage];
          }

          // Clean up the response ID
          agentResponses.delete(message.roomId);
          if (agentResponses.size === 0) {
            latestResponseIds.delete(runtime.agentId);
          }

          if (responseContent?.providers?.length && responseContent?.providers?.length > 0) {
            state = await runtime.composeState(message, responseContent?.providers || []);
          }

          await runtime.processActions(message, responseMessages, state, callback);
          
          await runtime.evaluate(message, state, shouldRespond, callback, responseMessages);
        } else {
          // Handle the case where the agent decided not to respond
          logger.debug('[Bootstrap] Agent decided not to respond (shouldRespond is false).');

          // Check if we still have the latest response ID
          const currentResponseId = agentResponses.get(message.roomId);
          if (currentResponseId !== responseId) {
            logger.info(
              `Ignore response discarded - newer message being processed for agent: ${runtime.agentId}, room: ${message.roomId}`
            );
            return; // Stop processing if a newer message took over
          }

          if (!message.id) {
            logger.error('[Bootstrap] Message ID is missing, cannot create ignore response.');
            return;
          }

          // Construct a minimal content object indicating ignore, include a generic thought
          const ignoreContent: Content = {
            thought: 'Agent decided not to respond to this message.',
            actions: ['IGNORE'],
            simple: true, // Treat it as simple for callback purposes
            inReplyTo: createUniqueUuid(runtime, message.id), // Reference original message
          };

          // Call the callback directly with the ignore content
          await callback(ignoreContent);

          // Also save this ignore action/thought to memory
          const ignoreMemory = {
            id: asUUID(v4()),
            entityId: runtime.agentId,
            agentId: runtime.agentId,
            content: ignoreContent,
            roomId: message.roomId,
            createdAt: Date.now(),
          };
          await runtime.createMemory(ignoreMemory, 'messages');
          logger.debug('[Bootstrap] Saved ignore response to memory', {
            memoryId: ignoreMemory.id,
          });

          // Clean up the response ID since we handled it
          agentResponses.delete(message.roomId);
          if (agentResponses.size === 0) {
            latestResponseIds.delete(runtime.agentId);
          }

          // Optionally, evaluate the decision to ignore (if relevant evaluators exist)
          // await runtime.evaluate(message, state, shouldRespond, callback, []);
        }

        // Emit run ended event on successful completion
        await runtime.emitEvent(EventType.RUN_ENDED, {
          runtime,
          runId,
          messageId: message.id,
          roomId: message.roomId,
          entityId: message.entityId,
          startTime,
          status: 'completed',
          endTime: Date.now(),
          duration: Date.now() - startTime,
          source: 'messageHandler',
        });
      } catch (error: any) {
        // Emit run ended event with error
        await runtime.emitEvent(EventType.RUN_ENDED, {
          runtime,
          runId,
          messageId: message.id,
          roomId: message.roomId,
          entityId: message.entityId,
          startTime,
          status: 'error',
          endTime: Date.now(),
          duration: Date.now() - startTime,
          error: error.message,
          source: 'messageHandler',
        });
      }
    })();

    await Promise.race([processingPromise, timeoutPromise]);
  } finally {
    clearTimeout(timeoutId);
    onComplete?.();
  }
};
} // namespace elizaos
