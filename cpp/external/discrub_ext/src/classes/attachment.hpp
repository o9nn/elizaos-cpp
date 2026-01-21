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

// https://discord.com/developers/docs/resources/channel#attachment-object
class Attachment {
  id: Snowflake;
  filename: std::string;
  description?: std::string;
  content_type?: std::string;
  size: number;
  url: std::string;
  proxy_url: std::string;
  height?: number | Maybe;
  width?: number | Maybe;
  ephemeral?: boolean;
  duration_secs?: number;
  waveform?: std::string;
  flags?: number;

  constructor(opts: {
    id: Snowflake;
    filename: std::string;
    description?: std::string;
    content_type?: std::string;
    size: number;
    url: std::string;
    proxy_url: std::string;
    height?: number | Maybe;
    width?: number | Maybe;
    ephemeral?: boolean;
    duration_secs?: number;
    waveform?: std::string;
    flags?: number;
  }) {
    this.id = opts.id;
    this.filename = opts.filename;
    this.description = opts.description;
    this.content_type = opts.content_type;
    this.size = opts.size;
    this.url = opts.url;
    this.proxy_url = opts.proxy_url;
    this.height = opts.height;
    this.width = opts.width;
    this.ephemeral = opts.ephemeral;
    this.duration_secs = opts.duration_secs;
    this.waveform = opts.waveform;
    this.flags = opts.flags;
  }

} // namespace elizaos
