#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_CHANNEL_USE-CHANNEL-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_CHANNEL_USE-CHANNEL-SLICE_H
#include "core.h"
#include "../../app/store.h"
#include "./channel-slice.h"
using setIsLoadingAction = setIsLoading;
using setChannelsAction = setChannels;
using setChannelAction = setChannel;
using resetChannelAction = resetChannel;
using setSelectedExportChannelsAction = setSelectedExportChannels;
using getChannelsAction = getChannels;
using changeChannelAction = changeChannel;
using loadChannelAction = loadChannel;
#include "../../app/hooks.h"
#include "../../classes/channel.h"
using Channel = _default;

extern std::function<object()> useChannelSlice;
#endif
