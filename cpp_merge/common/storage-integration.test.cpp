#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/tests/storage-integration.test.h"

object StorageIntegrationTestSuite = object{
    object::pair{std::string("name"), std::string("StorageIntegrationTestSuite")}, 
    object::pair{std::string("tests"), array<object>{ object{
        object::pair{std::string("name"), std::string("storage_configuration_validation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔥 REAL TEST: Storage configuration validation"));
            auto service = runtime->getService(std::string("elizaos-services"));
            if (!service) {
                throw any(std::make_shared<Error>(std::string("ElizaOS Services service not found - plugin not loaded correctly")));
            }
            auto storage = (as<any>(service))["getStorage"]();
            if (!storage) {
                throw any(std::make_shared<Error>(std::string("Storage service not available - service initialization failed")));
            }
            auto requiredEnvVars = array<string>{ std::string("ELIZAOS_STORAGE_ENDPOINT"), std::string("ELIZAOS_STORAGE_BUCKET"), std::string("ELIZAOS_STORAGE_ACCESS_KEY"), std::string("ELIZAOS_STORAGE_SECRET_KEY") };
            auto missingVars = requiredEnvVars->filter([=](auto varName) mutable
            {
                return OR((!const_(process->env)[varName]), (const_(process->env)[varName]->get_length() == 0));
            }
            );
            if (missingVars->get_length() > 0) {
                throw any(std::make_shared<Error>(std::string("Storage configuration incomplete. Missing: ") + missingVars->join(std::string(", ")) + std::string(". ") + std::string("Set these environment variables to run storage integration tests.")));
            }
            console->log(std::string("✅ Storage configuration validation passed"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("storage_upload_download_cycle")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔥 REAL TEST: Storage upload/download cycle"));
            auto service = runtime->getService(std::string("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto testKey = std::string("integration-test/") + Date->now() + std::string("-upload-download.txt");
            auto originalData = Buffer::from(std::string("Integration test data - ") + ((std::make_shared<Date>()))->toISOString() + std::string("\
Multi-line content\
with special chars: éñ中文🚀"));
            try
            {
                console->log(std::string("🔄 Uploading test file: ") + testKey + string_empty);
                auto uploadResult = std::async([=]() { storage["uploadFile"](testKey, originalData, std::string("text/plain")); });
                if (uploadResult != testKey) {
                    throw any(std::make_shared<Error>(std::string("Upload returned unexpected key: expected "") + testKey + std::string("", got "") + uploadResult + std::string(""")));
                }
                console->log(std::string("✅ Upload successful"));
                auto exists = std::async([=]() { storage["fileExists"](testKey); });
                if (!exists) {
                    throw any(std::make_shared<Error>(std::string("File existence check failed immediately after upload")));
                }
                console->log(std::string("✅ File existence confirmed"));
                auto metadata = std::async([=]() { storage["getFileMetadata"](testKey); });
                if (!metadata) {
                    throw any(std::make_shared<Error>(std::string("Failed to retrieve file metadata")));
                }
                if (metadata["size"] != originalData->length) {
                    throw any(std::make_shared<Error>(std::string("Size mismatch: expected ") + originalData->length + std::string(", got ") + metadata["size"] + string_empty));
                }
                if (metadata["contentType"] != std::string("text/plain")) {
                    throw any(std::make_shared<Error>(std::string("Content type mismatch: expected "text/plain", got "") + metadata["contentType"] + std::string(""")));
                }
                console->log(std::string("✅ Metadata validation passed: ") + metadata["size"] + std::string(" bytes, ") + metadata["contentType"] + string_empty);
                console->log(std::string("🔄 Downloading file for verification"));
                auto downloadedData = std::async([=]() { storage["downloadFile"](testKey); });
                if (!downloadedData["equals"](originalData)) {
                    throw any(std::make_shared<Error>(std::string("Downloaded data does not match uploaded data")));
                }
                console->log(std::string("✅ Download and data integrity verification passed"));
                std::async([=]() { storage["deleteFile"](testKey); });
                console->log(std::string("✅ File cleanup completed"));
                auto existsAfterDelete = std::async([=]() { storage["fileExists"](testKey); });
                if (existsAfterDelete) {
                    throw any(std::make_shared<Error>(std::string("File still exists after deletion")));
                }
                console->log(std::string("✅ Deletion verification passed"));
                console->log(std::string("✅ REAL STORAGE UPLOAD/DOWNLOAD CYCLE SUCCESS"));
            }
            catch (const any& error)
            {
                try
                {
                    std::async([=]() { storage["deleteFile"](testKey); });
                }
                catch (const any& cleanupError)
                {
                    console->warn(std::string("Failed to cleanup test file on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("storage_signed_url_generation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔥 REAL TEST: Storage signed URL generation"));
            auto service = runtime->getService(std::string("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto testKey = std::string("integration-test/") + Date->now() + std::string("-signed-url.txt");
            auto testData = Buffer::from(std::string("Signed URL test data"));
            try
            {
                std::async([=]() { storage["uploadFile"](testKey, testData, std::string("text/plain")); });
                auto getUrl = std::async([=]() { storage["getSignedUrl"](testKey, std::string("get"), 300); });
                if (!getUrl["startsWith"](std::string("http"))) {
                    throw any(std::make_shared<Error>(std::string("Invalid GET signed URL: ") + getUrl + string_empty));
                }
                if (OR((!getUrl["includes"](testKey)), (!getUrl["includes"](std::string("X-Amz-Signature"))))) {
                    throw any(std::make_shared<Error>(std::string("GET signed URL missing required components")));
                }
                console->log(std::string("✅ GET signed URL generation passed"));
                auto putKey = std::string("integration-test/") + Date->now() + std::string("-signed-put.txt");
                auto putUrl = std::async([=]() { storage["getSignedUrl"](putKey, std::string("put"), 300); });
                if (!putUrl["startsWith"](std::string("http"))) {
                    throw any(std::make_shared<Error>(std::string("Invalid PUT signed URL: ") + putUrl + string_empty));
                }
                if (OR((!putUrl["includes"](putKey)), (!putUrl["includes"](std::string("X-Amz-Signature"))))) {
                    throw any(std::make_shared<Error>(std::string("PUT signed URL missing required components")));
                }
                console->log(std::string("✅ PUT signed URL generation passed"));
                auto shortUrl = std::async([=]() { storage["getSignedUrl"](testKey, std::string("get"), 60); });
                auto longUrl = std::async([=]() { storage["getSignedUrl"](testKey, std::string("get"), 3600); });
                if (shortUrl == longUrl) {
                    throw any(std::make_shared<Error>(std::string("Signed URLs with different expiration times should differ")));
                }
                console->log(std::string("✅ Expiration time handling passed"));
                console->log(std::string("✅ REAL STORAGE SIGNED URL GENERATION SUCCESS"));
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
                    console->warn(std::string("Failed to cleanup test file on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("storage_list_operations")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔥 REAL TEST: Storage list operations"));
            auto service = runtime->getService(std::string("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto testPrefix = std::string("integration-test/list-test-") + Date->now() + string_empty;
            auto testFiles = array<string>{ string_empty + testPrefix + std::string("/file1.txt"), string_empty + testPrefix + std::string("/file2.txt"), string_empty + testPrefix + std::string("/subdir/file3.txt") };
            try
            {
                for (auto& filePath : testFiles)
                {
                    auto data = Buffer::from(std::string("Content for ") + filePath + string_empty);
                    std::async([=]() { storage["uploadFile"](filePath, data, std::string("text/plain")); });
                }
                console->log(std::string("✅ Uploaded ") + testFiles->get_length() + std::string(" test files"));
                auto listedFiles = std::async([=]() { storage["listFiles"](testPrefix); });
                if (listedFiles["length"] < testFiles->get_length()) {
                    throw any(std::make_shared<Error>(std::string("Expected at least ") + testFiles->get_length() + std::string(" files, got ") + listedFiles["length"] + string_empty));
                }
                for (auto& testFile : testFiles)
                {
                    if (!listedFiles["includes"](testFile)) {
                        throw any(std::make_shared<Error>(std::string("File ") + testFile + std::string(" not found in list results")));
                    }
                }
                console->log(std::string("✅ Prefix-based file listing passed"));
                auto subdirFiles = std::async([=]() { storage["listFiles"](string_empty + testPrefix + std::string("/subdir/")); });
                auto expectedSubdirFile = string_empty + testPrefix + std::string("/subdir/file3.txt");
                if (!subdirFiles["includes"](expectedSubdirFile)) {
                    throw any(std::make_shared<Error>(std::string("Subdirectory listing failed")));
                }
                console->log(std::string("✅ Subdirectory listing passed"));
                auto limitedFiles = std::async([=]() { storage["listFiles"](testPrefix, 2); });
                if (limitedFiles["length"] > 2) {
                    throw any(std::make_shared<Error>(std::string("Max keys limit not respected: got ") + limitedFiles["length"] + std::string(" files")));
                }
                console->log(std::string("✅ Max keys limit handling passed"));
                console->log(std::string("✅ REAL STORAGE LIST OPERATIONS SUCCESS"));
                for (auto& filePath : testFiles)
                {
                    std::async([=]() { storage["deleteFile"](filePath); });
                }
                console->log(std::string("✅ Test files cleanup completed"));
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
                    console->warn(std::string("Failed to cleanup test files on error:"), cleanupError);
                }
                throw any(error);
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("storage_error_handling")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔥 REAL TEST: Storage error handling"));
            auto service = runtime->getService(std::string("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto nonExistentKey = std::string("integration-test/non-existent-") + Date->now() + std::string(".txt");
            try
            {
                std::async([=]() { storage["downloadFile"](nonExistentKey); });
                throw any(std::make_shared<Error>(std::string("Download of non-existent file should have failed")));
            }
            catch (const any& error)
            {
                if (AND((is<Error>(error)), (!error->message->includes(std::string("failed"))))) {
                    throw any(std::make_shared<Error>(std::string("Download error should contain meaningful message")));
                }
                console->log(std::string("✅ Non-existent file download error handling passed"));
            }
            auto exists = std::async([=]() { storage["fileExists"](nonExistentKey); });
            if (exists) {
                throw any(std::make_shared<Error>(std::string("Non-existent file should not report as existing")));
            }
            console->log(std::string("✅ Non-existent file existence check passed"));
            auto metadata = std::async([=]() { storage["getFileMetadata"](nonExistentKey); });
            if (metadata != nullptr) {
                throw any(std::make_shared<Error>(std::string("Non-existent file should return null metadata")));
            }
            console->log(std::string("✅ Non-existent file metadata handling passed"));
            try
            {
                std::async([=]() { storage["deleteFile"](nonExistentKey); });
                console->log(std::string("✅ Non-existent file deletion handling passed"));
            }
            catch (const any& error)
            {
                console->log(std::string("⚠️  Storage throws on non-existent file deletion (acceptable behavior)"));
            }
            console->log(std::string("✅ REAL STORAGE ERROR HANDLING SUCCESS"));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("storage_large_file_handling")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            console->log(std::string("🔥 REAL TEST: Storage large file handling"));
            auto service = runtime->getService(std::string("elizaos-services"));
            auto storage = (as<any>(service))["getStorage"]();
            auto largeSizeBytes = 1024 * 1024;
            auto testKey = std::string("integration-test/") + Date->now() + std::string("-large-file.bin");
            auto largeData = Buffer::alloc(largeSizeBytes);
            for (auto i = 0; i < largeSizeBytes; i++)
            {
                (*largeData)[i] = i % 256;
            }
            try
            {
                console->log(std::string("🔄 Uploading large file: ") + largeSizeBytes + std::string(" bytes"));
                auto startTime = Date->now();
                std::async([=]() { storage["uploadFile"](testKey, largeData, std::string("application/octet-stream")); });
                auto uploadTime = Date->now() - startTime;
                console->log(std::string("✅ Large file upload completed in ") + uploadTime + std::string("ms"));
                auto metadata = std::async([=]() { storage["getFileMetadata"](testKey); });
                if (OR((!metadata), (metadata["size"] != largeSizeBytes))) {
                    throw any(std::make_shared<Error>(std::string("Size mismatch: expected ") + largeSizeBytes + std::string(", got ") + metadata["size"] + string_empty));
                }
                console->log(std::string("✅ Large file metadata validation passed"));
                console->log(std::string("🔄 Downloading large file for verification"));
                auto downloadStartTime = Date->now();
                auto downloadedData = std::async([=]() { storage["downloadFile"](testKey); });
                auto downloadTime = Date->now() - downloadStartTime;
                console->log(std::string("✅ Large file download completed in ") + downloadTime + std::string("ms"));
                if (!downloadedData["equals"](largeData)) {
                    throw any(std::make_shared<Error>(std::string("Large file data integrity check failed")));
                }
                console->log(std::string("✅ Large file data integrity verification passed"));
                console->log(std::string("✅ REAL STORAGE LARGE FILE HANDLING SUCCESS"));
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
                    console->warn(std::string("Failed to cleanup large test file on error:"), cleanupError);
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
