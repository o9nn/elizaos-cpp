#include ".shared/constants.hpp"
#include ".shared/response-utils.hpp"
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
;
;
;
;

// Configure multer for file uploads
const storage = multer.memoryStorage();
const upload = multer({
  storage,
  limits: {
    fileSize: MAX_FILE_SIZE,
    files: 1,
  },
  fileFilter: (_req, file, cb) => {
    if (ALLOWED_MEDIA_MIME_TYPES.includes(file.mimetype as any)) {
      cb(null, true);
    } else {
      cb(new Error('Invalid file type'));
    }
  },
});

// Helper > saveUploadedFile(Express.Multer.File file, const std::string& agentId););
  }

  // Generate unique filename
  const timestamp = Date.now();
  const random = Math.round(Math.random() * 1e9);
  const ext = path.extname(file.originalname);
  const filename = `${timestamp}-${random}${ext}`;
  const filePath = path.join(uploadDir, filename);

  // Write file to disk
  fs.writeFileSync(filePath, file.buffer);

  const url = `/media/uploads/agents/${agentId}/${filename}`;
  return { filename, url };
}

/**
 * Agent media upload functionality
 */


    if (!req.file) {
      return sendError(res, 400, 'INVALID_REQUEST', 'No media file provided');
    }

    const mediaType = getContentTypeFromMimeType(req.file.mimetype);
    if (!mediaType) {
      return sendError(
        res,
        400,
        'UNSUPPORTED_MEDIA_TYPE',
        `Unsupported media MIME type: ${req.file.mimetype}`
      );
    }

    try {
      // Save the uploaded file
      const result = await saveUploadedFile(req.file, agentId);

      logger.info(
        `[MEDIA UPLOAD] Successfully uploaded ${mediaType}: ${result.filename}. URL: ${result.url}`
      );

      sendSuccess(res, {
        url: result.url,
        type: mediaType,
        filename: result.filename,
        originalName: req.file.originalname,
        size: req.file.size,
      });
    } catch (error) {
      logger.error(`[MEDIA UPLOAD] Error processing upload: ${error}`);
      sendError(
        res,
        500,
        'UPLOAD_ERROR',
        'Failed to process media upload',
        error instanceof Error ? error.message : String(error)
      );
    }
  });

  return router;
}

} // namespace elizaos
