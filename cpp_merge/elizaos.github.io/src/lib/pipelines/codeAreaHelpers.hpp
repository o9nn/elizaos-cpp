#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_CODEAREAHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_CODEAREAHELPERS_H
#include "core.hpp"

typedef any WorkItemType;


template <typename RET>
RET categorizeWorkItem(string text);

any extractAreaFromPath(string path);

boolean isRootConfigFile(string path);

std::shared_ptr<Map<string, double>> buildAreaMap(array<object> files);

template <typename RET>
RET categorizeWorkItem(string text)
{
    auto lowercaseText = text->toLowerCase();
    if (OR((OR((lowercaseText->startsWith(std:("feat"))), (lowercaseText->includes(std:("feature"))))), (lowercaseText->includes(std:("add "))))) {
        return std:("feature");
    }
    if (OR((OR((lowercaseText->startsWith(std:("fix"))), (lowercaseText->includes(std:("fix"))))), (lowercaseText->includes(std:("bug"))))) {
        return std:("bugfix");
    }
    if (OR((lowercaseText->startsWith(std:("docs"))), (lowercaseText->includes(std:("document"))))) {
        return std:("docs");
    }
    if (OR((OR((OR((lowercaseText->startsWith(std:("refactor"))), (lowercaseText->includes(std:("refactor"))))), (lowercaseText->includes(std:("clean"))))), (lowercaseText->includes(std:("cleanup"))))) {
        return std:("refactor");
    }
    if (OR((lowercaseText->startsWith(std:("test"))), (lowercaseText->includes(std:("test"))))) {
        return std:("tests");
    }
    return std:("other");
};


#endif
