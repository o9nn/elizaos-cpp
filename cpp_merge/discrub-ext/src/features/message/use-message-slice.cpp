#include "use-message-slice.h"

std::function<object()> useMessageSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useMessages = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->messages;
        }
        );
    };
    auto useSelectedMessages = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->selectedMessages;
        }
        );
    };
    auto useFilteredMessages = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->filteredMessages;
        }
        );
    };
    auto useFilters = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->filters;
        }
        );
    };
    auto useIsLoading = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->isLoading;
        }
        );
    };
    auto useOrder = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->order;
        }
        );
    };
    auto useOrderBy = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->orderBy;
        }
        );
    };
    auto useSearchBeforeDate = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->searchBeforeDate;
        }
        );
    };
    auto useSearchAfterDate = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->searchAfterDate;
        }
        );
    };
    auto useSearchMessageContent = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->searchMessageContent;
        }
        );
    };
    auto useSelectedHasTypes = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["message"]->selectedHasTypes;
        }
        );
    };
    auto state = object{
        object::pair{std:("messages"), useMessages}, 
        object::pair{std:("selectedMessages"), useSelectedMessages}, 
        object::pair{std:("filteredMessages"), useFilteredMessages}, 
        object::pair{std:("filters"), useFilters}, 
        object::pair{std:("isLoading"), useIsLoading}, 
        object::pair{std:("order"), useOrder}, 
        object::pair{std:("orderBy"), useOrderBy}, 
        object::pair{std:("searchBeforeDate"), useSearchBeforeDate}, 
        object::pair{std:("searchAfterDate"), useSearchAfterDate}, 
        object::pair{std:("searchMessageContent"), useSearchMessageContent}, 
        object::pair{std:("selectedHasTypes"), useSelectedHasTypes}
    };
    auto setIsLoading = [=](auto value) mutable
    {
        dispatch(setIsLoadingAction(value));
    };
    auto setSelectedHasTypes = [=](auto hasTypes) mutable
    {
        dispatch(setSelectedHasTypesAction(hasTypes));
    };
    auto setSearchMessageContent = [=](P0 content) mutable
    {
        dispatch(setSearchMessageContentAction(content));
    };
    auto setSearchBeforeDate = [=](P0 date) mutable
    {
        dispatch(setSearchBeforeDateAction(date));
    };
    auto setSearchAfterDate = [=](P0 date) mutable
    {
        dispatch(setSearchAfterDateAction(date));
    };
    auto setSelected = [=](auto messageIds) mutable
    {
        dispatch(setSelectedAction(messageIds));
    };
    auto setOrder = [=](auto orderProps) mutable
    {
        dispatch(setOrderAction(orderProps));
    };
    auto setMessages = [=](auto messages) mutable
    {
        dispatch(setMessagesAction(messages));
    };
    auto setFilteredMessages = [=](auto messages) mutable
    {
        dispatch(setFilteredMessagesAction(messages));
    };
    auto resetFilters = [=]() mutable
    {
        dispatch(resetFiltersAction());
    };
    auto resetAdvancedFilters = [=]() mutable
    {
        dispatch(resetAdvancedFiltersAction());
    };
    auto updateFilters = [=](P0 filter) mutable
    {
        dispatch(updateFiltersAction(filter));
    };
    auto filterMessages = [=]() mutable
    {
        dispatch(filterMessagesAction());
    };
    auto deleteAttachment = [=](auto attachment) mutable
    {
        dispatch(deleteAttachmentAction(attachment));
    };
    auto updateMessage = [=](auto message) mutable
    {
        dispatch(updateMessageAction(message));
    };
    auto editMessages = [=](auto messages, auto text) mutable
    {
        dispatch(editMessagesAction(messages, text));
    };
    auto deleteMessage = [=](auto message) mutable
    {
        dispatch(deleteMessageAction(message));
    };
    auto deleteReaction = [=](auto channelId, auto messageId, auto emoji) mutable
    {
        dispatch(deleteReactionAction(channelId, messageId, emoji));
    };
    auto deleteMessages = [=](auto messages, auto deleteConfig = undefined) mutable
    {
        dispatch(deleteMessagesAction(messages, deleteConfig));
    };
    auto getMessageData = [=](P0 guildId, P1 channelId, auto options = object{}) mutable
    {
        return dispatch(getMessageDataAction(guildId, channelId, options));
    };
    auto resetMessageData = [=]() mutable
    {
        dispatch(resetMessageDataAction());
    };
    return object{
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setIsLoading"), std:("setIsLoading")}, 
        object::pair{std:("setSelectedHasTypes"), std:("setSelectedHasTypes")}, 
        object::pair{std:("setSearchMessageContent"), std:("setSearchMessageContent")}, 
        object::pair{std:("setSearchBeforeDate"), std:("setSearchBeforeDate")}, 
        object::pair{std:("setSearchAfterDate"), std:("setSearchAfterDate")}, 
        object::pair{std:("setSelected"), std:("setSelected")}, 
        object::pair{std:("setOrder"), std:("setOrder")}, 
        object::pair{std:("setMessages"), std:("setMessages")}, 
        object::pair{std:("setFilteredMessages"), std:("setFilteredMessages")}, 
        object::pair{std:("resetFilters"), std:("resetFilters")}, 
        object::pair{std:("resetAdvancedFilters"), std:("resetAdvancedFilters")}, 
        object::pair{std:("updateFilters"), std:("updateFilters")}, 
        object::pair{std:("filterMessages"), std:("filterMessages")}, 
        object::pair{std:("deleteAttachment"), std:("deleteAttachment")}, 
        object::pair{std:("updateMessage"), std:("updateMessage")}, 
        object::pair{std:("editMessages"), std:("editMessages")}, 
        object::pair{std:("deleteMessage"), std:("deleteMessage")}, 
        object::pair{std:("deleteMessages"), std:("deleteMessages")}, 
        object::pair{std:("getMessageData"), std:("getMessageData")}, 
        object::pair{std:("resetMessageData"), std:("resetMessageData")}, 
        object::pair{std:("deleteReaction"), std:("deleteReaction")}
    };
};

void Main(void)
{
}

MAIN
