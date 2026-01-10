#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/hooks/use-confirmation.h"

any useConfirmation()
{
    auto [confirmationState, setConfirmationState] = useState<object>(object{
        object::pair{std::string("open"), false}, 
        object::pair{std::string("options"), nullptr}, 
        object::pair{std::string("onConfirm"), nullptr}
    });
    auto confirm = useCallback([=](auto options, auto onConfirm) mutable
    {
        setConfirmationState(object{
            object::pair{std::string("open"), true}, 
            object::pair{std::string("options"), std::string("options")}, 
            object::pair{std::string("onConfirm"), std::string("onConfirm")}
        });
    }
    , array<any>());
    shared handleOpenChange = useCallback([=](auto open) mutable
    {
        if (!open) {
            setConfirmationState(object{
                object::pair{std::string("open"), false}, 
                object::pair{std::string("options"), nullptr}, 
                object::pair{std::string("onConfirm"), nullptr}
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
        object::pair{std::string("confirm"), std::string("confirm")}, 
        object::pair{std::string("isOpen"), confirmationState->open}, 
        object::pair{std::string("onOpenChange"), handleOpenChange}, 
        object::pair{std::string("onConfirm"), handleConfirm}, 
        object::pair{std::string("options"), confirmationState->options}
    };
};


