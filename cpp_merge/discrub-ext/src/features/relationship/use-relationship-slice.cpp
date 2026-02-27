#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/relationship/use-relationship-slice.h"

std::function<object()> useRelationshipSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useIsLoading = template <typename RET>
    [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["relationship"]->isLoading;
        }
        );
    };
    auto useFriends = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["relationship"]->friends;
        }
        );
    };
    auto state = object{
        object::pair{std:("isLoading"), useIsLoading}, 
        object::pair{std:("friends"), useFriends}
    };
    auto setIsLoading = [=](auto value) mutable
    {
        dispatch(setIsLoadingAction(value));
    };
    auto setFriends = [=](auto friends) mutable
    {
        dispatch(setFriendsAction(friends));
    };
    auto getFriends = [=]() mutable
    {
        dispatch(getFriendsAction());
    };
    auto addFriend = [=](auto username, auto discriminator) mutable
    {
        dispatch(addFriendAction(object{
            object::pair{std:("username"), std:("username")}, 
            object::pair{std:("discriminator"), std:("discriminator")}
        }));
    };
    auto deleteFriend = [=](auto userId) mutable
    {
        dispatch(deleteFriendAction(userId));
    };
    auto openDm = [=](auto userId) mutable
    {
        dispatch(openDmAction(userId));
    };
    return object{
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setIsLoading"), std:("setIsLoading")}, 
        object::pair{std:("setFriends"), std:("setFriends")}, 
        object::pair{std:("getFriends"), std:("getFriends")}, 
        object::pair{std:("addFriend"), std:("addFriend")}, 
        object::pair{std:("deleteFriend"), std:("deleteFriend")}, 
        object::pair{std:("openDm"), std:("openDm")}
    };
};

void Main(void)
{
}

MAIN
