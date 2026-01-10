#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/classes/attachment.h"

Attachment::Attachment(object opts) {
    this->id = opts["id"];
    this->filename = opts["filename"];
    this->description = opts["description"];
    this->content_type = opts["content_type"];
    this->size = opts["size"];
    this->url = opts["url"];
    this->proxy_url = opts["proxy_url"];
    this->height = opts["height"];
    this->width = opts["width"];
    this->ephemeral = opts["ephemeral"];
    this->duration_secs = opts["duration_secs"];
    this->waveform = opts["waveform"];
    this->flags = opts["flags"];
}

