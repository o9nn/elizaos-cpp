#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/classes/embed.h"

Embed::Embed(object opts) {
    this->title = opts["title"];
    *reinterpret_cast<long*>(&this->type) = static_cast<long>(opts["type"]);
    this->description = opts["description"];
    this->url = opts["url"];
    this->timestamp = opts["timestamp"];
    this->color = opts["color"];
    this->footer = opts["footer"];
    this->image = opts["image"];
    this->thumbnail = opts["thumbnail"];
    this->video = opts["video"];
    this->provider = opts["provider"];
    this->author = opts["author"];
    this->fields = opts["fields"];
}

