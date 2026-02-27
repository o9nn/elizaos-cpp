#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/tests/storage-integration.test.h"

object StorageIntegrationTestSuite = object{
    object::pair{std:("name"), std:("StorageIntegrationTestSuite")}, 
    object::pair{std:("tests"), array<object>{ object{
        object::pair{std:("name"), std:("storage_configuration_validation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔥 REAL TEST: Storage configuration validation"));
            auto service = runtime->getService(std:("elizaos-services"));
            if (!service) {
                throw any(std::make_shared<Error>(std:("ElizaOS Services service not found - plugin not loaded correctly")));
            }
            auto storage = (as<any>(service))["getStorage"]();
            if (!storage) {
                throw any(std::make_shared<Error>(std:("Storage service not available - service initialization failed")));
            }
            auto requiredEnvVars = array<string>{ std:("ELIZAOS_STORAGE_ENDPOINT"), std:("ELIZAOS_STORAGE_BUCKET"), std:("ELIZAOS_STORAGE_ACCESS_KEY"), std:("ELIZAOS_STORAGE_SECRET_KEY") };
            auto missingVars = requiredEnvVars->filter([=](auto varName) mutable
            {
                return OR((!const_(process->env)[varName]), (const_(process->env)[varName]->get_length() == 0));
            }
            );
            if (missingVars->get_length() > 0) {
                throw any(std::make_shared<Error>(std:("Storage configuration incomplete. Missing: ") + missingVars->join(std:(", ")) + std:(". ") + std:("Set these environment variables to run storage integration tests.")));
            }
            console->log(std:("✅ Storage configuration validation passed"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("storage_upload_download_cycle")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔥 REAL TEST: Storage upload/download cycle"));
            auto service = runtime->getService(std:("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto testKey = std:("integration-test/") + Date->now() + std:("-upload-download.txt");
            auto originalData = Buffer::from(std:("Integration test data - ") + ((std::make_shared<Date>()))->toISOString() + std:("\
Multi-line content\
with special chars: éñ中文🚀"));
            try
            {
                console->log(std:("🔄 Uploading test file: ") + testKey + string_empty);
                auto uploadResult = std::async([=]() { storage["uploadFile"](testKey, originalData, std:("text/plain")); });
                if (uploadResult != testKey) {
                    throw any(std::make_shared<Error>(std:("Upload returned unexpected key: expected "") + testKey + std:("", got "") + uploadResult + std:(""")));
                }
                console->log(std:("✅ Upload successful"));
                auto exists = std::async([=]() { storage["fileExists"](testKey); });
                if (!exists) {
                    throw any(std::make_shared<Error>(std:("File existence check failed immediately after upload")));
                }
                console->log(std:("✅ File existence confirmed"));
                auto metadata = std::async([=]() { storage["getFileMetadata"](testKey); });
                if (!metadata) {
                    throw any(std::make_shared<Error>(std:("Failed to retrieve file metadata")));
                }
                if (metadata["size"] != originalData->length) {
                    throw any(std::make_shared<Error>(std:("Size mismatch: expected ") + originalData->length + std:(", got ") + metadata["size"] + string_empty));
                }
                if (metadata["contentType"] != std:("text/plain")) {
                    throw any(std::make_shared<Error>(std:("Content type mismatch: expected "text/plain", got "") + metadata["contentType"] + std:(""")));
                }
                console->log(std:("✅ Metadata validation passed: ") + metadata["size"] + std:(" bytes, ") + metadata["contentType"] + string_empty);
                console->log(std:("🔄 Downloading file for verification"));
                auto downloadedData = std::async([=]() { storage["downloadFile"](testKey); });
                if (!downloadedData["equals"](originalData)) {
                    throw any(std::make_shared<Error>(std:("Downloaded data does not match uploaded data")));
                }
                console->log(std:("✅ Download and data integrity verification passed"));
                std::async([=]() { storage["deleteFile"](testKey); });
                console->log(std:("✅ File cleanup completed"));
                auto existsAfterDelete = std::async([=]() { storage["fileExists"](testKey); });
                if (existsAfterDelete) {
                    throw any(std::make_shared<Error>(std:("File still exists after deletion")));
                }
                console->log(std:("✅ Deletion verification passed"));
                console->log(std:("✅ REAL STORAGE UPLOAD/DOWNLOAD CYCLE SUCCESS"));
            }
            catch (const any& error)
            {
                try
                {
                    std::async([=]() { storage["deleteFile"](testKey); });
                }
                catch (const any& cleanupError)
                {
                    console->warn(std:("Failed to cleanup test file on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("storage_signed_url_generation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔥 REAL TEST: Storage signed URL generation"));
            auto service = runtime->getService(std:("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto testKey = std:("integration-test/") + Date->now() + std:("-signed-url.txt");
            auto testData = Buffer::from(std:("Signed URL test data"));
            try
            {
                std::async([=]() { storage["uploadFile"](testKey, testData, std:("text/plain")); });
                auto getUrl = std::async([=]() { storage["getSignedUrl"](testKey, std:("get"), 300); });
                if (!getUrl["startsWith"](std:("http"))) {
                    throw any(std::make_shared<Error>(std:("Invalid GET signed URL: ") + getUrl + string_empty));
                }
                if (OR((!getUrl["includes"](testKey)), (!getUrl["includes"](std:("X-Amz-Signature"))))) {
                    throw any(std::make_shared<Error>(std:("GET signed URL missing required components")));
                }
                console->log(std:("✅ GET signed URL generation passed"));
                auto putKey = std:("integration-test/") + Date->now() + std:("-signed-put.txt");
                auto putUrl = std::async([=]() { storage["getSignedUrl"](putKey, std:("put"), 300); });
                if (!putUrl["startsWith"](std:("http"))) {
                    throw any(std::make_shared<Error>(std:("Invalid PUT signed URL: ") + putUrl + string_empty));
                }
                if (OR((!putUrl["includes"](putKey)), (!putUrl["includes"](std:("X-Amz-Signature"))))) {
                    throw any(std::make_shared<Error>(std:("PUT signed URL missing required components")));
                }
                console->log(std:("✅ PUT signed URL generation passed"));
                auto shortUrl = std::async([=]() { storage["getSignedUrl"](testKey, std:("get"), 60); });
                auto longUrl = std::async([=]() { storage["getSignedUrl"](testKey, std:("get"), 3600); });
                if (shortUrl == longUrl) {
                    throw any(std::make_shared<Error>(std:("Signed URLs with different expiration times should differ")));
                }
                console->log(std:("✅ Expiration time handling passed"));
                console->log(std:("✅ REAL STORAGE SIGNED URL GENERATION SUCCESS"));
                std::async([=]() { storage["deleteFile"](testKey); });
            }
            catch (const any& error)
            {
                try
                {
                    std::async([=]() { storage["deleteFile"](testKey); });
                }
                catch (const any& cleanupError)
                {
                    console->warn(std:("Failed to cleanup test file on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("storage_list_operations")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔥 REAL TEST: Storage list operations"));
            auto service = runtime->getService(std:("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto testPrefix = std:("integration-test/list-test-") + Date->now() + string_empty;
            auto testFiles = array<string>{ string_empty + testPrefix + std:("/file1.txt"), string_empty + testPrefix + std:("/file2.txt"), string_empty + testPrefix + std:("/subdir/file3.txt") };
            try
            {
                for (auto& filePath : testFiles)
                {
                    auto data = Buffer::from(std:("Content for ") + filePath + string_empty);
                    std::async([=]() { storage["uploadFile"](filePath, data, std:("text/plain")); });
                }
                console->log(std:("✅ Uploaded ") + testFiles->get_length() + std:(" test files"));
                auto listedFiles = std::async([=]() { storage["listFiles"](testPrefix); });
                if (listedFiles["length"] < testFiles->get_length()) {
                    throw any(std::make_shared<Error>(std:("Expected at least ") + testFiles->get_length() + std:(" files, got ") + listedFiles["length"] + string_empty));
                }
                for (auto& testFile : testFiles)
                {
                    if (!listedFiles["includes"](testFile)) {
                        throw any(std::make_shared<Error>(std:("File ") + testFile + std:(" not found in list results")));
                    }
                }
                console->log(std:("✅ Prefix-based file listing passed"));
                auto subdirFiles = std::async([=]() { storage["listFiles"](string_empty + testPrefix + std:("/subdir/")); });
                auto expectedSubdirFile = string_empty + testPrefix + std:("/subdir/file3.txt");
                if (!subdirFiles["includes"](expectedSubdirFile)) {
                    throw any(std::make_shared<Error>(std:("Subdirectory listing failed")));
                }
                console->log(std:("✅ Subdirectory listing passed"));
                auto limitedFiles = std::async([=]() { storage["listFiles"](testPrefix, 2); });
                if (limitedFiles["length"] > 2) {
                    throw any(std::make_shared<Error>(std:("Max keys limit not respected: got ") + limitedFiles["length"] + std:(" files")));
                }
                console->log(std:("✅ Max keys limit handling passed"));
                console->log(std:("✅ REAL STORAGE LIST OPERATIONS SUCCESS"));
                for (auto& filePath : testFiles)
                {
                    std::async([=]() { storage["deleteFile"](filePath); });
                }
                console->log(std:("✅ Test files cleanup completed"));
            }
            catch (const any& error)
            {
                try
                {
                    for (auto& filePath : testFiles)
                    {
                        std::async([=]() { storage["deleteFile"](filePath); });
                    }
                }
                catch (const any& cleanupError)
                {
                    console->warn(std:("Failed to cleanup test files on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("storage_error_handling")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔥 REAL TEST: Storage error handling"));
            auto service = runtime->getService(std:("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto nonExistentKey = std:("integration-test/non-existent-") + Date->now() + std:(".txt");
            try
            {
                std::async([=]() { storage["downloadFile"](nonExistentKey); });
                throw any(std::make_shared<Error>(std:("Download of non-existent file should have failed")));
            }
            catch (const any& error)
            {
                if (AND((is<Error>(error)), (!error->message->includes(std:("failed"))))) {
                    throw any(std::make_shared<Error>(std:("Download error should contain meaningful message")));
                }
                console->log(std:("✅ Non-existent file download error handling passed"));
            }
            auto exists = std::async([=]() { storage["fileExists"](nonExistentKey); });
            if (exists) {
                throw any(std::make_shared<Error>(std:("Non-existent file should not report as existing")));
            }
            console->log(std:("✅ Non-existent file existence check passed"));
            auto metadata = std::async([=]() { storage["getFileMetadata"](nonExistentKey); });
            if (metadata != nullptr) {
                throw any(std::make_shared<Error>(std:("Non-existent file should return null metadata")));
            }
            console->log(std:("✅ Non-existent file metadata handling passed"));
            try
            {
                std::async([=]() { storage["deleteFile"](nonExistentKey); });
                console->log(std:("✅ Non-existent file deletion handling passed"));
            }
            catch (const any& error)
            {
                console->log(std:("⚠️  Storage throws on non-existent file deletion (acceptable behavior)"));
            }
            console->log(std:("✅ REAL STORAGE ERROR HANDLING SUCCESS"));
        }
        }
    }, object{
        object::pair{std:("name"), std:("storage_large_file_handling")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            console->log(std:("🔥 REAL TEST: Storage large file handling"));
            auto service = runtime->getService(std:("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto largeSizeBytes = 1024 * 1024;
            auto testKey = std:("integration-test/") + Date->now() + std:("-large-file.bin");
            auto largeData = Buffer::alloc(largeSizeBytes);
            for (auto i = 0; i < largeSizeBytes; i++)
            {
                (*largeData)[i] = i % 256;
            }
            try
            {
                console->log(std:("🔄 Uploading large file: ") + largeSizeBytes + std:(" bytes"));
                auto startTime = Date->now();
                std::async([=]() { storage["uploadFile"](testKey, largeData, std:("application/octet-stream")); });
                auto uploadTime = Date->now() - startTime;
                console->log(std:("✅ Large file upload completed in ") + uploadTime + std:("ms"));
                auto metadata = std::async([=]() { storage["getFileMetadata"](testKey); });
                if (OR((!metadata), (metadata["size"] != largeSizeBytes))) {
                    throw any(std::make_shared<Error>(std:("Size mismatch: expected ") + largeSizeBytes + std:(", got ") + metadata["size"] + string_empty));
                }
                console->log(std:("✅ Large file metadata validation passed"));
                console->log(std:("🔄 Downloading large file for verification"));
                auto downloadStartTime = Date->now();
                auto downloadedData = std::async([=]() { storage["downloadFile"](testKey); });
                auto downloadTime = Date->now() - downloadStartTime;
                console->log(std:("✅ Large file download completed in ") + downloadTime + std:("ms"));
                if (!downloadedData["equals"](largeData)) {
                    throw any(std::make_shared<Error>(std:("Large file data integrity check failed")));
                }
                console->log(std:("✅ Large file data integrity verification passed"));
                console->log(std:("✅ REAL STORAGE LARGE FILE HANDLING SUCCESS"));
                std::async([=]() { storage["deleteFile"](testKey); });
            }
            catch (const any& error)
            {
                try
                {
                    std::async([=]() { storage["deleteFile"](testKey); });
                }
                catch (const any& cleanupError)
                {
                    console->warn(std:("Failed to cleanup large test file on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    } }}
};

void Main(void)
{
}

MAIN
