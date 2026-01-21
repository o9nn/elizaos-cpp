#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_PRIMITIVES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_PRIMITIVES_H
#include "core.h"

typedef  UUID;

typedef object Media;

typedef Record<std::string, any> Metadata;

class Content;
enum struct ContentType;

UUID asUUID(std::string id);

class Content : public object, public std::enable_shared_from_this<Content> {
public:
    using std::enable_shared_from_this<Content>::shared_from_this;
    std::string thought;

    std::string text;

    array<string> actions;

    array<string> providers;

    std::string source;

    std::string target;

    std::string url;

    UUID inReplyTo;

    array<Media> attachments;

    std::string channelType;
};

enum struct ContentType {
    IMAGE = std::string("image"), VIDEO = std::string("video"), AUDIO = std::string("audio"), DOCUMENT = std::string("document"), LINK = std::string("link")
};
#endif
