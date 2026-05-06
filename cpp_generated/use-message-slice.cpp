#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/message/use-message-slice.h"

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
        object::pair{std::string("messages"), useMessages}, 
        object::pair{std::string("selectedMessages"), useSelectedMessages}, 
        object::pair{std::string("filteredMessages"), useFilteredMessages}, 
        object::pair{std::string("filters"), useFilters}, 
        object::pair{std::string("isLoading"), useIsLoading}, 
        object::pair{std::string("order"), useOrder}, 
        object::pair{std::string("orderBy"), useOrderBy}, 
        object::pair{std::string("searchBeforeDate"), useSearchBeforeDate}, 
        object::pair{std::string("searchAfterDate"), useSearchAfterDate}, 
        object::pair{std::string("searchMessageContent"), useSearchMessageContent}, 
        object::pair{std::string("selectedHasTypes"), useSelectedHasTypes}
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
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setIsLoading"), std::string("setIsLoading")}, 
        object::pair{std::string("setSelectedHasTypes"), std::string("setSelectedHasTypes")}, 
        object::pair{std::string("setSearchMessageContent"), std::string("setSearchMessageContent")}, 
        object::pair{std::string("setSearchBeforeDate"), std::string("setSearchBeforeDate")}, 
        object::pair{std::string("setSearchAfterDate"), std::string("setSearchAfterDate")}, 
        object::pair{std::string("setSelected"), std::string("setSelected")}, 
        object::pair{std::string("setOrder"), std::string("setOrder")}, 
        object::pair{std::string("setMessages"), std::string("setMessages")}, 
        object::pair{std::string("setFilteredMessages"), std::string("setFilteredMessages")}, 
        object::pair{std::string("resetFilters"), std::string("resetFilters")}, 
        object::pair{std::string("resetAdvancedFilters"), std::string("resetAdvancedFilters")}, 
        object::pair{std::string("updateFilters"), std::string("updateFilters")}, 
        object::pair{std::string("filterMessages"), std::string("filterMessages")}, 
        object::pair{std::string("deleteAttachment"), std::string("deleteAttachment")}, 
        object::pair{std::string("updateMessage"), std::string("updateMessage")}, 
        object::pair{std::string("editMessages"), std::string("editMessages")}, 
        object::pair{std::string("deleteMessage"), std::string("deleteMessage")}, 
        object::pair{std::string("deleteMessages"), std::string("deleteMessages")}, 
        object::pair{std::string("getMessageData"), std::string("getMessageData")}, 
        object::pair{std::string("resetMessageData"), std::string("resetMessageData")}, 
        object::pair{std::string("deleteReaction"), std::string("deleteReaction")}
    };
};

void Main(void)
{
}

MAIN
