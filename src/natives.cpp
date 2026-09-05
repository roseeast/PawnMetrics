#include "natives.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>

MetricsStore g_store;
MetricsServer g_server(g_store);
MetricsConfig g_config;

namespace {

std::string trim(std::string value)
{
    const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });

    const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    if (begin >= end) {
        return {};
    }

    return std::string(begin, end);
}

std::string get_string(AMX* amx, cell param)
{
    cell* address = nullptr;

    if (amx_GetAddr(amx, param, &address) != AMX_ERR_NONE || address == nullptr) {
        return {};
    }

    int length = 0;
    amx_StrLen(address, &length);

    if (length <= 0) {
        return {};
    }

    std::string value(static_cast<std::size_t>(length), '\0');
    amx_GetString(value.data(), address, 0, length + 1);
    return value;
}

bool read_config(const std::string& path, MetricsConfig& config)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        const std::size_t comment = line.find_first_of("#;");
        if (comment != std::string::npos) {
            line = line.substr(0, comment);
        }

        const std::size_t equals = line.find('=');
        if (equals == std::string::npos) {
            continue;
        }

        const std::string key = trim(line.substr(0, equals));
        const std::string value = trim(line.substr(equals + 1));

        if (key == "bind" || key == "bind_address") {
            config.bind = value;
        } else if (key == "port") {
            try {
                const int port = std::stoi(value.empty() ? "0" : value);
                if (port > 0 && port <= 65535) {
                    config.port = static_cast<std::uint16_t>(port);
                }
            } catch (...) {
                return false;
            }
        } else if (key == "auth_token" || key == "token") {
            config.auth_token = value;
        }
    }

    return true;
}

cell AMX_NATIVE_CALL Metrics_Start(AMX*, cell* params)
{
    const int port = params[1] > 0 ? static_cast<int>(params[1]) : 9100;

    if (port < 1 || port > 65535) {
        return 0;
    }

    g_config.port = static_cast<std::uint16_t>(port);
    return g_server.start(g_config.bind, g_config.port, g_config.auth_token) ? 1 : 0;
}

cell AMX_NATIVE_CALL Metrics_StartEx(AMX* amx, cell* params)
{
    const std::string bind = get_string(amx, params[1]);
    const int port = params[2] > 0 ? static_cast<int>(params[2]) : 9100;

    if (port < 1 || port > 65535) {
        return 0;
    }

    g_config.bind = bind.empty() ? "0.0.0.0" : bind;
    g_config.port = static_cast<std::uint16_t>(port);
    return g_server.start(g_config.bind, g_config.port, g_config.auth_token) ? 1 : 0;
}

cell AMX_NATIVE_CALL Metrics_Stop(AMX*, cell*)
{
    g_server.stop();
    return 1;
}

cell AMX_NATIVE_CALL Metrics_IsRunning(AMX*, cell*)
{
    return g_server.running() ? 1 : 0;
}

cell AMX_NATIVE_CALL Metrics_SetAuthToken(AMX* amx, cell* params)
{
    g_config.auth_token = get_string(amx, params[1]);
    return 1;
}

cell AMX_NATIVE_CALL Metrics_LoadConfig(AMX* amx, cell* params)
{
    return read_config(get_string(amx, params[1]), g_config) ? 1 : 0;
}

cell AMX_NATIVE_CALL Metrics_StartFromConfig(AMX* amx, cell* params)
{
    if (!read_config(get_string(amx, params[1]), g_config)) {
        return 0;
    }

    return g_server.start(g_config.bind, g_config.port, g_config.auth_token) ? 1 : 0;
}

