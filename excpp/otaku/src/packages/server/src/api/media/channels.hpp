#include ".shared/constants.hpp"
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
  fileFilter: (_req: express.Request, file: Express.Multer.File, cb: multer.FileFilterCallback) => {
    // Check if mimetype is in the allowed list
    const isAllowed = ALLOWED_MEDIA_MIME_TYPES.some((allowed) => allowed === file.mimetype);
    if (isAllowed) {
      cb(null, true);
    } else {
      cb(new Error(`Invalid file type. Only ${ALLOWED_MEDIA_MIME_TYPES.join(', ')} are allowed`));
    }
  },
});

// Helper > saveUploadedFile(Express.Multer.File file, const std::string& channelId););
  }

  // Generate unique filename
  const timestamp = Date.now();
  const random = Math.round(Math.random() * 1e9);
  const ext = path.extname(file.originalname);
  const filename = `${timestamp}-${random}${ext}`;
  const filePath = path.join(uploadDir, filename);

  // Write file to disk
  fs.writeFileSync(filePath, file.buffer);

  const url = `/media/uploads/channels/${channelId}/${filename}`;
  return { filename, url };
}

/**
 * Channel media upload functionality
 */
,
  });

  // Upload media to channel
  router.post(
    '/:channelId/upload-media',
    uploadMediaRateLimiter, // Apply rate limiter
    upload.single('file'),
    async (req, res) => {
      const channelId = validateUuid(req.params.channelId);
      if (!channelId) {
        res.status(400).json({ success: false, error: 'Invalid channelId format' });
        return;
      }

      if (!req.file) {
        res.status(400).json({ success: false, error: 'No media file provided' });
        return;
      }

      try {
        // Save the uploaded file
        const result = await saveUploadedFile(req.file, channelId);

        logger.info(
          `[Channel Media Upload] File uploaded for channel ${channelId}: ${result.filename}. URL: ${result.url}`
        );

        res.json({
          success: true,
          data: {
            url: result.url, // Relative URL, client prepends server origin
            type: req.file.mimetype,
            filename: result.filename,
            originalName: req.file.originalname,
            size: req.file.size,
          },
        });
      } catch (error: any) {
        logger.error(
          `[Channel Media Upload] Error processing upload for channel ${channelId}: ${error.message}`,
          error
        );
        res.status(500).json({ success: false, error: 'Failed to process media upload' });
      }
    }
  );

  return router;
}

} // namespace elizaos
