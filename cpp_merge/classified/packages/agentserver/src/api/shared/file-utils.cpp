#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/shared/file-utils.h"

string sanitizeFilename(string filename)
{
    if (!filename) {
        return std:("unnamed");
    }
    auto sanitized = filename->replace((new RegExp(std:("[/\\:*?"<>|]"))), std:("_"))->replace((new RegExp(std:("\0"))), string_empty)->replace((new RegExp(std:("\.+"))), std:("."))->trim();
    if (OR((!sanitized), (sanitized == std:(".")))) {
        return std:("unnamed");
    }
    auto maxLength = 255;
    if (sanitized->get_length() > maxLength) {
        auto ext = path->extname(sanitized);
        auto nameWithoutExt = path->basename(sanitized, ext);
        auto truncatedName = nameWithoutExt->substring(0, maxLength - ext->length - 1);
        return truncatedName + ext;
    }
    return sanitized;
};


std::function<void(string)> cleanupFile = [=](auto filePath) mutable
{
    try
    {
        if (AND((filePath), (fs->existsSync(filePath)))) {
            auto normalizedPath = path->normalize(filePath);
            fs->unlinkSync(normalizedPath);
            logger->debug(std:("[FILE] Successfully cleaned up file: ") + normalizedPath + string_empty);
        }
    }
    catch (const any& error)
    {
        logger->error(std:("Error cleaning up file ") + filePath + std:(":"), error);
    }
};
std::function<void(array<any>)> cleanupFiles = [=](auto files) mutable
{
    if (files) {
        files->forEach([=](auto file) mutable
        {
            logger->debug(std:("[FILE] Multer file ") + file->originalname + std:(" in memory, no cleanup needed"));
        }
        );
    }
};
std::function<void(any)> cleanupUploadedFile = [=](auto file) mutable
{
    logger->debug(std:("[FILE] Multer file ") + file->originalname + std:(" in memory, no cleanup needed"));
};

void Main(void)
{
}

MAIN