cell AMX_NATIVE_CALL Metrics_Set(AMX* amx, cell* params)
{
    g_store.set(get_string(amx, params[1]), "", amx_ctof(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_SetInt(AMX* amx, cell* params)
{
    g_store.set(get_string(amx, params[1]), "", static_cast<double>(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_SetLabeled(AMX* amx, cell* params)
{
    g_store.set(get_string(amx, params[1]), get_string(amx, params[2]), amx_ctof(params[3]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_SetLabeledInt(AMX* amx, cell* params)
{
    g_store.set(get_string(amx, params[1]), get_string(amx, params[2]), static_cast<double>(params[3]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Inc(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), "", 1.0);
    return 1;
}

cell AMX_NATIVE_CALL Metrics_IncLabeled(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), get_string(amx, params[2]), 1.0);
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Add(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), "", amx_ctof(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_AddInt(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), "", static_cast<double>(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_AddLabeled(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), get_string(amx, params[2]), amx_ctof(params[3]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_AddLabeledInt(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), get_string(amx, params[2]), static_cast<double>(params[3]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_ObserveHistogram(AMX* amx, cell* params)
{
    g_store.observe_histogram(get_string(amx, params[1]), "", amx_ctof(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_ObserveHistogramLabeled(AMX* amx, cell* params)
{
    g_store.observe_histogram(get_string(amx, params[1]), get_string(amx, params[2]), amx_ctof(params[3]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_ObserveSummary(AMX* amx, cell* params)
{
    g_store.observe_summary(get_string(amx, params[1]), "", amx_ctof(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_ObserveSummaryLabeled(AMX* amx, cell* params)
{
    g_store.observe_summary(get_string(amx, params[1]), get_string(amx, params[2]), amx_ctof(params[3]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Remove(AMX* amx, cell* params)
{
    g_store.remove(get_string(amx, params[1]), "");
    return 1;
}

cell AMX_NATIVE_CALL Metrics_RemoveLabeled(AMX* amx, cell* params)
{
    g_store.remove(get_string(amx, params[1]), get_string(amx, params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Clear(AMX*, cell*)
{
    g_store.clear();
    return 1;
}

cell AMX_NATIVE_CALL Metrics_SetServerInfo(AMX*, cell* params)
{
    g_store.set("samp_server_maxplayers", "", static_cast<double>(params[3]));
    g_store.set("samp_server_info", "", 1.0);
    return 1;
}

const AMX_NATIVE_INFO s_natives[] = {
    {"Metrics_Start", Metrics_Start},
    {"Metrics_StartEx", Metrics_StartEx},
    {"Metrics_Stop", Metrics_Stop},
    {"Metrics_IsRunning", Metrics_IsRunning},
    {"Metrics_SetAuthToken", Metrics_SetAuthToken},
    {"Metrics_LoadConfig", Metrics_LoadConfig},
    {"Metrics_StartFromConfig", Metrics_StartFromConfig},
    {"Metrics_Set", Metrics_Set},
    {"Metrics_SetInt", Metrics_SetInt},
    {"Metrics_SetLabeled", Metrics_SetLabeled},
    {"Metrics_SetLabeledInt", Metrics_SetLabeledInt},
    {"Metrics_Inc", Metrics_Inc},
    {"Metrics_IncLabeled", Metrics_IncLabeled},
    {"Metrics_Add", Metrics_Add},
    {"Metrics_AddInt", Metrics_AddInt},
    {"Metrics_AddLabeled", Metrics_AddLabeled},
    {"Metrics_AddLabeledInt", Metrics_AddLabeledInt},
    {"Metrics_ObserveHistogram", Metrics_ObserveHistogram},
    {"Metrics_ObserveHistogramLabeled", Metrics_ObserveHistogramLabeled},
    {"Metrics_ObserveSummary", Metrics_ObserveSummary},
    {"Metrics_ObserveSummaryLabeled", Metrics_ObserveSummaryLabeled},
    {"Metrics_Remove", Metrics_Remove},
    {"Metrics_RemoveLabeled", Metrics_RemoveLabeled},
    {"Metrics_Clear", Metrics_Clear},
    {"Metrics_SetServerInfo", Metrics_SetServerInfo},
    {nullptr, nullptr}
};

} // namespace

const AMX_NATIVE_INFO* get_pawn_metrics_natives()
{
    return s_natives;
}

std::size_t get_pawn_metrics_native_count()
{
    return (sizeof(s_natives) / sizeof(s_natives[0])) - 1;
}

void stop_pawn_metrics()
{
    g_server.stop();
}
