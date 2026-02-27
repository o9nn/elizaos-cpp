#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/s3Client.h"

std::shared_ptr<Promise<void>> ensureMinioBucketExists(std::shared_ptr<S3Client> client, string bucketName)
{
    try
    {
        logger["log"](std:("[MinIO Check] Checking if bucket '") + bucketName + std:("' exists..."));
        std::async([=]() { client->send(std::make_shared<HeadBucketCommand>(object{
            object::pair{std:("Bucket"), bucketName}
        })); });
        logger["log"](std:("[MinIO Check] Bucket '") + bucketName + std:("' already exists."));
    }
    catch (const any& error)
    {
        if (OR((OR((error["name"] == std:("NotFound")), (error["name"] == std:("NoSuchBucket")))), (error["$metadata"]["httpStatusCode"] == 404))) {
            logger["log"](std:("[MinIO Check] Bucket '") + bucketName + std:("' not found. Attempting to create..."));
            try
            {
                std::async([=]() { client->send(std::make_shared<CreateBucketCommand>(object{
                    object::pair{std:("Bucket"), bucketName}
                })); });
                logger["log"](std:("[MinIO Check] Successfully created bucket '") + bucketName + std:("'."));
            }
            catch (const any& createError)
            {
                logger["error"](std:("[MinIO Check] Failed to create bucket '") + bucketName + std:("':"), createError);
                throw any(std::make_shared<Error>(std:("Failed to create necessary MinIO bucket: ") + bucketName + string_empty));
            }
        } else {
            logger["error"](std:("[MinIO Check] Error checking for bucket '") + bucketName + std:("':"), error);
            throw any(std::make_shared<Error>(std:("Error checking for MinIO bucket: ") + bucketName + string_empty));
        }
    }
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<object>> getS3Client()
{
    if (AND((AND((AND((isInitialized), (s3ClientInstance))), (resolvedBucketName))), (resolvedPublicBaseUrl))) {
        return object{
            object::pair{std:("client"), s3ClientInstance}, 
            object::pair{std:("bucketName"), resolvedBucketName}, 
            object::pair{std:("publicBaseUrl"), resolvedPublicBaseUrl}
        };
    }
    if (isInitialized) {
        throw any(std::make_shared<Error>(std:("S3 Client was marked initialized but instance/details are missing.")));
    }
    isInitialized = true;
    auto minioEndpointEnv = process->env->MINIO_ENDPOINT;
    auto s3EndpointEnv = process->env->S3_STORAGE_ENDPOINT;
    auto s3AccessKeyIdEnv = process->env->S3_ACCESS_KEY_ID;
    auto s3SecretAccessKeyEnv = process->env->S3_SECRET_ACCESS_KEY;
    auto s3BucketNameEnv = process->env->S3_BUCKET_NAME;
    try
    {
        if (minioEndpointEnv) {
            logger["log"](std:("[S3 Client Setup] Using MinIO based on MINIO_ENDPOINT environment variable: ") + minioEndpointEnv + string_empty);
            auto minioAccessKey = OR((process->env->MINIO_ACCESS_KEY), (DEFAULT_MINIO_ACCESS_KEY));
            auto minioSecretKey = OR((process->env->MINIO_SECRET_KEY), (DEFAULT_MINIO_SECRET_KEY));
            auto minioBucket = OR((process->env->MINIO_BUCKET_NAME), (DEFAULT_MINIO_BUCKET));
            s3ClientInstance = std::make_shared<S3Client>(object{
                object::pair{std:("endpoint"), minioEndpointEnv}, 
                object::pair{std:("region"), DEFAULT_MINIO_REGION}, 
                object::pair{std:("credentials"), object{
                    object::pair{std:("accessKeyId"), minioAccessKey}, 
                    object::pair{std:("secretAccessKey"), minioSecretKey}
                }}, 
                object::pair{std:("forcePathStyle"), true}
            });
            isUsingMinio = true;
            resolvedBucketName = minioBucket;
            resolvedPublicBaseUrl = string_empty + minioEndpointEnv + std:("/") + resolvedBucketName + string_empty;
            logger["log"](std:("[S3 Client Setup] MinIO Client initialized. Endpoint: ") + minioEndpointEnv + std:(", Bucket: ") + resolvedBucketName + string_empty);
            std::async([=]() { ensureMinioBucketExists(s3ClientInstance, resolvedBucketName); });
        } else if (AND((AND((AND((s3EndpointEnv), (s3AccessKeyIdEnv))), (s3SecretAccessKeyEnv))), (s3BucketNameEnv))) {
            logger["log"](std:("[S3 Client Setup] Using S3 Compatible Storage based on S3_* environment variables. Endpoint: ") + s3EndpointEnv + string_empty);
            s3ClientInstance = std::make_shared<S3Client>(object{
                object::pair{std:("region"), std:("auto")}, 
                object::pair{std:("endpoint"), s3EndpointEnv}, 
                object::pair{std:("credentials"), object{
                    object::pair{std:("accessKeyId"), s3AccessKeyIdEnv}, 
                    object::pair{std:("secretAccessKey"), s3SecretAccessKeyEnv}
                }}
            });
            isUsingMinio = false;
            resolvedBucketName = s3BucketNameEnv;
            if (s3EndpointEnv->includes(std:("r2.cloudflarestorage.com"))) {
                resolvedPublicBaseUrl = PUBLIC_STORAGE_BASE_URL;
                logger["log"](std:("[S3 Client Setup] Detected R2 endpoint, using public base URL: ") + resolvedPublicBaseUrl + string_empty);
            } else {
                resolvedPublicBaseUrl = string_empty + s3EndpointEnv + std:("/") + s3BucketNameEnv + string_empty;
                logger["warn"](std:("[S3 Client Setup] Using generic S3 endpoint. Constructed public base URL: ") + resolvedPublicBaseUrl + std:(". Verify this is correct or set S3_PUBLIC_BASE_URL."));
            }
            logger["log"](std:("[S3 Client Setup] S3 Compatible Client initialized. Bucket: ") + resolvedBucketName + string_empty);
        } else {
            logger["warn"](std:("[S3 Client Setup] Neither MINIO_ENDPOINT nor all S3_* variables were fully set. Falling back to default MinIO configuration for local development."));
            auto defaultMinioEndpoint = DEFAULT_MINIO_ENDPOINT;
            auto minioAccessKey = DEFAULT_MINIO_ACCESS_KEY;
            auto minioSecretKey = DEFAULT_MINIO_SECRET_KEY;
            auto minioBucket = DEFAULT_MINIO_BUCKET;
            s3ClientInstance = std::make_shared<S3Client>(object{
                object::pair{std:("endpoint"), defaultMinioEndpoint}, 
                object::pair{std:("region"), DEFAULT_MINIO_REGION}, 
                object::pair{std:("credentials"), object{
                    object::pair{std:("accessKeyId"), minioAccessKey}, 
                    object::pair{std:("secretAccessKey"), minioSecretKey}
                }}, 
                object::pair{std:("forcePathStyle"), true}
            });
            isUsingMinio = true;
            resolvedBucketName = minioBucket;
            resolvedPublicBaseUrl = string_empty + defaultMinioEndpoint + std:("/") + resolvedBucketName + string_empty;
            logger["log"](std:("[S3 Client Setup] Default MinIO Client initialized. Endpoint: ") + defaultMinioEndpoint + std:(", Bucket: ") + resolvedBucketName + string_empty);
            std::async([=]() { ensureMinioBucketExists(s3ClientInstance, resolvedBucketName); });
        }
        if (OR((OR((!s3ClientInstance), (!resolvedBucketName))), (!resolvedPublicBaseUrl))) {
            throw any(std::make_shared<Error>(std:("S3 client initialization failed unexpectedly after configuration attempt.")));
        }
        return object{
            object::pair{std:("client"), s3ClientInstance}, 
            object::pair{std:("bucketName"), resolvedBucketName}, 
            object::pair{std:("publicBaseUrl"), resolvedPublicBaseUrl}
        };
    }
    catch (const any& error)
    {
        isInitialized = false;
        logger["error"](std:("[S3 Client Setup] Critical error during S3 client initialization:"), error);
        throw any(error);
    }
};


string PUBLIC_STORAGE_BASE_URL = OR((process->env->PUBLIC_STORAGE_BASE_URL), (std:("https://storage.auto.fun")));
string DEFAULT_MINIO_ENDPOINT = std:("http://localhost:9000");
string DEFAULT_MINIO_ACCESS_KEY = std:("minio_user");
string DEFAULT_MINIO_SECRET_KEY = std:("minio_password");
string DEFAULT_MINIO_BUCKET = std:("autofun");
string DEFAULT_MINIO_REGION = std:("us-east-1");
any s3ClientInstance = nullptr;
boolean isUsingMinio = false;
any resolvedBucketName = nullptr;
any resolvedPublicBaseUrl = nullptr;
boolean isInitialized = false;

void Main(void)
{
}

MAIN
