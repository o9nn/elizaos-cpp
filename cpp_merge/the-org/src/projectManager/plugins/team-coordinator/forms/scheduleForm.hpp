#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_FORMS_SCHEDULEFORM_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_FORMS_SCHEDULEFORM_H
#include "core.hpp"
// External dependency removed
// External dependency removed

class DiscordComponent;
class DiscordActionRow;
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

std::shared_ptr<Promise<void>> sendCheckInScheduleForm(std::shared_ptr<HandlerCallback> callback, array<object> channels = std::nullopt);

#endif
