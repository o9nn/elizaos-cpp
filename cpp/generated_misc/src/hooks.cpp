#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/app/hooks.h"

std::function<std::shared_ptr<AppDispatch>()> useAppDispatch = useDispatch;
std::shared_ptr<TypedUseSelectorHook<std::shared_ptr<RootState>>> useAppSelector = useSelector;

void Main(void)
{
}

MAIN
