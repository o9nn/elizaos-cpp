#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/utils/axios.h"

std::string baseURL = process->env->NEXT_PUBLIC_NEST_API_URL;
std::any axiosInstance = axios->create(object{
    object::pair{std::string("baseURL"), std::string("baseURL")}, 
    object::pair{std::string("headers"), object{
        object::pair{std::string("Content-Type"), std::string("application/json")}
    }}
});
std::function<std::any(std::any, object)> get = [=](auto url, auto config = object{}) mutable
{
    return axiosInstance->get(url, config);
};

void Main(void)
{
    axiosInstance->interceptors->request->use([=](auto config) mutable
    {
        auto token = localStorage->getItem(std::string("accessToken"));
        if (token) {
            config["headers"]["Authorization"] = std::string("Bearer ") + token + string_empty;
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
