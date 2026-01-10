#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/shared/file-utils.h"

string sanitizeFilename(string filename)
{
    if (!filename) {
        return std::string("unnamed");
    }
    auto sanitized = filename->replace((new RegExp(std::string("[/\\:*?"<>|]"))), std::string("_"))->replace((new RegExp(std::string("\0"))), string_empty)->replace((new RegExp(std::string("\.+"))), std::string("."))->trim();
    if (OR((!sanitized), (sanitized == std::string(".")))) {
        return std::string("unnamed");
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
            logger->debug(std::string("[FILE] Successfully cleaned up file: ") + normalizedPath + string_empty);
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("Error cleaning up file ") + filePath + std::string(":"), error);
    }
};
std::function<void(array<any>)> cleanupFiles = [=](auto files) mutable
{
    if (files) {
        files->forEach([=](auto file) mutable
        {
            logger->debug(std::string("[FILE] Multer file ") + file->originalname + std::string(" in memory, no cleanup needed"));
        }
        );
    }
};
std::function<void(any)> cleanupUploadedFile = [=](auto file) mutable
{
    logger->debug(std::string("[FILE] Multer file ") + file->originalname + std::string(" in memory, no cleanup needed"));
};

void Main(void)
{
}

MAIN
