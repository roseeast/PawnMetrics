#include "metrics_server.hpp"
#include "metrics_store.hpp"

#include <cstdint>
#include <string>

#include "amx/amx.h"
#include "plugincommon.h"

typedef void (*logprintf_t)(const char* format, ...);
extern void* pAMXFunctions;
logprintf_t logprintf = nullptr;

namespace {

MetricsStore g_store;
MetricsServer g_server(g_store);

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

cell AMX_NATIVE_CALL Metrics_Start(AMX*, cell* params)
{
    const int port = params[1] > 0 ? static_cast<int>(params[1]) : 9100;

    if (port < 1 || port > 65535) {
        return 0;
    }

    return g_server.start(static_cast<std::uint16_t>(port)) ? 1 : 0;
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

cell AMX_NATIVE_CALL Metrics_Set(AMX* amx, cell* params)
{
    g_store.set(get_string(amx, params[1]), amx_ctof(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_SetInt(AMX* amx, cell* params)
{
    g_store.set(get_string(amx, params[1]), static_cast<double>(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Inc(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), 1.0);
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Add(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), amx_ctof(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_AddInt(AMX* amx, cell* params)
{
    g_store.add(get_string(amx, params[1]), static_cast<double>(params[2]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Remove(AMX* amx, cell* params)
{
    g_store.remove(get_string(amx, params[1]));
    return 1;
}

cell AMX_NATIVE_CALL Metrics_Clear(AMX*, cell*)
{
    g_store.clear();
    return 1;
}

cell AMX_NATIVE_CALL Metrics_SetServerInfo(AMX*, cell* params)
{
    g_store.set("samp_server_maxplayers", static_cast<double>(params[3]));
    g_store.set("samp_server_info", 1.0);
    return 1;
}

AMX_NATIVE_INFO natives[] = {
    {"Metrics_Start", Metrics_Start},
    {"Metrics_Stop", Metrics_Stop},
    {"Metrics_IsRunning", Metrics_IsRunning},
    {"Metrics_Set", Metrics_Set},
    {"Metrics_SetInt", Metrics_SetInt},
    {"Metrics_Inc", Metrics_Inc},
    {"Metrics_Add", Metrics_Add},
    {"Metrics_AddInt", Metrics_AddInt},
    {"Metrics_Remove", Metrics_Remove},
    {"Metrics_Clear", Metrics_Clear},
    {"Metrics_SetServerInfo", Metrics_SetServerInfo},
    {nullptr, nullptr}
};

}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

    if (logprintf != nullptr) {
        logprintf("[pawn-metrics] plugin loaded");
    }

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    g_server.stop();

    if (logprintf != nullptr) {
        logprintf("[pawn-metrics] plugin unloaded");
    }
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
    return amx_Register(amx, natives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX*)
{
    return AMX_ERR_NONE;
}

