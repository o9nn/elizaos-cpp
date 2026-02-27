#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_MEDIA_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_MEDIA_H
#include "core.hpp"
// External dependency removed

class MediaUploadParams;
class MediaUploadResponse;
class ChannelUploadResponse;

class MediaUploadParams : public object, public std::enable_shared_from_this<MediaUploadParams> {
public:
    using std::enable_shared_from_this<MediaUploadParams>::shared_from_this;
    any file;

    string filename;

    string contentType;

    Record<string, any> metadata;
};

class MediaUploadResponse : public object, public std::enable_shared_from_this<MediaUploadResponse> {
public:
    using std::enable_shared_from_this<MediaUploadResponse>::shared_from_this;
    string url;

    string type;

    string filename;

    string originalName;

    double size;
};

class ChannelUploadResponse : public object, public std::enable_shared_from_this<ChannelUploadResponse> {
public:
    using std::enable_shared_from_this<ChannelUploadResponse>::shared_from_this;
    string url;

    string type;

    string filename;

    string originalName;

    double size;
};

#endif
