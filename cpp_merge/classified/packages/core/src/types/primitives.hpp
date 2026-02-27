#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_PRIMITIVES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_PRIMITIVES_H
#include "core.hpp"
#include <string>

typedef  UUID;

typedef object Media;

typedef Record<string, any> Metadata;

class Content;
enum struct ContentType;

UUID asUUID(string id);

class Content : public object, public std::enable_shared_from_this<Content> {
public:
    using std::enable_shared_from_this<Content>::shared_from_this;
    string thought;

    string text;

    array<string> actions;

    array<string> providers;

    string source;

    string target;

    string url;

    UUID inReplyTo;

    array<Media> attachments;

    string channelType;
};

enum struct ContentType {
    IMAGE = std::string("image"), VIDEO = std::string("video"), AUDIO = std::string("audio"), DOCUMENT = std::string("document"), LINK = std::string("link")
};
#endif
