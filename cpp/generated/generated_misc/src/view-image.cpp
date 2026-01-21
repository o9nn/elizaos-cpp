#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tools/src/image-tools/view-image.h"

any getMimeType(string filename)
{
    auto ext = path->extname(filename)->toLowerCase();
    static switch_type __switch497_790 = {
        { any(std::string(".png")), 1 },
        { any(std::string(".jpg")), 2 },
        { any(std::string(".jpeg")), 3 },
        { any(std::string(".webp")), 4 },
        { any(std::string(".gif")), 5 },
        { any(std::string(".svg")), 6 }
    };
    switch (__switch497_790[ext])
    {
    case 1:
        return std::string("image/png");
    case 2:
    case 3:
        return std::string("image/jpeg");
    case 4:
        return std::string("image/webp");
    case 5:
        return std::string("image/gif");
    case 6:
        return std::string("image/svg+xml");
    default:
        return nullptr;
    }
};


void viewImage(string imagePath)
{
    if (!fs::existsSync(imagePath)) {
        console->error(std::string("Error: File '") + imagePath + std::string("' does not exist"));
        process->exit(1);
    }
    auto stats = fs::statSync(imagePath);
    if (!stats->isFile()) {
        console->error(std::string("Error: '") + imagePath + std::string("' is not a file"));
        process->exit(1);
    }
    auto mimeType = getMimeType(imagePath);
    if (OR((!mimeType), (!VALID_MIME_TYPES->has(mimeType)))) {
        console->error(std::string("Error: Unsupported image type: ") + mimeType + std::string(". Valid types are: ") + Array->from(VALID_MIME_TYPES)->join(std::string(", ")) + string_empty);
        process->exit(1);
    }
    try
    {
        auto imageBuffer = fs::readFileSync(imagePath);
        auto base64 = imageBuffer->toString(std::string("base64"));
        console->log(std::string("![") + path->basename(imagePath) + std::string("](data:") + mimeType + std::string(";base64,") + base64 + std::string(")"));
    }
    catch (const any& error)
    {
        console->error(std::string("Error processing image: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std::string("view-image"))->description(std::string("View an image file as a base64-encoded markdown image"))->version(std::string("1.0.0"))->argument(std::string("<image-file>"), std::string("Path to the image file to view"))->action([=](auto imagePath) mutable
    {
        viewImage(imagePath);
    }
    );
    program->parse(process->argv);
};


std::shared_ptr<Set<string>> VALID_MIME_TYPES = std::make_shared<Set>(array<string>{ std::string("image/png"), std::string("image/jpeg"), std::string("image/jpg"), std::string("image/webp"), std::string("image/gif"), std::string("image/svg+xml") });

void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std::string("/bin/view_image"))))) {
        setupCLI();
    }
}

MAIN
