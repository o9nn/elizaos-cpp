#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/hooks/use-sidebar-state.h"

any useSidebarState()
{
    auto [isVisible, setIsVisible] = useState<boolean>(false);
    useEffect([=]() mutable
    {
        try
        {
            auto stored = localStorage->getItem(SIDEBAR_STATE_KEY);
            if (stored != nullptr) {
                auto parsedState = JSON->parse(stored);
                setIsVisible(parsedState);
            }
        }
        catch (const any& error)
        {
            clientLogger->error(std::string("Error reading sidebar state from localStorage:"), error);
            setIsVisible(false);
        }
    }
    , array<any>());
    shared setSidebarVisible = useCallback([=](auto visible) mutable
    {
        setIsVisible(visible);
        try
        {
            localStorage->setItem(SIDEBAR_STATE_KEY, JSON->stringify(visible));
        }
        catch (const any& error)
        {
            clientLogger->error(std::string("Error saving sidebar state to localStorage:"), error);
        }
    }
    , array<any>());
    auto toggleSidebar = useCallback([=]() mutable
    {
        setSidebarVisible(!isVisible);
    }
    , array<any>{ isVisible, setSidebarVisible });
    return object{
        object::pair{std::string("isVisible"), std::string("isVisible")}, 
        object::pair{std::string("setSidebarVisible"), std::string("setSidebarVisible")}, 
        object::pair{std::string("toggleSidebar"), std::string("toggleSidebar")}
    };
};


string SIDEBAR_STATE_KEY = std::string("eliza-agent-sidebar-visible");

void Main(void)
{
}

MAIN
