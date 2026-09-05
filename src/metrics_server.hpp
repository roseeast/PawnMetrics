#pragma once

#include "metrics_store.hpp"

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>

class MetricsServer {
public:
    explicit MetricsServer(MetricsStore& store);
    ~MetricsServer();

    bool start(const std::string& bind_address, std::uint16_t port, const std::string& auth_token);
    void stop();
    bool running() const;

private:
    void run(std::string bind_address, std::uint16_t port, std::string auth_token);
    void close_socket();
    static bool is_authorized(const std::string& request, const std::string& auth_token);

    MetricsStore& store_;
    std::atomic<bool> running_{false};
    std::thread thread_;
    int socket_{-1};
};
