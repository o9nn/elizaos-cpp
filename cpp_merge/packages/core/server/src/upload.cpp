#include "upload.hpp"

string generateSecureFilename(string originalName)
{
    auto uniqueSuffix = string_empty + Date->now() + std:("-") + Math->round(Math->random() * 1000000000) + string_empty;
    auto sanitizedName = sanitizeFilename(originalName);
    return string_empty + uniqueSuffix + std:("-") + sanitizedName + string_empty;
};


boolean validateAudioFile(std::shared_ptr<Express::Multer::File> file)
{
    return ALLOWED_AUDIO_MIME_TYPES->includes(as<any>(file->mimetype));
};


boolean validateMediaFile(std::shared_ptr<Express::Multer::File> file)
{
    return ALLOWED_MEDIA_MIME_TYPES->includes(as<any>(file->mimetype));
};


any storage = multer->memoryStorage();
std::function<any()> agentAudioUpload = [=]() mutable
{
    return multer(object{
        object::pair{std:("storage"), std:("storage")}, 
        object::pair{std:("limits"), object{
            object::pair{std:("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std:("files"), 1}
        }}, 
        object::pair{std:("fileFilter"), [=](auto req, auto file, auto cb) mutable
        {
            if (ALLOWED_AUDIO_MIME_TYPES->includes(as<any>(file["mimetype"]))) {
                cb(nullptr, true);
            } else {
                cb(std::make_shared<Error>(std:("Invalid audio file type")), false);
            }
        }
        }
    });
};
std::function<any()> agentMediaUpload = [=]() mutable
{
    return multer(object{
        object::pair{std:("storage"), std:("storage")}, 
        object::pair{std:("limits"), object{
            object::pair{std:("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std:("files"), 1}
        }}, 
        object::pair{std:("fileFilter"), [=](auto req, auto file, auto cb) mutable
        {
            if (ALLOWED_MEDIA_MIME_TYPES->includes(as<any>(file["mimetype"]))) {
                cb(nullptr, true);
            } else {
                cb(std::make_shared<Error>(std:("Invalid media file type")), false);
            }
        }
        }
    });
};
std::function<any()> channelUpload = [=]() mutable
{
    return multer(object{
        object::pair{std:("storage"), std:("storage")}, 
        object::pair{std:("limits"), object{
            object::pair{std:("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std:("files"), 1}
        }}, 
        object::pair{std:("fileFilter"), [=](auto req, auto file, auto cb) mutable
        {
            if (ALLOWED_MEDIA_MIME_TYPES->includes(as<any>(file["mimetype"]))) {
                cb(nullptr, true);
            } else {
                cb(std::make_shared<Error>(std:("Invalid file type")), false);
            }
        }
        }
    });
};
std::function<any()> genericUpload = [=]() mutable
{
    return multer(object{
        object::pair{std:("storage"), std:("storage")}, 
        object::pair{std:("limits"), object{
            object::pair{std:("fileSize"), MAX_FILE_SIZE}, 
            object::pair{std:("files"), 1}
        }}
    });
};
std::function<any()> upload = genericUpload;

void Main(void)
{
}

MAIN
