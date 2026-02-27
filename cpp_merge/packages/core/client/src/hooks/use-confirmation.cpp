#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/hooks/use-confirmation.h"

any useConfirmation()
{
    auto [confirmationState, setConfirmationState] = useState<object>(object{
        object::pair{std:("open"), false}, 
        object::pair{std:("options"), nullptr}, 
        object::pair{std:("onConfirm"), nullptr}
    });
    auto confirm = useCallback([=](auto options, auto onConfirm) mutable
    {
        setConfirmationState(object{
            object::pair{std:("open"), true}, 
            object::pair{std:("options"), std:("options")}, 
            object::pair{std:("onConfirm"), std:("onConfirm")}
        });
    }
    , array<any>());
    shared handleOpenChange = useCallback([=](auto open) mutable
    {
        if (!open) {
            setConfirmationState(object{
                object::pair{std:("open"), false}, 
                object::pair{std:("options"), nullptr}, 
                object::pair{std:("onConfirm"), nullptr}
            });
        }
    }
    , array<any>());
    auto handleConfirm = useCallback([=]() mutable
    {
        if (confirmationState->onConfirm) {
            confirmationState->onConfirm();
        }
        handleOpenChange(false);
    }
    , array<any>{ confirmationState->onConfirm, handleOpenChange });
    return object{
        object::pair{std:("confirm"), std:("confirm")}, 
        object::pair{std:("isOpen"), confirmationState->open}, 
        object::pair{std:("onOpenChange"), handleOpenChange}, 
        object::pair{std:("onConfirm"), handleConfirm}, 
        object::pair{std:("options"), confirmationState->options}
    };
};


