#include "view-image.h"

any getMimeType(string filename)
{
    auto ext = path->extname(filename)->toLowerCase();
    static switch_type __switch497_790 = {
        { any(std:(".png")), 1 },
        { any(std:(".jpg")), 2 },
        { any(std:(".jpeg")), 3 },
        { any(std:(".webp")), 4 },
        { any(std:(".gif")), 5 },
        { any(std:(".svg")), 6 }
    };
    switch (__switch497_790[ext])
    {
    case 1:
        return std:("image/png");
    case 2:
    case 3:
        return std:("image/jpeg");
    case 4:
        return std:("image/webp");
    case 5:
        return std:("image/gif");
    case 6:
        return std:("image/svg+xml");
    default:
        return nullptr;
    }
};


void viewImage(string imagePath)
{
    if (!fs::existsSync(imagePath)) {
        console->error(std:("Error: File '") + imagePath + std:("' does not exist"));
        process->exit(1);
    }
    auto stats = fs::statSync(imagePath);
    if (!stats->isFile()) {
        console->error(std:("Error: '") + imagePath + std:("' is not a file"));
        process->exit(1);
    }
    auto mimeType = getMimeType(imagePath);
    if (OR((!mimeType), (!VALID_MIME_TYPES->has(mimeType)))) {
        console->error(std:("Error: Unsupported image type: ") + mimeType + std:(". Valid types are: ") + Array->from(VALID_MIME_TYPES)->join(std:(", ")) + string_empty);
        process->exit(1);
    }
    try
    {
        auto imageBuffer = fs::readFileSync(imagePath);
        auto base64 = imageBuffer->toString(std:("base64"));
        console->log(std:("![") + path->basename(imagePath) + std:("](data:") + mimeType + std:(";base64,") + base64 + std:(")"));
    }
    catch (const any& error)
    {
        console->error(std:("Error processing image: ") + error + string_empty);
        process->exit(1);
    }
};


void setupCLI()
{
    program->name(std:("view-image"))->description(std:("View an image file as a base64-encoded markdown image"))->version(std:("1.0.0"))->argument(std:("<image-file>"), std:("Path to the image file to view"))->action([=](auto imagePath) mutable
    {
        viewImage(imagePath);
    }
    );
    program->parse(process->argv);
};


std::shared_ptr<Set<string>> VALID_MIME_TYPES = std::make_shared<Set>(array<string>{ std:("image/png"), std:("image/jpeg"), std:("image/jpg"), std:("image/webp"), std:("image/gif"), std:("image/svg+xml") });

void Main(void)
{
    if (OR((require->main == module), (require->main->filename->endsWith(std:("/bin/view_image"))))) {
        setupCLI();
    }
}

MAIN
