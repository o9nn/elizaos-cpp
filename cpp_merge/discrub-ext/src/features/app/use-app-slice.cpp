#include "use-app-slice.h"

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
        object::pair{std:("discrubCancelled"), useDiscrubCancelled}, 
        object::pair{std:("discrubPaused"), useDiscrubPaused}, 
        object::pair{std:("task"), useTask}, 
        object::pair{std:("settings"), useSettings}
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
            object::pair{std:("message"), std:("message")}, 
            object::pair{std:("timeout"), std:("timeout")}
        })); });
        return std::shared_ptr<Promise<void>>();
    };
    return object{
        object::pair{std:("state"), std:("state")}, 
        object::pair{std:("setDiscrubPaused"), std:("setDiscrubPaused")}, 
        object::pair{std:("setDiscrubCancelled"), std:("setDiscrubCancelled")}, 
        object::pair{std:("setIsModifying"), std:("setIsModifying")}, 
        object::pair{std:("setModifyEntity"), std:("setModifyEntity")}, 
        object::pair{std:("setStatus"), std:("setStatus")}, 
        object::pair{std:("resetStatus"), std:("resetStatus")}, 
        object::pair{std:("resetModify"), std:("resetModify")}, 
        object::pair{std:("checkDiscrubPaused"), std:("checkDiscrubPaused")}, 
        object::pair{std:("setTimeoutMessage"), std:("setTimeoutMessage")}, 
        object::pair{std:("setSettings"), std:("setSettings")}
    };
};

void Main(void)
{
}

MAIN
