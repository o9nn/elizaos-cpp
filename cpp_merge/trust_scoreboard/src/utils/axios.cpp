#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/utils/axios.h"

string baseURL = process->env->NEXT_PUBLIC_NEST_API_URL;
any axiosInstance = axios->create(object{
    object::pair{std:("baseURL"), std:("baseURL")}, 
    object::pair{std:("headers"), object{
        object::pair{std:("Content-Type"), std:("application/json")}
    }}
});
std::function<any(any, object)> get = [=](auto url, auto config = object{}) mutable
{
    return axiosInstance->get(url, config);
};

void Main(void)
{
    axiosInstance->interceptors->request->use([=](auto config) mutable
    {
        auto token = localStorage->getItem(std:("accessToken"));
        if (token) {
            config["headers"]["Authorization"] = std:("Bearer ") + token + string_empty;
        }
        return config;
    }
    , [=](auto error) mutable
    {
        return Promise->reject(error);
    }
    );
}

MAIN
