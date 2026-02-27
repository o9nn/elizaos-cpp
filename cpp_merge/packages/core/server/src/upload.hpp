#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_UPLOAD_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_SERVER_SRC_UPLOAD_H
#include "core.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "multer.h"
using multer = _default;
#include "@elizaos/core.h"
#include "./api/shared/file-utils.js.h"
#include "./api/shared/constants.js.h"

string generateSecureFilename(string originalName);

template <typename P1>
string ensureUploadDir(string id, P1 type);

extern any storage;
extern std::function<any()> agentAudioUpload;
extern std::function<any()> agentMediaUpload;
extern std::function<any()> channelUpload;
extern std::function<any()> genericUpload;
extern std::function<any()> upload;
boolean validateAudioFile(std::shared_ptr<Express::Multer::File> file);

boolean validateMediaFile(std::shared_ptr<Express::Multer::File> file);

template <typename P2>
std::shared_ptr<Promise<object>> processUploadedFile(std::shared_ptr<Express::Multer::File> file, string targetId, P2 type);

template <typename P1>
string ensureUploadDir(string id, P1 type)
{
    if (!validateUuid(id)) {
        throw any(std::make_shared<Error>(std:("Invalid ") + type->slice(0, -1) + std:(" ID format")));
    }
    auto uploadDir = createSecureUploadDir(id, type);
    if (!fs->existsSync(uploadDir)) {
        fs->mkdirSync(uploadDir, object{
            object::pair{std:("recursive"), true}
        });
    }
    logger->debug(std:("[UPLOAD] Secure ") + type->slice(0, -1) + std:(" upload directory created: ") + uploadDir + string_empty);
    return uploadDir;
};


template <typename P2>
std::shared_ptr<Promise<object>> processUploadedFile(std::shared_ptr<Express::Multer::File> file, string targetId, P2 type)
{
    try
    {
        auto uploadDir = ensureUploadDir(targetId, type);
        auto filename = generateSecureFilename(file->originalname);
        auto finalPath = path->join(uploadDir, filename);
        std::async([=]() { fs->promises->writeFile(finalPath, file->buffer); });
        auto url = std:("/media/uploads/") + type + std:("/") + targetId + std:("/") + filename + string_empty;
        logger->debug(std:("[UPLOAD] File processed successfully: ") + filename + string_empty);
        return object{
            object::pair{std:("filename"), std:("filename")}, 
            object::pair{std:("path"), finalPath}, 
            object::pair{std:("url"), std:("url")}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("[UPLOAD] Error processing uploaded file:"), error);
        throw any(error);
    }
};


#endif
