#include <a_samp>
#include <pawn_metrics>

main()
{
}

public OnGameModeInit()
{
    Metrics_LoadConfig("scriptfiles/pawn_metrics.cfg");
    Metrics_SetAuthToken("");

    if (!Metrics_StartEx("127.0.0.1", 9100))
    {
        print("[pawn-metrics] failed to start metrics server");
        return 1;
    }

    Metrics_UpdateServerInfo();
    Metrics_SetInt("samp_players_online", 0);
    Metrics_SetInt("samp_connects_total", 0);
    Metrics_SetInt("samp_disconnects_total", 0);
    Metrics_SetInt("samp_commands_total", 0);
    Metrics_SetInt("samp_anticheat_flags_total", 0);
    Metrics_SetInt("samp_money_created_total", 0);
    Metrics_SetInt("samp_money_removed_total", 0);
    Metrics_SetLabeledInt("samp_reports_open", "type=\"player\"", 0);

    SetTimer("Metrics_Update", 5000, true);
    print("[pawn-metrics] scrape endpoint: http://127.0.0.1:9100/metrics");
    return 1;
}

public OnGameModeExit()
{
    Metrics_Stop();
    return 1;
}

public OnPlayerConnect(playerid)
{
    Metrics_Inc("samp_connects_total");
    Metrics_SetPlayersOnline();
    Metrics_ObserveHistogram("samp_player_connect_seconds", 0.05);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    Metrics_Inc("samp_disconnects_total");
    Metrics_IncLabeled("samp_disconnects_reason_total", "reason=\"quit\"");
    Metrics_SetPlayersOnline();
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    Metrics_Inc("samp_commands_total");
    Metrics_ObserveSummaryLabeled("samp_command_length_chars", "source=\"player\"", float(strlen(cmdtext)));
    return 0;
}

forward Metrics_Update();
public Metrics_Update()
{
    Metrics_UpdateServerInfo();
    Metrics_SetPlayersOnline();
    return 1;
}
