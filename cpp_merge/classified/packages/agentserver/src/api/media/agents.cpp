#include "agents.hpp"
#include <string>

std::shared_ptr<Promise<object>> saveUploadedFile(std::shared_ptr<Express::Multer::File> file, string agentId)
{
    auto uploadDir = path->join(process->cwd(), std::string(".eliza/data/uploads/agents"), agentId);
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
    auto url = std::string("/media/uploads/agents/") + agentId + std::string("/") + filename + string_empty;
    return object{
        object::pair{std::string("filename"), std::string("filename")}, 
        object::pair{std::string("url"), std::string("url")}
    };
};


std::shared_ptr<express::Router> createAgentMediaRouter()
{
    auto router = express->Router();
    router->post(std::string("/:agentId/upload-media"), upload->single(std::string("file")), [=](auto req, auto res) mutable
    {
        logger->debug(std::string("[MEDIA UPLOAD] Processing media upload with multer"));
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        if (!req["file"]) {
            return sendError(res, 400, std::string("INVALID_REQUEST"), std::string("No media file provided"));
        }
        auto mediaType = getContentTypeFromMimeType(req["file"]["mimetype"]);
        if (!mediaType) {
            return sendError(res, 400, std::string("UNSUPPORTED_MEDIA_TYPE"), std::string("Unsupported media MIME type: ") + req["file"]["mimetype"] + string_empty);
        }
        try
        {
            auto result = std::async([=]() { saveUploadedFile(req["file"], agentId); });
            logger->info(std::string("[MEDIA UPLOAD] Successfully uploaded ") + mediaType + std::string(": ") + result["filename"] + std::string(". URL: ") + result["url"] + string_empty);
            sendSuccess(res, object{
                object::pair{std::string("url"), result["url"]}, 
                object::pair{std::string("type"), mediaType}, 
                object::pair{std::string("filename"), result["filename"]}, 
                object::pair{std::string("originalName"), req["file"]["originalname"]}, 
                object::pair{std::string("size"), req["file"]["size"]}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[MEDIA UPLOAD] Error processing upload: ") + error + string_empty);
            sendError(res, 500, std::string("UPLOAD_ERROR"), std::string("Failed to process media upload"), (is<Error>(error)) ? error->message : String(error));
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
