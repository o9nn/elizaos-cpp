#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_TEST-UTILS_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_TEST-UTILS_TEST_H
#include "core.h"
#include "@jest/globals.h"
#include "path.h"
#include "../src/utils/config.h"
#include "../src/utils/common.h"

extern string REPO_ROOT;
template <typename T>
array<array<T>> chunkArray(array<T> array, double size);

array<any> flattenArray(array<any> array);

template <typename T>
array<T> removeDuplicates(array<T> array);

template <typename T>
T deepClone(T obj);

boolean isEqual(any obj1, any obj2);

template <typename T, typename K>
Pick<T, K> pick(T obj, array<K> keys);

template <typename T, typename K>
Omit<T, K> omit(T obj, array<K> keys);

template <typename T>
array<array<T>> chunkArray(array<T> array, double size)
{
    auto chunks = array<array<T>>();
    for (auto i = 0; i < array->get_length(); i += size)
    {
        chunks->push(array->slice(i, i + size));
    }
    return chunks;
};


template <typename T>
array<T> removeDuplicates(array<T> array)
{
    return array<any>{ std::make_shared<Set>(array) };
};


template <typename T>
T deepClone(T obj)
{
    if (OR((obj == nullptr), (type_of(obj) != std::string("object")))) {
        return obj;
    }
    if (is<Date>(obj)) {
        return as<any>(std::make_shared<Date>(obj->getTime()));
    }
    if (Array->isArray(obj)) {
        return as<any>(obj->map([=](auto item) mutable
        {
            return deepClone(item);
        }
        ));
    }
    auto cloned = object{};
    for (auto& key : keys_(obj))
    {
        if ((as<any>(obj))["hasOwnProperty"](key)) {
            cloned[key] = deepClone(const_((as<any>(obj)))[key]);
        }
    }
    return as<T>(cloned);
};


template <typename T, typename K>
Pick<T, K> pick(T obj, array<K> keys)
{
    shared result = as<Pick<T, K>>(object{});
    keys->forEach([=](auto key) mutable
    {
        if (in(key, obj)) {
            (*result)[key] = const_(obj)[key];
        }
    }
    );
    return result;
};


template <typename T, typename K>
Omit<T, K> omit(T obj, array<K> keys)
{
    shared result = as<any>(utils::assign(object{
    }, obj));
    keys->forEach([=](auto key) mutable
    {
        result.Delete(key);
    }
    );
    return result;
};


#endif
