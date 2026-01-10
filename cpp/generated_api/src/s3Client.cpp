#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/s3Client.h"

std::shared_ptr<Promise<void>> ensureMinioBucketExists(std::shared_ptr<S3Client> client, string bucketName)
{
    try
    {
        logger["log"](std::string("[MinIO Check] Checking if bucket '") + bucketName + std::string("' exists..."));
        std::async([=]() { client->send(std::make_shared<HeadBucketCommand>(object{
            object::pair{std::string("Bucket"), bucketName}
        })); });
        logger["log"](std::string("[MinIO Check] Bucket '") + bucketName + std::string("' already exists."));
    }
    catch (const any& error)
    {
        if (OR((OR((error["name"] == std::string("NotFound")), (error["name"] == std::string("NoSuchBucket")))), (error["$metadata"]["httpStatusCode"] == 404))) {
            logger["log"](std::string("[MinIO Check] Bucket '") + bucketName + std::string("' not found. Attempting to create..."));
            try
            {
                std::async([=]() { client->send(std::make_shared<CreateBucketCommand>(object{
                    object::pair{std::string("Bucket"), bucketName}
                })); });
                logger["log"](std::string("[MinIO Check] Successfully created bucket '") + bucketName + std::string("'."));
            }
            catch (const any& createError)
            {
                logger["error"](std::string("[MinIO Check] Failed to create bucket '") + bucketName + std::string("':"), createError);
                throw any(std::make_shared<Error>(std::string("Failed to create necessary MinIO bucket: ") + bucketName + string_empty));
            }
        } else {
            logger["error"](std::string("[MinIO Check] Error checking for bucket '") + bucketName + std::string("':"), error);
            throw any(std::make_shared<Error>(std::string("Error checking for MinIO bucket: ") + bucketName + string_empty));
        }
    }
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<object>> getS3Client()
{
    if (AND((AND((AND((isInitialized), (s3ClientInstance))), (resolvedBucketName))), (resolvedPublicBaseUrl))) {
        return object{
            object::pair{std::string("client"), s3ClientInstance}, 
            object::pair{std::string("bucketName"), resolvedBucketName}, 
            object::pair{std::string("publicBaseUrl"), resolvedPublicBaseUrl}
        };
    }
    if (isInitialized) {
        throw any(std::make_shared<Error>(std::string("S3 Client was marked initialized but instance/details are missing.")));
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
            logger["log"](std::string("[S3 Client Setup] Using MinIO based on MINIO_ENDPOINT environment variable: ") + minioEndpointEnv + string_empty);
            auto minioAccessKey = OR((process->env->MINIO_ACCESS_KEY), (DEFAULT_MINIO_ACCESS_KEY));
            auto minioSecretKey = OR((process->env->MINIO_SECRET_KEY), (DEFAULT_MINIO_SECRET_KEY));
            auto minioBucket = OR((process->env->MINIO_BUCKET_NAME), (DEFAULT_MINIO_BUCKET));
            s3ClientInstance = std::make_shared<S3Client>(object{
                object::pair{std::string("endpoint"), minioEndpointEnv}, 
                object::pair{std::string("region"), DEFAULT_MINIO_REGION}, 
                object::pair{std::string("credentials"), object{
                    object::pair{std::string("accessKeyId"), minioAccessKey}, 
                    object::pair{std::string("secretAccessKey"), minioSecretKey}
                }}, 
                object::pair{std::string("forcePathStyle"), true}
            });
            isUsingMinio = true;
            resolvedBucketName = minioBucket;
            resolvedPublicBaseUrl = string_empty + minioEndpointEnv + std::string("/") + resolvedBucketName + string_empty;
            logger["log"](std::string("[S3 Client Setup] MinIO Client initialized. Endpoint: ") + minioEndpointEnv + std::string(", Bucket: ") + resolvedBucketName + string_empty);
            std::async([=]() { ensureMinioBucketExists(s3ClientInstance, resolvedBucketName); });
        } else if (AND((AND((AND((s3EndpointEnv), (s3AccessKeyIdEnv))), (s3SecretAccessKeyEnv))), (s3BucketNameEnv))) {
            logger["log"](std::string("[S3 Client Setup] Using S3 Compatible Storage based on S3_* environment variables. Endpoint: ") + s3EndpointEnv + string_empty);
            s3ClientInstance = std::make_shared<S3Client>(object{
                object::pair{std::string("region"), std::string("auto")}, 
                object::pair{std::string("endpoint"), s3EndpointEnv}, 
                object::pair{std::string("credentials"), object{
                    object::pair{std::string("accessKeyId"), s3AccessKeyIdEnv}, 
                    object::pair{std::string("secretAccessKey"), s3SecretAccessKeyEnv}
                }}
            });
            isUsingMinio = false;
            resolvedBucketName = s3BucketNameEnv;
            if (s3EndpointEnv->includes(std::string("r2.cloudflarestorage.com"))) {
                resolvedPublicBaseUrl = PUBLIC_STORAGE_BASE_URL;
                logger["log"](std::string("[S3 Client Setup] Detected R2 endpoint, using public base URL: ") + resolvedPublicBaseUrl + string_empty);
            } else {
                resolvedPublicBaseUrl = string_empty + s3EndpointEnv + std::string("/") + s3BucketNameEnv + string_empty;
                logger["warn"](std::string("[S3 Client Setup] Using generic S3 endpoint. Constructed public base URL: ") + resolvedPublicBaseUrl + std::string(". Verify this is correct or set S3_PUBLIC_BASE_URL."));
            }
            logger["log"](std::string("[S3 Client Setup] S3 Compatible Client initialized. Bucket: ") + resolvedBucketName + string_empty);
        } else {
            logger["warn"](std::string("[S3 Client Setup] Neither MINIO_ENDPOINT nor all S3_* variables were fully set. Falling back to default MinIO configuration for local development."));
            auto defaultMinioEndpoint = DEFAULT_MINIO_ENDPOINT;
            auto minioAccessKey = DEFAULT_MINIO_ACCESS_KEY;
            auto minioSecretKey = DEFAULT_MINIO_SECRET_KEY;
            auto minioBucket = DEFAULT_MINIO_BUCKET;
            s3ClientInstance = std::make_shared<S3Client>(object{
                object::pair{std::string("endpoint"), defaultMinioEndpoint}, 
                object::pair{std::string("region"), DEFAULT_MINIO_REGION}, 
                object::pair{std::string("credentials"), object{
                    object::pair{std::string("accessKeyId"), minioAccessKey}, 
                    object::pair{std::string("secretAccessKey"), minioSecretKey}
                }}, 
                object::pair{std::string("forcePathStyle"), true}
            });
            isUsingMinio = true;
            resolvedBucketName = minioBucket;
            resolvedPublicBaseUrl = string_empty + defaultMinioEndpoint + std::string("/") + resolvedBucketName + string_empty;
            logger["log"](std::string("[S3 Client Setup] Default MinIO Client initialized. Endpoint: ") + defaultMinioEndpoint + std::string(", Bucket: ") + resolvedBucketName + string_empty);
            std::async([=]() { ensureMinioBucketExists(s3ClientInstance, resolvedBucketName); });
        }
        if (OR((OR((!s3ClientInstance), (!resolvedBucketName))), (!resolvedPublicBaseUrl))) {
            throw any(std::make_shared<Error>(std::string("S3 client initialization failed unexpectedly after configuration attempt.")));
        }
        return object{
            object::pair{std::string("client"), s3ClientInstance}, 
            object::pair{std::string("bucketName"), resolvedBucketName}, 
            object::pair{std::string("publicBaseUrl"), resolvedPublicBaseUrl}
        };
    }
    catch (const any& error)
    {
        isInitialized = false;
        logger["error"](std::string("[S3 Client Setup] Critical error during S3 client initialization:"), error);
        throw any(error);
    }
};


string PUBLIC_STORAGE_BASE_URL = OR((process->env->PUBLIC_STORAGE_BASE_URL), (std::string("https://storage.auto.fun")));
string DEFAULT_MINIO_ENDPOINT = std::string("http://localhost:9000");
string DEFAULT_MINIO_ACCESS_KEY = std::string("minio_user");
string DEFAULT_MINIO_SECRET_KEY = std::string("minio_password");
string DEFAULT_MINIO_BUCKET = std::string("autofun");
string DEFAULT_MINIO_REGION = std::string("us-east-1");
any s3ClientInstance = nullptr;
boolean isUsingMinio = false;
any resolvedBucketName = nullptr;
any resolvedPublicBaseUrl = nullptr;
boolean isInitialized = false;

void Main(void)
{
}

MAIN
