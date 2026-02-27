#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_TYPES_INDEX_H
#include "core.h"
#include "@elizaos/core.h"

class CharacterModification;
class CharacterSnapshot;
class ModificationOperation;
class CharacterDiff;
class ValidationResult;
class ModificationOptions;

class CharacterModification : public object, public std::enable_shared_from_this<CharacterModification> {
public:
    using std::enable_shared_from_this<CharacterModification>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    double versionNumber;

    string diffXml;

    string reasoning;

    std::shared_ptr<Date> appliedAt;

    std::shared_ptr<Date> rolledBackAt;

    std::shared_ptr<Date> createdAt;
};

class CharacterSnapshot : public object, public std::enable_shared_from_this<CharacterSnapshot> {
public:
    using std::enable_shared_from_this<CharacterSnapshot>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> agentId;

    double versionNumber;

    any characterData;

    std::shared_ptr<Date> createdAt;
};

class ModificationOperation : public object, public std::enable_shared_from_this<ModificationOperation> {
public:
    using std::enable_shared_from_this<ModificationOperation>::shared_from_this;
    any type;

    string path;

    any value;

    string dataType;
};

class CharacterDiff : public object, public std::enable_shared_from_this<CharacterDiff> {
public:
    using std::enable_shared_from_this<CharacterDiff>::shared_from_this;
    array<std::shared_ptr<ModificationOperation>> operations;

    string reasoning;

    string timestamp;
};

class ValidationResult : public object, public std::enable_shared_from_this<ValidationResult> {
public:
    using std::enable_shared_from_this<ValidationResult>::shared_from_this;
    boolean valid;

    array<string> errors;

    array<string> warnings;
};

class ModificationOptions : public object, public std::enable_shared_from_this<ModificationOptions> {
public:
    using std::enable_shared_from_this<ModificationOptions>::shared_from_this;
    array<string> focusAreas;

    double maxChanges;

    boolean preserveCore;
};

#endif
