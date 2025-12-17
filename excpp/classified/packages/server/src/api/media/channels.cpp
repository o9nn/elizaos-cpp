#include "channels.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> saveUploadedFile(Express.Multer.File file, const std::string& channelId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    filename: string; url: string
}

express::Router createChannelMediaRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Define rate limiter: maximum 100 requests per 15 minutes
    const auto uploadMediaRateLimiter = rateLimit({;
        windowMs: 15 * 60 * 1000, // 15 minutes
        max: 100, // Limit each IP to 100 requests per windowMs
        message: { success: false, error: 'Too many requests, please try again later.' },
        });

        // Upload media to channel
        router.post(;
        '/:channelId/upload-media',
        uploadMediaRateLimiter, // Apply rate limiter;
        upload.single('file'),
        async (req, res) => {
            const auto channelId = validateUuid(req.params.channelId);
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
                const auto result = saveUploadedFile(req.file, channelId);

                logger.info(
                "[Channel Media Upload] File uploaded for channel " + std::to_string(channelId) + ": " + std::to_string(result.filename) + ". URL: " + std::to_string(result.url)
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
                            "[Channel Media Upload] Error processing upload for channel " + std::to_string(channelId) + ": " + std::to_string(error.message)
                            error;
                            );
                            res.status(500).json({ success: false, error: 'Failed to process media upload' });
                        }
                    }
                    );

                    return router;

}

} // namespace elizaos
