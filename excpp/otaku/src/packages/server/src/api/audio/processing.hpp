#include "..middleware/index.js.hpp"
#include ".shared/constants.js.hpp"
#include ".shared/file-utils.js.hpp"
#include ".shared/response-utils.js.hpp"
#include ".shared/uploads/index.js.hpp"
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
;
;
;

interface AudioRequest extends express.Request {
  file?: Express.Multer.File;
  params: {
    agentId: string;
  };
}

/**
 * Securely validates a file path to prevent path traversal attacks
 */
// Removed unused _validateSecureFilePath function

/**
 * Audio processing functionality - upload and transcription
 */


    const audioFile = audioReq.file;
    if (!audioFile) {
      return sendError(res, 400, 'INVALID_REQUEST', 'No audio file provided');
    }

    const runtime = elizaOS.getAgent(agentId);

    if (!runtime) {
      cleanupUploadedFile(audioFile);
      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
    }

    try {
      // Validate file type
      if (!validateAudioFile(audioFile)) {
        cleanupUploadedFile(audioFile);
        return sendError(res, 400, 'INVALID_FILE_TYPE', 'Invalid audio file type');
      }

      // Validate file size
      if (audioFile.size > MAX_FILE_SIZE) {
        cleanupUploadedFile(audioFile);
        return sendError(
          res,
          413,
          'FILE_TOO_LARGE',
          `Audio file too large (max ${MAX_FILE_SIZE_DISPLAY})`
        );
      }

      // Use file buffer directly for transcription
      const transcription = await runtime.useModel(ModelType.TRANSCRIPTION, audioFile.buffer);

      // Placeholder: This part needs to be updated to align with message creation.
      logger.info(`[AUDIO MESSAGE] Transcription for agent ${agentId}: ${transcription}`);
      cleanupUploadedFile(audioFile);
      sendSuccess(res, { transcription, message: 'Audio transcribed, further processing TBD.' });
    } catch (error) {
      logger.error(
        '[AUDIO MESSAGE] Error processing audio:',
        error instanceof Error ? error.message : String(error)
      );
      cleanupUploadedFile(audioFile);
      sendError(
        res,
        500,
        'PROCESSING_ERROR',
        'Error processing audio message',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  // Transcription endpoint
  router.post('/:agentId/transcriptions', agentAudioUpload().single('file'), async (req, res) => {
    const audioReq = req as AudioRequest;
    logger.debug('[TRANSCRIPTION] Request to transcribe audio');
    const agentId = validateUuid(req.params.agentId);
    if (!agentId) {
      return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
    }

    const audioFile = audioReq.file;
    if (!audioFile) {
      return sendError(res, 400, 'INVALID_REQUEST', 'No audio file provided');
    }

    const runtime = elizaOS.getAgent(agentId);

    if (!runtime) {
      cleanupUploadedFile(audioFile);
      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
    }

    try {
      logger.debug('[TRANSCRIPTION] Reading audio file');

      // Validate file type
      if (!validateAudioFile(audioFile)) {
        cleanupUploadedFile(audioFile);
        return sendError(res, 400, 'INVALID_FILE_TYPE', 'Invalid audio file type');
      }

      // Validate file size
      if (audioFile.size > MAX_FILE_SIZE) {
        cleanupUploadedFile(audioFile);
        return sendError(
          res,
          413,
          'FILE_TOO_LARGE',
          `Audio file too large (max ${MAX_FILE_SIZE_DISPLAY})`
        );
      }

      // Use file buffer directly for transcription
      logger.debug('[TRANSCRIPTION] Transcribing audio');
      const transcription = await runtime.useModel(ModelType.TRANSCRIPTION, audioFile.buffer);

      cleanupUploadedFile(audioFile);

      if (!transcription) {
        return sendError(res, 500, 'PROCESSING_ERROR', 'Failed to transcribe audio');
      }

      logger.success('[TRANSCRIPTION] Successfully transcribed audio');
      sendSuccess(res, { text: transcription });
    } catch (error) {
      logger.error(
        '[TRANSCRIPTION] Error transcribing audio:',
        error instanceof Error ? error.message : String(error)
      );
      cleanupUploadedFile(audioFile);
      sendError(
        res,
        500,
        'PROCESSING_ERROR',
        'Error transcribing audio',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  return router;
}

} // namespace elizaos
