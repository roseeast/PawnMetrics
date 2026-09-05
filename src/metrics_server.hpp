#pragma once

#include "metrics_store.hpp"

#include <atomic>
#include <cstdint>
#include <thread>

class MetricsServer {
public:
    explicit MetricsServer(MetricsStore& store);
    ~MetricsServer();

    bool start(std::uint16_t port);
    void stop();
    bool running() const;

private:
    void run(std::uint16_t port);
    void close_socket();

    MetricsStore& store_;
    std::atomic<bool> running_{false};
    std::thread thread_;
    int socket_{-1};
};

