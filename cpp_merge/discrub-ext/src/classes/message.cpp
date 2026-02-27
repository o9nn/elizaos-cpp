#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/classes/message.h"

Message::Message(object opts) {
    this->id = opts["id"];
    this->call = opts["call"];
    this->channel_id = opts["channel_id"];
    this->author = opts["author"];
    this->content = opts["content"];
    this->timestamp = opts["timestamp"];
    this->edited_timestamp = opts["edited_timestamp"];
    this->tts = opts["tts"];
    this->mention_everyone = opts["mention_everyone"];
    this->mentions = opts["mentions"];
    this->mention_channels = opts["mention_channels"];
    this->attachments = opts["attachments"];
    this->embeds = opts["embeds"];
    this->reactions = opts["reactions"];
    this->nonce = opts["nonce"];
    this->pinned = opts["pinned"];
    this->webhook_id = opts["webhook_id"];
    this->type = opts["type"];
    this->activity = opts["activity"];
    this->application = opts["application"];
    this->application_id = opts["application_id"];
    this->message_reference = opts["message_reference"];
    this->flags = opts["flags"];
    this->referenced_message = opts["referenced_message"];
    this->interaction = opts["interaction"];
    this->thread = opts["thread"];
    this->components = opts["components"];
    this->sticker_items = opts["sticker_items"];
    this->stickers = opts["stickers"];
    this->position = opts["position"];
    this->role_subscription_data = opts["role_subscription_data"];
    this->resolved = opts["resolved"];
    this->userName = opts["author"]->username;
}

