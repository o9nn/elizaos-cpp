#include "use-pause.h"

any usePause()
{
    auto [paused, setPause] = useState<boolean>(false);
    return object{
        object::pair{std:("paused"), std:("paused")}, 
        object::pair{std:("setPause"), std:("setPause")}
    };
};


