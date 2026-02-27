#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-outside-clickdetection.h"

std::function<void(array<any>, std::function<void()>)> useOutsideClickDetection = [=](auto refs, auto doOnOutsideClick) mutable
{
    useEffect([=]() mutable
    {
        shared handleClickOutside = [=](auto event) mutable
        {
            for (auto i = 0; i < refs->get_length(); i++)
            {
                auto ref = const_(refs)[i];
                if (AND((AND((AND((ref), (ref->current))), (is<Node>(event->target)))), (ref->current->contains(event->target)))) return;
            }
            doOnOutsideClick();
        };
        document->addEventListener(std::string("click"), handleClickOutside);
        return [=]() mutable
        {
            document->removeEventListener(std::string("click"), handleClickOutside);
        };
    }
    , array<array<any>>{ refs, doOnOutsideClick });
};

void Main(void)
{
}

MAIN
