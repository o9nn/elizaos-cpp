#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/shared/constants.h"

double MAX_FILE_SIZE = 50 * 1024 * 1024;
string MAX_FILE_SIZE_DISPLAY = std:("50MB");
any ALLOWED_AUDIO_MIME_TYPES = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ std:("audio/mpeg"), std:("audio/mp3"), std:("audio/wav"), std:("audio/ogg"), std:("audio/webm"), std:("audio/mp4"), std:("audio/aac"), std:("audio/flac"), std:("audio/x-wav"), std:("audio/wave") });
any ALLOWED_MEDIA_MIME_TYPES = as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ ALLOWED_AUDIO_MIME_TYPES, std:("image/jpeg"), std:("image/png"), std:("image/gif"), std:("image/webp"), std:("video/mp4"), std:("video/webm"), std:("application/pdf"), std:("text/plain") });

void Main(void)
{
}

MAIN
