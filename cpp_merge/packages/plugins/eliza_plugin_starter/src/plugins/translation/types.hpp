#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_TRANSLATION_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PLUGIN_STARTER_SRC_PLUGINS_TRANSLATION_TYPES_H
#include "core.hpp"
// External dependency removed
using ElizaServiceType = ServiceType;

class TranslationConfig;
class TranslationData;
class TranslationActionContent;
class TranslationEvalContent;
class TranslationEvalResponse;
class TranslationProviderResponse;
class TranslationServiceConfig;
class TranslationService;
class TranslationServiceResponse;

class TranslationConfig : public object, public std::enable_shared_from_this<TranslationConfig> {
public:
    using std::enable_shared_from_this<TranslationConfig>::shared_from_this;
    object provider;
};

class TranslationData : public object, public std::enable_shared_from_this<TranslationData> {
public:
    using std::enable_shared_from_this<TranslationData>::shared_from_this;
    string sourceText;

    string targetText;

    string sourceLang;

    string targetLang;

    double confidence;
};

class TranslationActionContent : public Content, public std::enable_shared_from_this<TranslationActionContent> {
public:
    using std::enable_shared_from_this<TranslationActionContent>::shared_from_this;
    string text;
};

class TranslationEvalContent : public Content, public std::enable_shared_from_this<TranslationEvalContent> {
public:
    using std::enable_shared_from_this<TranslationEvalContent>::shared_from_this;
    string text;
};

class TranslationEvalResponse : public object, public std::enable_shared_from_this<TranslationEvalResponse> {
public:
    using std::enable_shared_from_this<TranslationEvalResponse>::shared_from_this;
    boolean success;

    string response;

    double confidence;
};

class TranslationProviderResponse : public object, public std::enable_shared_from_this<TranslationProviderResponse> {
public:
    using std::enable_shared_from_this<TranslationProviderResponse>::shared_from_this;
    boolean success;

    std::shared_ptr<TranslationData> data;

    string error;
};

class TranslationServiceConfig : public object, public std::enable_shared_from_this<TranslationServiceConfig> {
public:
    using std::enable_shared_from_this<TranslationServiceConfig>::shared_from_this;
    string model;

    double temperature;

    double maxTokens;
};

class TranslationService : public Service, public std::enable_shared_from_this<TranslationService> {
public:
    using std::enable_shared_from_this<TranslationService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<TranslationServiceResponse>>> translate(string text, string targetLang, string sourceLang = undefined) = 0;
};

class TranslationServiceResponse : public object, public std::enable_shared_from_this<TranslationServiceResponse> {
public:
    using std::enable_shared_from_this<TranslationServiceResponse>::shared_from_this;
    boolean success;

    string translation;

    string error;

    object metadata;
};

#endif
