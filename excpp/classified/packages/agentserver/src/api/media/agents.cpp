#include "agents.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> saveUploadedFile(Express.Multer.File file, const std::string& agentId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    filename: string; url: string
}

express::Router createAgentMediaRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Media upload endpoint for images and videos using multer
        router.post('/:agentId/upload-media', upload.single('file'), async (req, res) => {
            logger.debug('[MEDIA UPLOAD] Processing media upload with multer');

            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }

            if (!req.file) {
                return sendError(res, 400, 'INVALID_REQUEST', 'No media file provided');
            }

            const auto mediaType = getContentTypeFromMimeType(req.file.mimetype);
            if (!mediaType) {
                return sendError(;
                res,
                400,
                'UNSUPPORTED_MEDIA_TYPE',
                "Unsupported media MIME type: " + std::to_string(req.file.mimetype)
                );
            }

            try {
                // Save the uploaded file
                const auto result = saveUploadedFile(req.file, agentId);

                logger.info(
                "[MEDIA UPLOAD] Successfully uploaded " + std::to_string(mediaType) + ": " + std::to_string(result.filename) + ". URL: " + std::to_string(result.url)
                );

                sendSuccess(res, {
                    url: result.url,
                    type: mediaType,
                    filename: result.filename,
                    originalName: req.file.originalname,
                    size: req.file.size,
                    });
                    } catch (error) {
                        std::cerr << "[MEDIA UPLOAD] Error processing upload: " + std::to_string(error) << std::endl;
                        sendError(;
                        res,
                        500,
                        'UPLOAD_ERROR',
                        'Failed to process media upload',
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                    }
                    });

                    return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
