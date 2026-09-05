#include "natives.hpp"

#include "amx/amx.h"
#include "plugincommon.h"

typedef void (*logprintf_t)(const char* format, ...);
extern void* pAMXFunctions;
logprintf_t logprintf = nullptr;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = reinterpret_cast<logprintf_t>(ppData[PLUGIN_DATA_LOGPRINTF]);

    if (logprintf != nullptr) {
        logprintf("[pawn-metrics] plugin loaded (legacy plugin mode)");
    }

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    stop_pawn_metrics();

    if (logprintf != nullptr) {
        logprintf("[pawn-metrics] plugin unloaded");
    }
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
    return amx_Register(amx, get_pawn_metrics_natives(), -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX*)
{
    return AMX_ERR_NONE;
}
