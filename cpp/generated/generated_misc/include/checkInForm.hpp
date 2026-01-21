#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_FORMS_CHECKINFORM_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_FORMS_CHECKINFORM_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"

class DiscordComponent;
class DiscordActionRow;
class DiscordTextInput;
class DiscordSelectMenu;
class DiscordButton;

class DiscordComponent : public object, public std::enable_shared_from_this<DiscordComponent> {
public:
    using std::enable_shared_from_this<DiscordComponent>::shared_from_this;
    double type;
};

class DiscordActionRow : public DiscordComponent, public std::enable_shared_from_this<DiscordActionRow> {
public:
    using std::enable_shared_from_this<DiscordActionRow>::shared_from_this;
    double type;

    array<std::shared_ptr<DiscordComponent>> components;
};

class DiscordTextInput : public DiscordComponent, public std::enable_shared_from_this<DiscordTextInput> {
public:
    using std::enable_shared_from_this<DiscordTextInput>::shared_from_this;
    double type;

    string custom_id;

    string value;

    double style;
};

class DiscordSelectMenu : public DiscordComponent, public std::enable_shared_from_this<DiscordSelectMenu> {
public:
    using std::enable_shared_from_this<DiscordSelectMenu>::shared_from_this;
    double type;

    string custom_id;

    string placeholder;

    array<object> options;

    boolean required;
};

class DiscordButton : public DiscordComponent, public std::enable_shared_from_this<DiscordButton> {
public:
    using std::enable_shared_from_this<DiscordButton>::shared_from_this;
    double type;

    double style;

    string custom_id;

    string label;
};

std::shared_ptr<Promise<void>> sendCheckInReportForm(std::shared_ptr<HandlerCallback> callback, array<object> channels = undefined, object serverInfo = undefined);

#endif
