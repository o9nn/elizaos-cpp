#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_EMBED_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_CLASSES_EMBED_H
#include "core.h"
#include "../types/embed-footer-object.h"
#include "../types/embed-image-object.h"
#include "../types/embed-thumbnail-object.h"
#include "../types/embed-video-object.h"
#include "../types/embed-provider-object.h"
#include "../types/embed-author-object.h"
#include "../types/embed-field-object.h"
#include "../enum/embed-type.h"

class Embed;

class Embed : public object, public std::enable_shared_from_this<Embed> {
public:
    using std::enable_shared_from_this<Embed>::shared_from_this;
    string title;

    EmbedType type;

    string description;

    string url;

    string timestamp;

    double color;

    EmbedFooterObject footer;

    EmbedImageObject image;

    EmbedThumbnailObject thumbnail;

    EmbedVideoObject video;

    EmbedProviderObject provider;

    EmbedAuthorObject author;

    array<EmbedFieldObject> fields;

    Embed(object opts);
};

#endif
