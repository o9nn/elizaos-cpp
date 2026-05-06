#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> importAWSSDK() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Use dynamic imports that will be resolved at runtime
            const auto s3Module = import("@aws-sdk/client-s3").catch(() => nullptr);
            const auto presignerModule = import("@aws-sdk/s3-request-presigner").catch(() => nullptr);
            const auto mimeModule = import("mime-types").catch(() => nullptr);

            if (!s3Module || !presignerModule || !mimeModule) {
                throw std::runtime_error('AWS SDK modules not available');
            }

            return {
                S3Client: s3Module.S3Client,
                PutObjectCommand: s3Module.PutObjectCommand,
                GetObjectCommand: s3Module.GetObjectCommand,
                DeleteObjectCommand: s3Module.DeleteObjectCommand,
                HeadObjectCommand: s3Module.HeadObjectCommand,
                ListObjectsV2Command: s3Module.ListObjectsV2Command,
                getSignedUrl: presignerModule.getSignedUrl,
                lookup: mimeModule.lookup,
                };
                } catch (_error) {
                    std::cout << "AWS SDK packages not available - storage functionality will be limited" << std::endl;
                    return nullptr;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string getSetting(IAgentRuntime runtime, const std::string& key, std::optional<std::string> defaultValue) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runtime.getSetting(key) || process.env[key] || defaultValue;

}

void getStorageConfig(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        endpoint: getSetting(runtime, "ELIZAOS_STORAGE_ENDPOINT"),
        bucket: getSetting(runtime, "ELIZAOS_STORAGE_BUCKET"),
        accessKey: getSetting(runtime, "ELIZAOS_STORAGE_ACCESS_KEY"),
        secretKey: getSetting(runtime, "ELIZAOS_STORAGE_SECRET_KEY"),
        };

}

std::future<std::optional<std::any>> createS3Client(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getStorageConfig(runtime);

    if (!config.endpoint || !config.accessKey || !config.secretKey) {
        return nullptr;
    }

    try {
        const auto aws = importAWSSDK();
        if (!aws) {
            std::cout << "AWS SDK not available - cannot create S3 client" << std::endl;
            return nullptr;
        }

        return new aws.S3Client({;
            endpoint: config.endpoint,
            region: "auto", // Cloudflare R2 uses "auto" region
            credentials: {
                accessKeyId: config.accessKey,
                secretAccessKey: config.secretKey,
                },
                forcePathStyle: true, // Required for R2 compatibility
                });
                } catch (error) {
                    logger.error(
                    "Failed to create S3 client: " + std::to_string(true /* instanceof check */ ? error.message : "Unknown error")
                    );
                    return nullptr;
                }

}

} // namespace elizaos
