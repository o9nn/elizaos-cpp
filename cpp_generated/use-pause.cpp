#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-pause.h"

any usePause()
{
    auto [paused, setPause] = useState<boolean>(false);
    return object{
        object::pair{std::string("paused"), std::string("paused")}, 
        object::pair{std::string("setPause"), std::string("setPause")}
    };
};


