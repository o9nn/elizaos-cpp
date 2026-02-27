#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/upload.h"

string generateSecureFilename(string originalName)
{
    auto uniqueSuffix = string_empty + Date->now() + std::string("-") + Math->round(Math->random() * 1000000000) + string_empty;
    auto sanitizedName = sanitizeFilename(originalName);
    return string_empty + uniqueSuffix + std::string("-") + sanitizedName + string_empty;
};


boolean validateAudioFile(std::shared_ptr<Express::Multer::File> file)
{
    return ALLOWED_AUDIO_MIME_TYPES->some([=](auto allowed) mutable
    {
        return allowed == file->mimetype;
    }
    );
};


boolean validateMediaFile(std::shared_ptr<Express::Multer::File> file)
{
    return ALLOWED_MEDIA_MIME_TYPES->some([=](auto allowed) mutable
    {
        return allowed == file->mimetype;
    }
    );
};


any storage = multer->memoryStorage();
std::function<any()> agentAudioUpload = [=]() mutable
{
    return multer(object{
        object::pair{std::string("storage"), std::string("storage")}, 
        object::pair{std::string("limits"), object{
            object::pair{std::string("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std::string("files"), 1}
        }}, 
        object::pair{std::string("fileFilter"), [=](auto _req, auto file, auto cb) mutable
        {
            auto isAllowed = ALLOWED_AUDIO_MIME_TYPES->some([=](auto allowed) mutable
            {
                return allowed == file["mimetype"];
            }
            );
            if (isAllowed) {
                cb(nullptr, true);
            } else {
                cb(std::make_shared<Error>(std::string("Invalid audio file type. Only ") + ALLOWED_AUDIO_MIME_TYPES->join(std::string(", ")) + std::string(" are allowed")));
            }
        }
        }
    });
};
std::function<any()> agentMediaUpload = [=]() mutable
{
    return multer(object{
        object::pair{std::string("storage"), std::string("storage")}, 
        object::pair{std::string("limits"), object{
            object::pair{std::string("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std::string("files"), 1}
        }}, 
        object::pair{std::string("fileFilter"), [=](auto _req, auto file, auto cb) mutable
        {
            auto isAllowed = ALLOWED_MEDIA_MIME_TYPES->some([=](auto allowed) mutable
            {
                return allowed == file["mimetype"];
            }
            );
            if (isAllowed) {
                cb(nullptr, true);
            } else {
                cb(std::make_shared<Error>(std::string("Invalid media file type. Only ") + ALLOWED_MEDIA_MIME_TYPES->join(std::string(", ")) + std::string(" are allowed")));
            }
        }
        }
    });
};
std::function<any()> channelUpload = [=]() mutable
{
    return multer(object{
        object::pair{std::string("storage"), std::string("storage")}, 
        object::pair{std::string("limits"), object{
            object::pair{std::string("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std::string("files"), 1}
        }}, 
        object::pair{std::string("fileFilter"), [=](auto _req, auto file, auto cb) mutable
        {
            auto isAllowed = ALLOWED_MEDIA_MIME_TYPES->some([=](auto allowed) mutable
            {
                return allowed == file["mimetype"];
            }
            );
            if (isAllowed) {
                cb(nullptr, true);
            } else {
                cb(std::make_shared<Error>(std::string("Invalid media file type. Only ") + ALLOWED_MEDIA_MIME_TYPES->join(std::string(", ")) + std::string(" are allowed")));
            }
        }
        }
    });
};
std::function<any()> genericUpload = [=]() mutable
{
    return multer(object{
        object::pair{std::string("storage"), std::string("storage")}, 
        object::pair{std::string("limits"), object{
            object::pair{std::string("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std::string("files"), 1}
        }}
    });
};
std::function<any()> upload = genericUpload;

void Main(void)
{
}

MAIN
