#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_API_CLIENT_SRC_TYPES_BASE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_API_CLIENT_SRC_TYPES_BASE_H
#include "core.hpp"
// External dependency removed

template <typename T, typename RET>
using ApiResponse = any;

template <typename T>
class ApiSuccessResponse;
class ApiErrorResponse;
class RequestConfig;
class PaginationParams;
class ApiClientConfig;

template <typename T>
class ApiSuccessResponse : public object, public std::enable_shared_from_this<ApiSuccessResponse<T>> {
public:
    using std::enable_shared_from_this<ApiSuccessResponse<T>>::shared_from_this;
    boolean success;

    T data;
};

class ApiErrorResponse : public object, public std::enable_shared_from_this<ApiErrorResponse> {
public:
    using std::enable_shared_from_this<ApiErrorResponse>::shared_from_this;
    boolean success;

    object error;
};

class RequestConfig : public object, public std::enable_shared_from_this<RequestConfig> {
public:
    using std::enable_shared_from_this<RequestConfig>::shared_from_this;
    Record<string, string> headers;

    Record<string, any> params;

    double timeout;
};

class PaginationParams : public object, public std::enable_shared_from_this<PaginationParams> {
public:
    using std::enable_shared_from_this<PaginationParams>::shared_from_this;
    double page;

    double limit;

    double offset;
};

class ApiClientConfig : public object, public std::enable_shared_from_this<ApiClientConfig> {
public:
    using std::enable_shared_from_this<ApiClientConfig>::shared_from_this;
    string baseUrl;

    string apiKey;

    double timeout;

    Record<string, string> headers;
};

#endif
