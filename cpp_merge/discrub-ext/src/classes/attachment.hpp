#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_CLASSES_ATTACHMENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_CLASSES_ATTACHMENT_H
#include "core.hpp"

class Attachment;

class Attachment : public object, public std::enable_shared_from_this<Attachment> {
public:
    using std::enable_shared_from_this<Attachment>::shared_from_this;
    std::shared_ptr<Snowflake> id;

    string filename;

    string description;

    string content_type;

    double size;

    string url;

    string proxy_url;

    any height;

    any width;

    boolean ephemeral;

    double duration_secs;

    string waveform;

    double flags;

    Attachment(object opts);
};

#endif
