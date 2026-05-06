#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/features/app/use-app-slice.h"

std::function<object()> useAppSlice = [=]() mutable
{
    shared dispatch = useAppDispatch();
    auto useDiscrubCancelled = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["app"]->discrubCancelled;
        }
        );
    };
    auto useDiscrubPaused = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["app"]->discrubPaused;
        }
        );
    };
    auto useTask = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["app"]->task;
        }
        );
    };
    auto useSettings = [=]() mutable
    {
        return useAppSelector([=](auto state) mutable
        {
            return state["app"]->settings;
        }
        );
    };
    auto state = object{
        object::pair{std::string("discrubCancelled"), useDiscrubCancelled}, 
        object::pair{std::string("discrubPaused"), useDiscrubPaused}, 
        object::pair{std::string("task"), useTask}, 
        object::pair{std::string("settings"), useSettings}
    };
    auto setDiscrubPaused = [=](auto value) mutable
    {
        dispatch(setDiscrubPausedAction(value));
    };
    auto setDiscrubCancelled = [=](auto value) mutable
    {
        dispatch(setDiscrubCancelledAction(value));
    };
    auto setIsModifying = [=](auto value) mutable
    {
        dispatch(setIsModifyingAction(value));
    };
    auto setModifyEntity = [=](auto value) mutable
    {
        dispatch(setModifyEntityAction(value));
    };
    auto setStatus = [=](auto value) mutable
    {
        dispatch(setStatusAction(value));
    };
    auto resetStatus = [=]() mutable
    {
        dispatch(resetStatusAction());
    };
    auto resetModify = [=]() mutable
    {
        dispatch(resetModifyAction());
    };
    auto setSettings = [=](auto settings) mutable
    {
        dispatch(setSettingsAction(settings));
    };
    auto checkDiscrubPaused = [=]() mutable
    {
        std::async([=]() { dispatch(checkDiscrubPausedAction()); });
        return std::shared_ptr<Promise<void>>();
    };
    auto setTimeoutMessage = [=](auto message, auto timeout) mutable
    {
        std::async([=]() { dispatch(setTimeoutMessageAction(object{
            object::pair{std::string("message"), std::string("message")}, 
            object::pair{std::string("timeout"), std::string("timeout")}
        })); });
        return std::shared_ptr<Promise<void>>();
    };
    return object{
        object::pair{std::string("state"), std::string("state")}, 
        object::pair{std::string("setDiscrubPaused"), std::string("setDiscrubPaused")}, 
        object::pair{std::string("setDiscrubCancelled"), std::string("setDiscrubCancelled")}, 
        object::pair{std::string("setIsModifying"), std::string("setIsModifying")}, 
        object::pair{std::string("setModifyEntity"), std::string("setModifyEntity")}, 
        object::pair{std::string("setStatus"), std::string("setStatus")}, 
        object::pair{std::string("resetStatus"), std::string("resetStatus")}, 
        object::pair{std::string("resetModify"), std::string("resetModify")}, 
        object::pair{std::string("checkDiscrubPaused"), std::string("checkDiscrubPaused")}, 
        object::pair{std::string("setTimeoutMessage"), std::string("setTimeoutMessage")}, 
        object::pair{std::string("setSettings"), std::string("setSettings")}
    };
};

void Main(void)
{
}

MAIN
