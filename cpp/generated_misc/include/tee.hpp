#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_TEE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_TYPES_TEE_H
#include "core.h"
#include "./primitives.h"

class TeeAgent;
enum struct TEEMode;
class RemoteAttestationQuote;
class DeriveKeyAttestationData;
class RemoteAttestationMessage;
enum struct TeeType;
class TeePluginConfig;

class TeeAgent : public object, public std::enable_shared_from_this<TeeAgent> {
public:
    using std::enable_shared_from_this<TeeAgent>::shared_from_this;
    string id;

    string agentId;

    string agentName;

    double createdAt;

    string publicKey;

    string attestation;
};

enum struct TEEMode {
    OFF = std::string("OFF"), LOCAL = std::string("LOCAL"), DOCKER = std::string("DOCKER"), PRODUCTION = std::string("PRODUCTION")
};
class RemoteAttestationQuote : public object, public std::enable_shared_from_this<RemoteAttestationQuote> {
public:
    using std::enable_shared_from_this<RemoteAttestationQuote>::shared_from_this;
    string quote;

    double timestamp;
};

class DeriveKeyAttestationData : public object, public std::enable_shared_from_this<DeriveKeyAttestationData> {
public:
    using std::enable_shared_from_this<DeriveKeyAttestationData>::shared_from_this;
    string agentId;

    string publicKey;

    string subject;
};

class RemoteAttestationMessage : public object, public std::enable_shared_from_this<RemoteAttestationMessage> {
public:
    using std::enable_shared_from_this<RemoteAttestationMessage>::shared_from_this;
    string agentId;

    double timestamp;

    object message;
};

enum struct TeeType {
    TDX_DSTACK = std::string("tdx_dstack")
};
class TeePluginConfig : public object, public std::enable_shared_from_this<TeePluginConfig> {
public:
    using std::enable_shared_from_this<TeePluginConfig>::shared_from_this;
    string vendor;

    Metadata vendorConfig;
};

#endif
