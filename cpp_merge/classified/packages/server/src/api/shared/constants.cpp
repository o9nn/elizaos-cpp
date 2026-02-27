#include "constants.hpp"
#include <string>

double MAX_FILE_SIZE = 50 * 1024 * 1024;
string MAX_FILE_SIZE_DISPLAY = std::string("50MB");
any ALLOWED_AUDIO_MIME_TYPES = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ std::string("audio/mpeg"), std::string("audio/mp3"), std::string("audio/wav"), std::string("audio/ogg"), std::string("audio/webm"), std::string("audio/mp4"), std::string("audio/aac"), std::string("audio/flac"), std::string("audio/x-wav"), std::string("audio/wave") });
any ALLOWED_MEDIA_MIME_TYPES = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ ALLOWED_AUDIO_MIME_TYPES, std::string("image/jpeg"), std::string("image/png"), std::string("image/gif"), std::string("image/webp"), std::string("video/mp4"), std::string("video/webm"), std::string("application/pdf"), std::string("text/plain") });

void Main(void)
{
}

MAIN
