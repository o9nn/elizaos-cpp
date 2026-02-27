#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_ROOMS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_TYPES_ROOMS_H
#include "core.hpp"
// External dependency removed

class Room;

class Room : public object, public std::enable_shared_from_this<Room> {
public:
    using std::enable_shared_from_this<Room>::shared_from_this;
    string id;

    string name;

    std::shared_ptr<ChannelType> type;

    array<object> entities;
};

#endif
