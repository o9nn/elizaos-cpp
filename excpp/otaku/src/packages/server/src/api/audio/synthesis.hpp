#include ".shared/response-utils.hpp"
#include "audioBuffer.hpp"
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

import type { ElizaOS } from '@elizaos/core';
;
;
;
;

/**
 * Text-to-speech synthesis functionality
 */


    const { text } = req.body;
    if (!text) {
      return sendError(res, 400, 'INVALID_REQUEST', 'Text is required for speech synthesis');
    }

    const runtime = elizaOS.getAgent(agentId);

    if (!runtime) {
      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
    }

    try {
      const speechResponse = await runtime.useModel(ModelType.TEXT_TO_SPEECH, text);
      const audioResult = await convertToAudioBuffer(speechResponse, true);

      logger.debug('[TTS] Setting response headers');
      res.set({
        'Content-Type': audioResult.mimeType,
        'Content-Length': audioResult.buffer.length.toString(),
      });

      res.send(audioResult.buffer);
    } catch (error) {
      logger.error(
        '[TTS] Error generating speech:',
        error instanceof Error ? error.message : String(error)
      );
      sendError(
        res,
        500,
        'PROCESSING_ERROR',
        'Error generating speech',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  // Speech generation endpoint
  router.post('/:agentId/speech/generate', async (req, res) => {
    logger.debug('[SPEECH GENERATE] Request to generate speech from text');
    const agentId = validateUuid(req.params.agentId);
    if (!agentId) {
      return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
    }

    const { text } = req.body;
    if (!text) {
      return sendError(res, 400, 'INVALID_REQUEST', 'Text is required for speech synthesis');
    }

    const runtime = elizaOS.getAgent(agentId);

    if (!runtime) {
      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
    }

    try {
      logger.debug('[SPEECH GENERATE] Using text-to-speech model');
      const speechResponse = await runtime.useModel(ModelType.TEXT_TO_SPEECH, text);
      const audioResult = await convertToAudioBuffer(speechResponse, true);
      logger.debug('[SPEECH GENERATE] Detected audio MIME type:', audioResult.mimeType);

      logger.debug('[SPEECH GENERATE] Setting response headers');
      res.set({
        'Content-Type': audioResult.mimeType,
        'Content-Length': audioResult.buffer.length.toString(),
      });

      res.send(audioResult.buffer);
      logger.success(
        `[SPEECH GENERATE] Successfully generated speech for: ${runtime.character.name}`
      );
    } catch (error) {
      logger.error(
        '[SPEECH GENERATE] Error generating speech:',
        error instanceof Error ? error.message : String(error)
      );
      sendError(
        res,
        500,
        'PROCESSING_ERROR',
        'Error generating speech',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  return router;
}

} // namespace elizaos
