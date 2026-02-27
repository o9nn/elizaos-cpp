#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_S3CLIENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_SERVER_SRC_S3CLIENT_H
#include "core.hpp"
// External dependency removed
#include "./util.h"

extern string PUBLIC_STORAGE_BASE_URL;
extern string DEFAULT_MINIO_ENDPOINT;
extern string DEFAULT_MINIO_ACCESS_KEY;
extern string DEFAULT_MINIO_SECRET_KEY;
extern string DEFAULT_MINIO_BUCKET;
extern string DEFAULT_MINIO_REGION;
extern any s3ClientInstance;
extern boolean isUsingMinio;
extern any resolvedBucketName;
extern any resolvedPublicBaseUrl;
extern boolean isInitialized;
std::shared_ptr<Promise<void>> ensureMinioBucketExists(std::shared_ptr<S3Client> client, string bucketName);

std::shared_ptr<Promise<object>> getS3Client();

#endif
