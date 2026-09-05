#if defined _pawn_metrics_included
    #endinput
#endif
#define _pawn_metrics_included

native Metrics_Start(port = 9100);
native Metrics_StartEx(const bind_address[], port = 9100);
native Metrics_Stop();
native Metrics_IsRunning();
native Metrics_SetAuthToken(const token[]);
native Metrics_LoadConfig(const path[]);
native Metrics_StartFromConfig(const path[]);
native Metrics_Set(const name[], Float:value);
native Metrics_SetInt(const name[], value);
native Metrics_SetLabeled(const name[], const labels[], Float:value);
native Metrics_SetLabeledInt(const name[], const labels[], value);
native Metrics_Inc(const name[]);
native Metrics_IncLabeled(const name[], const labels[]);
native Metrics_Add(const name[], Float:value);
native Metrics_AddInt(const name[], value);
native Metrics_AddLabeled(const name[], const labels[], Float:value);
native Metrics_AddLabeledInt(const name[], const labels[], value);
native Metrics_ObserveHistogram(const name[], Float:value);
native Metrics_ObserveHistogramLabeled(const name[], const labels[], Float:value);
native Metrics_ObserveSummary(const name[], Float:value);
native Metrics_ObserveSummaryLabeled(const name[], const labels[], Float:value);
native Metrics_Remove(const name[]);
native Metrics_RemoveLabeled(const name[], const labels[]);
native Metrics_Clear();
native Metrics_SetServerInfo(const hostname[], const gamemode[], maxplayers);

stock Metrics_UpdateServerInfo()
{
    new hostname[144];
    new gamemode[64];

    GetServerVarAsString("hostname", hostname, sizeof hostname);
    GetServerVarAsString("gamemodetext", gamemode, sizeof gamemode);

    return Metrics_SetServerInfo(hostname, gamemode, GetMaxPlayers());
}

stock Metrics_SetPlayersOnline()
{
    new count;

    for (new playerid = 0, max = GetMaxPlayers(); playerid < max; playerid++)
    {
        if (IsPlayerConnected(playerid))
        {
            count++;
        }
    }

    return Metrics_SetInt("samp_players_online", count);
}
