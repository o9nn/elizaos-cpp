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

class MessageRegex {
  public static readonly BOLD = /\*\*(?<text>[^*]+)(?=(\*\*))\*\*/g;
  public static readonly LINK =
    /(?:(?<name>\[[^\]]+\])(?<url>\([^ )]+)?(?<description>[^[]*(?=(?:'|")\))'\))?)/g;
  public static readonly QUOTE = /`(?<text>[^`]+)(?=(`))`/g;
  public static readonly CHANNEL_MENTION = /<#(?<channel_id>\d+)>/g;
  public static readonly HYPER_LINK = /(^|\s)(http(s)?:\/\/)+[^\s]+(?=[\s])?/g;
  public static readonly UNDER_LINE = /__(?<text>[^_]+)(?=(__))__/g;
  public static readonly ITALICS =
    /(_|\*)(?<text>[^_*]+)(?=((_|\*)(\s|$)))(_|\*)/g;
  public static readonly CODE = /```(?<text>[^`]+)(?=(```))```/g;
  public static readonly USER_MENTION = /<@(?:&|!)?(?<user_id>[0-9]+)>/g;
  public static readonly EMOJI = /<a:[^<>]+:[0-9]+>|<:[^<>]+:[0-9]+>/g;
  public static readonly WINDOWS_INVALID_CHARACTERS =
    /\[|<|>|:|"|\/|\\|\||\?|\*|\]/g;
}

} // namespace elizaos
