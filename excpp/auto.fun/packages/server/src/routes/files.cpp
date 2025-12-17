#include "files.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> uploadToStorage(const std::vector<uint8_t>& buffer, { contentType: string options, string } key) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Use the shared S3 client getter
        const auto { client: s3Client, bucketName, publicBaseUrl } = getS3Client();

        const auto putCmd = new PutObjectCommand({;
            Bucket: bucketName,
            Key: options.key,
            Body: buffer,
            ContentType: options.contentType,
            // ACL: 'public-read', // ACL might not be needed/supported depending on setup (e.g., MinIO policies)
            });

            try {
                s3Client.send(putCmd);
                std::cout << "Successfully uploaded to Storage: " + options.key << std::endl;
                const auto finalPublicUrl = publicBaseUrl + "/" + options.key;
                return finalPublicUrl;
                } catch (error) {
                    std::cerr << "Failed to upload " + options.key + " to Storage:" << error << std::endl;
                    throw std::runtime_error(`Failed to upload ${options.key}`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
