#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_CODEAREAHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_CODEAREAHELPERS_H
#include "core.h"

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
    if (OR((OR((lowercaseText->startsWith(std::string("feat"))), (lowercaseText->includes(std::string("feature"))))), (lowercaseText->includes(std::string("add "))))) {
        return std::string("feature");
    }
    if (OR((OR((lowercaseText->startsWith(std::string("fix"))), (lowercaseText->includes(std::string("fix"))))), (lowercaseText->includes(std::string("bug"))))) {
        return std::string("bugfix");
    }
    if (OR((lowercaseText->startsWith(std::string("docs"))), (lowercaseText->includes(std::string("document"))))) {
        return std::string("docs");
    }
    if (OR((OR((OR((lowercaseText->startsWith(std::string("refactor"))), (lowercaseText->includes(std::string("refactor"))))), (lowercaseText->includes(std::string("clean"))))), (lowercaseText->includes(std::string("cleanup"))))) {
        return std::string("refactor");
    }
    if (OR((lowercaseText->startsWith(std::string("test"))), (lowercaseText->includes(std::string("test"))))) {
        return std::string("tests");
    }
    return std::string("other");
};


#endif
