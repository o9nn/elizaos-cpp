#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_MESSAGE_USE-MESSAGE-SLICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_FEATURES_MESSAGE_USE-MESSAGE-SLICE_H
#include "core.h"
#include "../../app/store.h"
#include "./message-slice.h"
using setIsLoadingAction = setIsLoading;
using setSelectedHasTypesAction = setSelectedHasTypes;
using setSearchMessageContentAction = setSearchMessageContent;
using setSearchBeforeDateAction = setSearchBeforeDate;
using setSearchAfterDateAction = setSearchAfterDate;
using setSelectedAction = setSelected;
using setOrderAction = setOrder;
using setMessagesAction = setMessages;
using setFilteredMessagesAction = setFilteredMessages;
using resetFiltersAction = resetFilters;
using resetAdvancedFiltersAction = resetAdvancedFilters;
using updateFiltersAction = updateFilters;
using filterMessagesAction = filterMessages;
using deleteAttachmentAction = deleteAttachment;
using updateMessageAction = updateMessage;
using editMessagesAction = editMessages;
using deleteMessageAction = deleteMessage;
using deleteMessagesAction = deleteMessages;
using getMessageDataAction = getMessageData;
using resetMessageDataAction = resetMessageData;
using deleteReactionAction = deleteReaction;
#include "../../app/hooks.h"
#include "./message-types.h"
#include "../../enum/has-type.h"
#include "../../enum/sort-direction.h"
#include "../../classes/message.h"
using Message = _default;
#include "../../classes/attachment.h"
using Attachment = _default;

extern std::function<object()> useMessageSlice;
#endif
