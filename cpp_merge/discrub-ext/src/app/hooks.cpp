#include "hooks.hpp"

std::function<std::shared_ptr<AppDispatch>()> useAppDispatch = useDispatch;
std::shared_ptr<TypedUseSelectorHook<std::shared_ptr<RootState>>> useAppSelector = useSelector;

void Main(void)
{
}

MAIN
