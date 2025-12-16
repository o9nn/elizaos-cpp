#include ".enum/embed-type.hpp"
#include ".types/embed-author-object.hpp"
#include ".types/embed-field-object.hpp"
#include ".types/embed-footer-object.hpp"
#include ".types/embed-image-object.hpp"
#include ".types/embed-provider-object.hpp"
#include ".types/embed-thumbnail-object.hpp"
#include ".types/embed-video-object.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// https://discord.com/developers/docs/resources/channel#embed-object
;
;
;
;
;
;
;
;

class Embed {
  title?: string;
  type?: EmbedType;
  description?: string;
  url?: string;
  timestamp?: string;
  color?: number;
  footer?: EmbedFooterObject;
  image?: EmbedImageObject;
  thumbnail?: EmbedThumbnailObject;
  video?: EmbedVideoObject;
  provider?: EmbedProviderObject;
  author?: EmbedAuthorObject;
  fields?: EmbedFieldObject[];

  constructor(opts: {
    title?: string;
    type?: EmbedType;
    description?: string;
    url?: string;
    timestamp?: string;
    color?: number;
    footer?: EmbedFooterObject;
    image?: EmbedImageObject;
    thumbnail?: EmbedThumbnailObject;
    video?: EmbedVideoObject;
    provider?: EmbedProviderObject;
    author?: EmbedAuthorObject;
    fields?: EmbedFieldObject[];
  }) {
    this.title = opts.title;
    this.type = opts.type;
    this.description = opts.description;
    this.url = opts.url;
    this.timestamp = opts.timestamp;
    this.color = opts.color;
    this.footer = opts.footer;
    this.image = opts.image;
    this.thumbnail = opts.thumbnail;
    this.video = opts.video;
    this.provider = opts.provider;
    this.author = opts.author;
    this.fields = opts.fields;
  }
}

default Embed;

} // namespace elizaos
