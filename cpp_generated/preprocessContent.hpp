#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_PREPROCESSCONTENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_PREPROCESSCONTENT_H
#include "core.h"
#include "./types.h"
#include "../data/tags.h"
#include "path.h"
using path = _default;

string preprocessFilePathContent(string content);

string preprocessTextContent(string content);

string preprocessCodeContent(string content);

string preprocessLabelContent(string content);

MatchContent preprocessContent(MatchContent content);

#endif
