#if defined _pawn_metrics_included
    #endinput
#endif
#define _pawn_metrics_included

native Metrics_Start(port = 9100);
native Metrics_Stop();
native Metrics_IsRunning();
native Metrics_Set(const name[], Float:value);
native Metrics_SetInt(const name[], value);
native Metrics_Inc(const name[]);
native Metrics_Add(const name[], Float:value);
native Metrics_AddInt(const name[], value);
native Metrics_Remove(const name[]);
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

