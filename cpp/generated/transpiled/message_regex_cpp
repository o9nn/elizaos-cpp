#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/enum/message-regex.h"

std::shared_ptr<RegExp> MessageRegex::BOLD = (new RegExp(std::string("\*\*(?<text>[^*]+)(?=(\*\*))\*\*")));

std::shared_ptr<RegExp> MessageRegex::LINK = (new RegExp(std::string("(?:(?<name>\[[^\]]+\])(?<url>\([^ )]+)?(?<description>[^[]*(?=(?:'|")\))'\))?)")));

std::shared_ptr<RegExp> MessageRegex::QUOTE = (new RegExp(std::string("`(?<text>[^`]+)(?=(`))`")));

std::shared_ptr<RegExp> MessageRegex::CHANNEL_MENTION = (new RegExp(std::string("<#(?<channel_id>\d+)>")));

std::shared_ptr<RegExp> MessageRegex::HYPER_LINK = (new RegExp(std::string("(^|\s)(http(s)?:\/\/)+[^\s]+(?=[\s])?")));

std::shared_ptr<RegExp> MessageRegex::UNDER_LINE = (new RegExp(std::string("__(?<text>[^_]+)(?=(__))__")));

std::shared_ptr<RegExp> MessageRegex::ITALICS = (new RegExp(std::string("(_|\*)(?<text>[^_*]+)(?=((_|\*)(\s|$)))(_|\*)")));

std::shared_ptr<RegExp> MessageRegex::CODE = (new RegExp(std::string("```(?<text>[^`]+)(?=(```))```")));

std::shared_ptr<RegExp> MessageRegex::USER_MENTION = (new RegExp(std::string("<@(?:&|!)?(?<user_id>[0-9]+)>")));

std::shared_ptr<RegExp> MessageRegex::EMOJI = (new RegExp(std::string("<a:[^<>]+:[0-9]+>|<:[^<>]+:[0-9]+>")));

std::shared_ptr<RegExp> MessageRegex::WINDOWS_INVALID_CHARACTERS = (new RegExp(std::string("\[|<|>|:|"|\/|\\|\||\?|\*|\]")));

