#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/auth.h"

std::shared_ptr<Promise<std::shared_ptr<LoginResponse>>> AuthService::login(std::shared_ptr<LoginRequest> request)
{
    auto response = std::async([=]() { this->post<std::shared_ptr<LoginResponse>>(std::string("/api/auth/login"), request); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<RefreshTokenResponse>>> AuthService::refreshToken()
{
    auto response = std::async([=]() { this->post<std::shared_ptr<RefreshTokenResponse>>(std::string("/api/auth/refresh"), object{}); });
    return response;
}

std::shared_ptr<Promise<std::shared_ptr<CurrentUserResponse>>> AuthService::getCurrentUser()
{
    auto response = std::async([=]() { this->get<std::shared_ptr<CurrentUserResponse>>(std::string("/api/auth/me")); });
    return response;
}

AuthService::AuthService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

