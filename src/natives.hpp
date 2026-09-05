#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "amx/amx.h"
#include "metrics_server.hpp"
#include "metrics_store.hpp"

struct MetricsConfig {
    std::string bind = "0.0.0.0";
    std::uint16_t port = 9100;
    std::string auth_token;
};

extern MetricsStore g_store;
extern MetricsServer g_server;
extern MetricsConfig g_config;

const AMX_NATIVE_INFO* get_pawn_metrics_natives();
std::size_t get_pawn_metrics_native_count();
void stop_pawn_metrics();
