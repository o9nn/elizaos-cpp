#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/enum/message-regex.h"

std::shared_ptr<RegExp> MessageRegex::BOLD = (new RegExp(std:("\*\*(?<text>[^*]+)(?=(\*\*))\*\*")));

std::shared_ptr<RegExp> MessageRegex::LINK = (new RegExp(std:("(?:(?<name>\[[^\]]+\])(?<url>\([^ )]+)?(?<description>[^[]*(?=(?:'|")\))'\))?)")));

std::shared_ptr<RegExp> MessageRegex::QUOTE = (new RegExp(std:(""(?<text>[^"]+)(?=("))"")));

std::shared_ptr<RegExp> MessageRegex::CHANNEL_MENTION = (new RegExp(std:("<#(?<channel_id>\d+)>")));

std::shared_ptr<RegExp> MessageRegex::HYPER_LINK = (new RegExp(std:("(^|\s)(http(s)?:\/\/)+[^\s]+(?=[\s])?")));

std::shared_ptr<RegExp> MessageRegex::UNDER_LINE = (new RegExp(std:("__(?<text>[^_]+)(?=(__))__")));

std::shared_ptr<RegExp> MessageRegex::ITALICS = (new RegExp(std:("(_|\*)(?<text>[^_*]+)(?=((_|\*)(\s|$)))(_|\*)")));

std::shared_ptr<RegExp> MessageRegex::CODE = (new RegExp(std:(""""(?<text>[^"]+)(?=("""))"""")));

std::shared_ptr<RegExp> MessageRegex::USER_MENTION = (new RegExp(std:("<@(?:&|!)?(?<user_id>[0-9]+)>")));

std::shared_ptr<RegExp> MessageRegex::EMOJI = (new RegExp(std:("<a:[^<>]+:[0-9]+>|<:[^<>]+:[0-9]+>")));

std::shared_ptr<RegExp> MessageRegex::WINDOWS_INVALID_CHARACTERS = (new RegExp(std:("\[|<|>|:|"|\/|\\|\||\?|\*|\]")));

