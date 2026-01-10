#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/media/channels.h"

std::shared_ptr<Promise<object>> saveUploadedFile(std::shared_ptr<Express::Multer::File> file, string channelId)
{
    auto uploadDir = path->join(process->cwd(), std::string(".eliza/data/uploads/channels"), channelId);
    if (!fs->existsSync(uploadDir)) {
        fs->mkdirSync(uploadDir, object{
            object::pair{std::string("recursive"), true}
        });
    }
    auto timestamp = Date->now();
    auto random = Math->round(Math->random() * 1000000000);
    auto ext = path->extname(file->originalname);
    auto filename = string_empty + timestamp + std::string("-") + random + string_empty + ext + string_empty;
    auto filePath = path->join(uploadDir, filename);
    fs->writeFileSync(filePath, file->buffer);
    auto url = std::string("/media/uploads/channels/") + channelId + std::string("/") + filename + string_empty;
    return object{
        object::pair{std::string("filename"), std::string("filename")}, 
        object::pair{std::string("url"), std::string("url")}
    };
};


std::shared_ptr<express::Router> createChannelMediaRouter()
{
    auto router = express->Router();
    auto uploadMediaRateLimiter = rateLimit(object{
        object::pair{std::string("windowMs"), 15 * 60 * 1000}, 
        object::pair{std::string("max"), 100}, 
        object::pair{std::string("message"), object{
            object::pair{std::string("success"), false}, 
            object::pair{std::string("error"), std::string("Too many requests, please try again later.")}
        }}
    });
    router->post(std::string("/:channelId/upload-media"), uploadMediaRateLimiter, upload->single(std::string("file")), [=](auto req, auto res) mutable
    {
        auto channelId = validateUuid(req["params"]["channelId"]);
        if (!channelId) {
            res["status"](400)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Invalid channelId format")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        if (!req["file"]) {
            res["status"](400)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("No media file provided")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        try
        {
            auto result = std::async([=]() { saveUploadedFile(req["file"], channelId); });
            logger->info(std::string("[Channel Media Upload] File uploaded for channel ") + channelId + std::string(": ") + result["filename"] + std::string(". URL: ") + result["url"] + string_empty);
            res["json"](object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("url"), result["url"]}, 
                    object::pair{std::string("type"), req["file"]["mimetype"]}, 
                    object::pair{std::string("filename"), result["filename"]}, 
                    object::pair{std::string("originalName"), req["file"]["originalname"]}, 
                    object::pair{std::string("size"), req["file"]["size"]}
                }}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[Channel Media Upload] Error processing upload for channel ") + channelId + std::string(": ") + error["message"] + string_empty, error);
            res["status"](500)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), std::string("Failed to process media upload")}
            });
        }
    }
    );
    return router;
};


any storage = multer->memoryStorage();
any upload = multer(object{
    object::pair{std::string("storage"), std::string("storage")}, 
    object::pair{std::string("limits"), object{
        object::pair{std::string("fileSize"), MAX_FILE_SIZE}, 
        object::pair{std::string("files"), 1}
    }}, 
    object::pair{std::string("fileFilter"), [=](auto req, auto file, auto cb) mutable
    {
        if (ALLOWED_MEDIA_MIME_TYPES->includes(as<any>(file["mimetype"]))) {
            cb(nullptr, true);
        } else {
            cb(std::make_shared<Error>(std::string("Invalid file type")), false);
        }
    }
    }
});

void Main(void)
{
}

MAIN
