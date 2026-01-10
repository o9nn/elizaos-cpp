#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_ARRAYHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_ARRAYHELPERS_H
#include "core.h"

template <typename T, typename K, typename P1>
Record<K, array<T>> groupBy(array<T> items, P1 keyFn);

template <typename T>
array<array<T>> chunk(array<T> array, double size);

template <typename T, typename K, typename P1>
Record<K, array<T>> groupBy(array<T> items, P1 keyFn)
{
    return items->reduce([=](auto groups, auto item) mutable
    {
        auto key = keyFn(item);
        groups[key] = OR((const_(groups)[key]), (array<any>()));
        const_(groups)[key]->push(item);
        return groups;
    }
    , as<Record<K, array<T>>>(object{}));
};


template <typename T>
array<array<T>> chunk(array<T> array, double size)
{
    return Array->from(object{
        object::pair{std::string("length"), Math->ceil(array->get_length() / size)}
    }, [=](auto _, auto i) mutable
    {
        return array->slice(i * size, (i + 1) * size);
    }
    );
};


#endif
