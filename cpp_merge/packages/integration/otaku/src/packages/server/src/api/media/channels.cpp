#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/media/channels.h"

std::shared_ptr<Promise<object>> saveUploadedFile(std::shared_ptr<Express::Multer::File> file, string channelId)
{
    auto uploadDir = path->join(getUploadsChannelsDir(), channelId);
    if (!fs->existsSync(uploadDir)) {
        fs->mkdirSync(uploadDir, object{
            object::pair{std:("recursive"), true}
        });
    }
    auto timestamp = Date->now();
    auto random = Math->round(Math->random() * 1000000000);
    auto ext = path->extname(file->originalname);
    auto filename = string_empty + timestamp + std:("-") + random + string_empty + ext + string_empty;
    auto filePath = path->join(uploadDir, filename);
    fs->writeFileSync(filePath, file->buffer);
    auto url = std:("/media/uploads/channels/") + channelId + std:("/") + filename + string_empty;
    return object{
        object::pair{std:("filename"), std:("filename")}, 
        object::pair{std:("url"), std:("url")}
    };
};


std::shared_ptr<express::Router> createChannelMediaRouter()
{
    auto router = express->Router();
    auto uploadMediaRateLimiter = rateLimit(object{
        object::pair{std:("windowMs"), 15 * 60 * 1000}, 
        object::pair{std:("max"), 100}, 
        object::pair{std:("message"), object{
            object::pair{std:("success"), false}, 
            object::pair{std:("error"), std:("Too many requests, please try again later.")}
        }}
    });
    router->post(std:("/:channelId/upload-media"), uploadMediaRateLimiter, upload->single(std:("file")), [=](auto req, auto res) mutable
    {
        auto channelId = validateUuid(req["params"]["channelId"]);
        if (!channelId) {
            res["status"](400)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Invalid channelId format")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        if (!req["file"]) {
            res["status"](400)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("No media file provided")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        try
        {
            auto result = std::async([=]() { saveUploadedFile(req["file"], channelId); });
            logger->info(std:("[Channel Media Upload] File uploaded for channel ") + channelId + std:(": ") + result["filename"] + std:(". URL: ") + result["url"] + string_empty);
            res["json"](object{
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("url"), result["url"]}, 
                    object::pair{std:("type"), req["file"]["mimetype"]}, 
                    object::pair{std:("filename"), result["filename"]}, 
                    object::pair{std:("originalName"), req["file"]["originalname"]}, 
                    object::pair{std:("size"), req["file"]["size"]}
                }}
            });
        }
        catch (const any& error)
        {
            logger->error(std:("[Channel Media Upload] Error processing upload for channel ") + channelId + std:(": ") + error["message"] + string_empty, error);
            res["status"](500)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), std:("Failed to process media upload")}
            });
        }
    }
    );
    return router;
};


any storage = multer->memoryStorage();
any upload = multer(object{
    object::pair{std:("storage"), std:("storage")}, 
    object::pair{std:("limits"), object{
        object::pair{std:("fileSize"), MAX_FILE_SIZE}, 
        object::pair{std:("files"), 1}
    }}, 
    object::pair{std:("fileFilter"), [=](auto _req, auto file, auto cb) mutable
    {
        auto isAllowed = ALLOWED_MEDIA_MIME_TYPES->some([=](auto allowed) mutable
        {
            return allowed == file->mimetype;
        }
        );
        if (isAllowed) {
            cb(nullptr, true);
        } else {
            cb(std::make_shared<Error>(std:("Invalid file type. Only ") + ALLOWED_MEDIA_MIME_TYPES->join(std:(", ")) + std:(" are allowed")));
        }
    }
    }
});

void Main(void)
{
}

MAIN
