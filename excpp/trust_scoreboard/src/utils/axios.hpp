#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

const baseURL = process.env.NEXT_PUBLIC_NEST_API_URL;

const axiosInstance = axios.create({
  baseURL,
  headers: {
    'Content-Type': 'application/json'
  }
});

// Intercept requests and add the authorization token if it exists
axiosInstance.interceptors.request.use(
  config => {
    const token = localStorage.getItem('accessToken');
    if (token) {
      config.headers.Authorization = `Bearer ${token}`;
    }
    return config;
  },
  error => Promise.reject(error)
);

const get = (url, config = {}) => axiosInstance.get(url, config);
// const post = (url, data, config = {}) => axiosInstance.post(url, data, config);
// const put = (url, data, config = {}) => axiosInstance.put(url, data, config);
// const patch = (url, data, config = {}) => axiosInstance.patch(url, data, config);
// const del = (url, config = {}) => axiosInstance.delete(url, config);
} // namespace elizaos
