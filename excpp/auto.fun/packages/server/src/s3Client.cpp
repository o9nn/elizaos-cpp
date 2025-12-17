#include "s3Client.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> ensureMinioBucketExists(S3Client client, const std::string& bucketName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << "[MinIO Check] Checking if bucket "" + bucketName + "" exists..." << std::endl;
            client.send(new HeadBucketCommand({ Bucket: bucketName }));
            std::cout << "[MinIO Check] Bucket "" + bucketName + "" already exists." << std::endl;
            } catch (error: any) {
                // Check if the error is because the bucket doesn't exist
                // Error handling might differ slightly based on SDK version/MinIO version
                // Common indicators: NotFound, NoSuchBucket, status code 404
                if (error.name == 'NotFound' || error.name == 'NoSuchBucket' || error.$metadata.httpStatusCode == 404) {
                    std::cout << "[MinIO Check] Bucket "" + bucketName + "" not found. Attempting to create..." << std::endl;
                    try {
                        client.send(new CreateBucketCommand({ Bucket: bucketName }));
                        std::cout << "[MinIO Check] Successfully created bucket "" + bucketName + ""." << std::endl;
                        } catch (createError) {
                            std::cerr << "[MinIO Check] Failed to create bucket "" + bucketName + "":" << createError << std::endl;
                            throw std::runtime_error(`Failed to create necessary MinIO bucket: ${bucketName}`);
                        }
                        } else {
                            // Unexpected error during HeadBucket check
                            std::cerr << "[MinIO Check] Error checking for bucket "" + bucketName + "":" << error << std::endl;
                            throw std::runtime_error(`Error checking for MinIO bucket: ${bucketName}`);
                        }
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<> getS3Client() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    client: S3Client, bucketName: string, publicBaseUrl: string
}

} // namespace elizaos
