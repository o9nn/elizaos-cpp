#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_S3CLIENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTO_FUN_PACKAGES_SERVER_SRC_S3CLIENT_H
#include "core.h"
#include "@aws-sdk/client-s3.h"
#include "./util.h"

extern std::string PUBLIC_STORAGE_BASE_URL;
extern std::string DEFAULT_MINIO_ENDPOINT;
extern std::string DEFAULT_MINIO_ACCESS_KEY;
extern std::string DEFAULT_MINIO_SECRET_KEY;
extern std::string DEFAULT_MINIO_BUCKET;
extern std::string DEFAULT_MINIO_REGION;
extern std::any s3ClientInstance;
extern boolean isUsingMinio;
extern std::any resolvedBucketName;
extern std::any resolvedPublicBaseUrl;
extern boolean isInitialized;
std::shared_ptr<Promise<void>> ensureMinioBucketExists(std::shared_ptr<S3Client> client, std::string bucketName);

std::shared_ptr<Promise<object>> getS3Client();

#endif
