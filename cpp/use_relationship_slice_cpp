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
        object::pair{std::string("isLoading"), useIsLoading}, 
        object::pair{std::string("friends"), useFriends}
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
            object::pair{std::string("username"), std::string("username")}, 
            object::pair{std::string("discriminator"), std::string("discriminator")}
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
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setIsLoading"), std::string("setIsLoading")}, 
        object::pair{std::string("setFriends"), std::string("setFriends")}, 
        object::pair{std::string("getFriends"), std::string("getFriends")}, 
        object::pair{std::string("addFriend"), std::string("addFriend")}, 
        object::pair{std::string("deleteFriend"), std::string("deleteFriend")}, 
        object::pair{std::string("openDm"), std::string("openDm")}
    };
};

void Main(void)
{
}

MAIN
