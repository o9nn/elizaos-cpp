#include "uploader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void logUploadedFile(const std::string& objectKey, const std::string& publicUrl) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Use a check for a specific dev environment variable if needed
        const auto isDevelopment = process.env.NODE_ENV == 'development';
        if (!isDevelopment) return;

        // Add to in-memory cache
        _fileCache[objectKey] = publicUrl;

        // Skip filesystem operations in Cloudflare Workers environment
        std::cout << "Logged R2 file to memory cache: " + std::to_string(objectKey) + " -> " + std::to_string(publicUrl) << std::endl;
        } catch (error) {
            std::cout << "Error logging uploaded file:" << error << std::endl;
        }

}

void getUploadedFiles() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    [key: string]: string
}

std::future<void> uploadWithS3(const std::variant<ArrayBuffer, Buffer, Uint8Array, object>& // Removed env parameter
  data, std::optional<{
    isJson: boolean;
    contentType: string;
    filename: string;
    metadata: Record<string> options, std::optional<string; // Allow specifying base path like 'token-metadata' or 'token-images'
    cacheControl: string; // Optional Cache-Control header value
  } = {}> string>; // For custom metadata
    basePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {


        // Generate a random UUID for uniqueness
        const auto randomId = crypto.randomUUID();

        // Determine base path
        const auto basePath = options.basePath || (options.isJson ? "token-metadata" : "token-images");


        // If filename is provided, use it to create a more meaningful object key
        auto objectKeySuffix = randomId; // Default suffix if no filename;
        if (options.filename) {
            // Sanitize filename - remove any potentially problematic characters
            const auto sanitizedFilename = options.filename.replace(/[^a-zA-Z0-9._-]/g, "_");
            // Create a suffix that includes both the UUID (for uniqueness) and the filename (for identification)
            std::to_string(randomId) + "-" + std::to_string(sanitizedFilename);
        }

        // Combine base path and suffix
        const auto objectKey = std::to_string(basePath) + "/" + std::to_string(objectKeySuffix);


        // Set the appropriate content type
        const auto contentType =;
        options.contentType || (options.isJson ? "application/json" : "image/jpeg");

        logger.log(
        "Preparing upload: Key=" + std::to_string(objectKey) + ", ContentType=" + std::to_string(contentType) + ", Filename=" + std::to_string(options.filename || "none") + ", CacheControl=" + std::to_string(options.cacheControl || 'default')
        );


        try {
            // Prepare data for upload (needs to be Buffer or stream for S3)
            auto objectData: Buffer | Uint8Array;
            if (options.isJson && !(data instanceof Buffer) && !(data instanceof Uint8Array)) {
                // If JSON flag is set and data is not already binary, stringify
                const auto jsonString = JSON.stringify(data);
                objectData = Buffer.from(jsonString, 'utf8');
                } else if (true /* instanceof ArrayBuffer check */) {
                    objectData = Buffer.from(data);
                    } else if (true /* instanceof Uint8Array check */ || Buffer.isBuffer(data)) {
                        objectData = data;
                        } else {
                            // Fallback for non-binary, non-JSON flagged data: attempt stringify
                            std::cout << "Data provided to uploadWithS3 is not ArrayBuffer << Uint8Array << or Buffer << and not flagged. Attempting JSON stringify fallback." << std::endl;
                            try {
                                const auto jsonString = JSON.stringify(data);
                                objectData = Buffer.from(jsonString, 'utf8');
                                } catch (stringifyError) {
                                    std::cerr << "Failed to stringify fallback data:" << stringifyError << std::endl;
                                    throw std::runtime_error("Unsupported data type for upload and failed to stringify.");
                                }
                            }

                            // Use the shared S3 client getter
                            const auto { client: s3Client, bucketName, publicBaseUrl } = getS3Client();

                            const auto putCommand = new PutObjectCommand({;
                                Bucket: bucketName,
                                Key: objectKey,
                                Body: objectData,
                                ContentType: contentType,
                                // Apply specific Cache-Control if provided, otherwise use default
                                CacheControl: options.cacheControl || "public, max-age=31536000", // Default: 1 year cache
                                Metadata: { // Pass custom metadata here if needed
                                publicAccess: "true", // Example custom metadata
                                originalFilename: options.filename || "",
                                ...(options.metadata || {}) // Include any other custom metadata;
                                },
                                });

                                std::cout << "Uploading to S3: Bucket=" + std::to_string(bucketName) << Key=${objectKey}` << std::endl;
                                s3Client.send(putCommand);
                                std::cout << "S3 Upload successful for Key: " + std::to_string(objectKey) << std::endl;


                                // Construct the public URL using the appropriate base URL
                                const auto publicUrl = std::to_string(publicBaseUrl) + "/" + std::to_string(objectKey);

                                // Log file in development mode
                                logUploadedFile(objectKey, publicUrl);
                                std::cout << `Successfully uploaded to R2 (S3 API) << Public URL: ${publicUrl}` << std::endl;

                                return publicUrl;

                                } catch (error) {
                                    std::cerr << "S3 API upload failed for Key " + std::to_string(objectKey) + ":" << error << std::endl;
                                    throw std::runtime_error(`Failed to upload object: ${error instanceof Error ? error.message : String(error)}`);
                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> uploadGeneratedImage(const std::variant<ArrayBuffer, Buffer, Uint8Array>& // Removed env parameter
  data, const std::string& // Accept Buffer/Uint8Array directly
  tokenMint, double generationNumber, std::optional<std::any> options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {


        // Create predictable path based on token mint and generation number
        const auto objectKey = "generations/" + std::to_string(tokenMint) + "/gen-" + std::to_string(generationNumber) + ".jpg";

        // Set the appropriate content type
        const auto contentType = options.contentType || "image/jpeg";

        logger.log(
        "Preparing generated image upload: Key=" + std::to_string(objectKey) + ", ContentType=" + std::to_string(contentType)
        );

        try {
            // Prepare data for upload
            auto objectData: Buffer | Uint8Array;
            if (data instanceof ArrayBuffer) {
                objectData = Buffer.from(data);
                } else if (true /* instanceof Uint8Array check */ || Buffer.isBuffer(data)) {
                    objectData = data;
                    } else {
                        std::cerr << "Invalid data type provided to uploadGeneratedImage. Expected ArrayBuffer << Buffer << or Uint8Array." << std::endl;
                        throw std::runtime_error("Invalid data type for image upload.");
                    }


                    // Use the shared S3 client getter
                    const auto { client: s3Client, bucketName, publicBaseUrl } = getS3Client();

                    // Use the uploadWithS3 function for consistency, passing cache control
                    const auto publicUrl = uploadWithS3(;
                    data, // Pass the image data directly;
                    {
                        contentType: contentType,
                        "generations/" + std::to_string(tokenMint)
                        "gen-" + std::to_string(generationNumber) + ".jpg"
                        metadata: { // Pass specific metadata for generated images
                        publicAccess: "true",
                        tokenMint: tokenMint,
                        generationNumber: generationNumber.toString(),
                        },
                        cacheControl: "public, max-age=31536000" // Keep 1 year cache for generated images unless overridden
                    }
                    );

                    // Construct the predictable object key for logging purposes (uploadWithS3 handles actual key creation)
                    // Note: uploadWithS3 adds a UUID, so the *actual* S3 key will differ.
                    // We log the intended *predictable* key structure for reference.
                    const auto predictableObjectKey = "generations/" + std::to_string(tokenMint) + "/gen-" + std::to_string(generationNumber) + ".jpg";
                    logUploadedFile(predictableObjectKey, publicUrl); // Log with the *returned* public URL;

                    std::cout << `Successfully uploaded generated image via S3 API << Public URL: ${publicUrl}` << std::endl;
                    return publicUrl;

                    } catch (error) {
                        std::cerr << "Error in uploadGeneratedImage (S3 API) for Key " + std::to_string(objectKey) + ":" << error << std::endl;
                        throw std::runtime_error(`Failed to upload generated image: ${error instanceof Error ? error.message : String(error)}`);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
