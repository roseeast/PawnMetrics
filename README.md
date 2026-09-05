<h1 align="center">Pawn Metrics</h1>

<p align="center">
  Prometheus-style metrics exporter for SA-MP and open.mp Pawn servers.
</p>

<p align="center">
  <a href="#compatibility"><img alt="SA-MP" src="https://img.shields.io/badge/SA--MP-supported-2f6feb?style=flat-square"></a>
  <a href="#compatibility"><img alt="open.mp" src="https://img.shields.io/badge/open.mp-legacy%20plugin-00a86b?style=flat-square"></a>
  <a href="#build-from-source"><img alt="C++17" src="https://img.shields.io/badge/C++-17-00599c?style=flat-square"></a>
  <a href="#prometheus"><img alt="Prometheus" src="https://img.shields.io/badge/Prometheus-text%20format-e6522c?style=flat-square"></a>
  <a href="#release-files"><img alt="Platforms" src="https://img.shields.io/badge/Linux%20%7C%20Windows-x86__64%20%7C%20x86-111827?style=flat-square"></a>
</p>

<p align="center">
  <a href="#installation">Installation</a>
  ·
  <a href="#native-api">Native API</a>
  ·
  <a href="#prometheus">Prometheus</a>
  ·
  <a href="#build-from-source">Build</a>
</p>

---

Pawn Metrics lets Pawn gamemodes expose server, gameplay, economy, admin, and anti-cheat metrics through a lightweight HTTP endpoint. It is designed for server owners who want modern monitoring with Prometheus, Grafana, uptime dashboards, or simple HTTP scrapers.

## Overview

| Item | Detail |
| --- | --- |
| Plugin name | `pawn_metrics` |
| Target | SA-MP and open.mp |
| Plugin type | Legacy SA-MP plugin API & Native open.mp Component |
| Language | C++17 |
| Metrics format | Prometheus text exposition format |
| Default endpoint | `http://127.0.0.1:9100/metrics` |
| Config file format | Simple `key=value` file |
| External runtime deps | None bundled by Pawn Metrics |
| Included platforms | Linux x86_64, Windows i386 |
| Pawn include | `pawno/include/pawn_metrics.inc` |

## Features

| Feature | Native | Notes |
| --- | --- | --- |
| Start exporter | `Metrics_Start` | Starts the HTTP metrics endpoint. |
| Stop exporter | `Metrics_Stop` | Stops the endpoint cleanly. |
| Set gauge value | `Metrics_Set`, `Metrics_SetInt` | Useful for online players, queue sizes, loaded houses, etc. |
| Increment counter | `Metrics_Inc` | Useful for connects, commands, kills, reports, warnings. |
| Add numeric value | `Metrics_Add`, `Metrics_AddInt` | Useful for money created/removed, item flow, query counts. |
| Labeled metrics | `Metrics_*Labeled` | Adds Prometheus labels such as `reason="teleport"` or `type="report"`. |
| Histograms | `Metrics_ObserveHistogram` | Records bucketed observations with default Prometheus-style buckets. |
| Summaries | `Metrics_ObserveSummary` | Records `_sum` and `_count` observations. |
| Auth token | `Metrics_SetAuthToken` | Protects `/metrics` with bearer token or `?token=` access. |
| Bind address | `Metrics_StartEx` | Binds the exporter to a specific IPv4 address. |
| Config file | `Metrics_LoadConfig`, `Metrics_StartFromConfig` | Loads bind address, port, and auth token from file. |
| Remove metric | `Metrics_Remove` | Deletes one metric from the exporter. |
| Clear metrics | `Metrics_Clear` | Resets all registered metrics. |
| Server helper | `Metrics_UpdateServerInfo` | Stock helper for server info. |
| Player helper | `Metrics_SetPlayersOnline` | Stock helper for current connected players. |

## Release Files

Prebuilt release packages are available in `release/`.

| File | Format | Contents |
| --- | --- | --- |
| `release/pawn-metrics-v0.2.0-windows-x86.zip` | ZIP | Windows release archive (legacy plugin & native component). |
| `release/pawn-metrics-v0.2.0-linux-x86_64.tar.gz` | tar.gz | Linux release archive (legacy plugin & native component). |

Package layout:

```text
pawn-metrics-v0.2.0/
  plugins/
    pawn_metrics.so
    pawn_metrics.dll
  components/
    pawn_metrics.so
    pawn_metrics.dll
  pawno/include/
    pawn_metrics.inc
  examples/
    example.pwn
    pawn_metrics.cfg
  README.md
  LICENSE
  CHANGELOG.md
```

## Compatibility

| Runtime | Status | Notes |
| --- | --- | --- |
| SA-MP Windows server | Supported | Use `plugins/pawn_metrics.dll`. The included DLL is 32-bit for classic SA-MP Windows servers. |
| SA-MP Linux server | Supported | Use `plugins/pawn_metrics.so`. The included Linux binary is x86_64. |
| open.mp Windows server | Supported (Component & Plugin) | Native component: `components/pawn_metrics.dll`. Legacy plugin: `plugins/pawn_metrics.dll`. |
| open.mp Linux server | Supported (Component & Plugin) | Native component: `components/pawn_metrics.so`. Legacy plugin: `plugins/pawn_metrics.so`. |
| Native open.mp component | Supported | Load via `"components": ["pawn_metrics"]` in `config.json`. See `components/openmp/README.md`. |

Pawn Metrics does not hook RakNet, patch memory, or depend on internal SA-MP addresses. It only registers Pawn natives and runs a small HTTP listener for metrics scraping.

## Installation

### 1. Install Files

#### For SA-MP (Legacy Plugin)

Copy the files from `plugins/` into your server:

```text
plugins/pawn_metrics.so        -> Linux server plugins folder
plugins/pawn_metrics.dll       -> Windows server plugins folder
pawno/include/pawn_metrics.inc -> Pawn compiler include folder
```

#### For open.mp (Native Component - Recommended)

Copy the files from `components/` into your server:

```text
components/pawn_metrics.so     -> Linux server components folder
components/pawn_metrics.dll    -> Windows server components folder
pawno/include/pawn_metrics.inc -> Pawn compiler include folder
```

For Qawno or custom build systems, place `pawn_metrics.inc` in the include path used by your compiler.

### 2. Load The Plugin / Component

#### For SA-MP `server.cfg`:

```text
plugins pawn_metrics
```

On some older Linux setups:

```text
plugins pawn_metrics.so
```

#### For open.mp `config.json` (Native Component Mode - Recommended):

```json
{
  "components": [
    "pawn_metrics"
  ]
}
```

#### For open.mp `config.json` (Legacy Plugin Mode):

```json
{
  "pawn": {
    "legacy_plugins": ["pawn_metrics"]
  }
}
```

### 3. Include In Pawn

```pawn
#include <pawn_metrics>
```

## Quick Start

```pawn
#include <a_samp>
#include <pawn_metrics>

public OnGameModeInit()
{
    Metrics_SetAuthToken("change-this-token");
    Metrics_StartEx("127.0.0.1", 9100);
    Metrics_UpdateServerInfo();
    Metrics_SetInt("samp_players_online", 0);
    Metrics_SetInt("samp_connects_total", 0);
    Metrics_SetLabeledInt("samp_reports_open", "type=\"player\"", 0);

    SetTimer("Metrics_Update", 5000, true);
    return 1;
}

public OnPlayerConnect(playerid)
{
    Metrics_Inc("samp_connects_total");
    Metrics_SetPlayersOnline();
    Metrics_ObserveHistogram("samp_player_connect_seconds", 0.05);
    return 1;
}

forward Metrics_Update();
public Metrics_Update()
{
    Metrics_UpdateServerInfo();
    Metrics_SetPlayersOnline();
    Metrics_ObserveSummaryLabeled("samp_tick_work_seconds", "source=\"metrics\"", 0.001);
    return 1;
}
```

Then open:

```text
http://127.0.0.1:9100/metrics?token=change-this-token
```

## Native API

| Native | Signature | Return | Description |
| --- | --- | ---: | --- |
| `Metrics_Start` | `port = 9100` | `1` or `0` | Starts the HTTP exporter. |
| `Metrics_StartEx` | `const bind_address[], port = 9100` | `1` or `0` | Starts the exporter on a specific IPv4 address. |
| `Metrics_Stop` | none | `1` | Stops the exporter. |
| `Metrics_IsRunning` | none | `1` or `0` | Checks whether the exporter is running. |
| `Metrics_SetAuthToken` | `const token[]` | `1` | Sets the required bearer/query token for metrics access. |
| `Metrics_LoadConfig` | `const path[]` | `1` or `0` | Loads config without starting the exporter. |
| `Metrics_StartFromConfig` | `const path[]` | `1` or `0` | Loads config and starts the exporter. |
| `Metrics_Set` | `const name[], Float:value` | `1` | Sets a floating-point metric value. |
| `Metrics_SetInt` | `const name[], value` | `1` | Sets an integer metric value. |
| `Metrics_SetLabeled` | `const name[], const labels[], Float:value` | `1` | Sets a labeled floating-point metric. |
| `Metrics_SetLabeledInt` | `const name[], const labels[], value` | `1` | Sets a labeled integer metric. |
| `Metrics_Inc` | `const name[]` | `1` | Adds `1` to a metric. |
| `Metrics_IncLabeled` | `const name[], const labels[]` | `1` | Adds `1` to a labeled metric. |
| `Metrics_Add` | `const name[], Float:value` | `1` | Adds a floating-point value. |
| `Metrics_AddInt` | `const name[], value` | `1` | Adds an integer value. |
| `Metrics_AddLabeled` | `const name[], const labels[], Float:value` | `1` | Adds a floating-point value to a labeled metric. |
| `Metrics_AddLabeledInt` | `const name[], const labels[], value` | `1` | Adds an integer value to a labeled metric. |
| `Metrics_ObserveHistogram` | `const name[], Float:value` | `1` | Records a histogram observation. |
| `Metrics_ObserveHistogramLabeled` | `const name[], const labels[], Float:value` | `1` | Records a labeled histogram observation. |
| `Metrics_ObserveSummary` | `const name[], Float:value` | `1` | Records a summary observation. |
| `Metrics_ObserveSummaryLabeled` | `const name[], const labels[], Float:value` | `1` | Records a labeled summary observation. |
| `Metrics_Remove` | `const name[]` | `1` | Removes one metric. |
| `Metrics_RemoveLabeled` | `const name[], const labels[]` | `1` | Removes one labeled metric. |
| `Metrics_Clear` | none | `1` | Clears all metrics. |
| `Metrics_SetServerInfo` | `const hostname[], const gamemode[], maxplayers` | `1` | Updates server info metrics. |

## Config File

Example `scriptfiles/pawn_metrics.cfg`:

```ini
bind=127.0.0.1
port=9100
auth_token=change-this-token
```

Usage:

```pawn
public OnGameModeInit()
{
    if (!Metrics_StartFromConfig("scriptfiles/pawn_metrics.cfg"))
    {
        print("[pawn-metrics] failed to start from config");
    }

    return 1;
}
```

## Authentication

If `auth_token` is empty, the endpoint is public. If a token is configured, requests must use one of these forms:

```text
Authorization: Bearer change-this-token
http://127.0.0.1:9100/metrics?token=change-this-token
```

For production servers, bind to `127.0.0.1` or firewall the metrics port unless Prometheus runs on a trusted remote host.

## Stock Helpers

The include provides convenience stocks:

```pawn
Metrics_UpdateServerInfo();
Metrics_SetPlayersOnline();
```

`Metrics_SetPlayersOnline` loops over `GetMaxPlayers()` and writes:

```text
samp_players_online
```

## Metric Naming

Metric names must be Prometheus-safe:

```text
[a-zA-Z_:][a-zA-Z0-9_:]*
```

Good examples:

```text
samp_players_online
samp_connects_total
samp_disconnects_total
samp_commands_total
samp_reports_total
samp_anticheat_flags_total
samp_money_created_total
samp_money_removed_total
samp_mysql_queries_total
samp_loaded_vehicles
samp_loaded_houses
```

Invalid names are ignored by the plugin.

## Prometheus

Example `prometheus.yml`:

```yaml
scrape_configs:
  - job_name: samp
    scrape_interval: 5s
    static_configs:
      - targets: ["127.0.0.1:9100"]
```

Example output:

```text
# HELP pawn_metrics_info Pawn metrics plugin scrape info
# TYPE pawn_metrics_info gauge
pawn_metrics_info 1
# TYPE samp_connects_total gauge
samp_connects_total 18
# TYPE samp_players_online gauge
samp_players_online 7
```

## Grafana Ideas

| Panel | Metric |
| --- | --- |
| Online players | `samp_players_online` |
| Connects per minute | `rate(samp_connects_total[1m])` |
| Disconnects per minute | `rate(samp_disconnects_total[1m])` |
| Commands per minute | `rate(samp_commands_total[1m])` |
| Anti-cheat flags | `rate(samp_anticheat_flags_total[5m])` |
| Money created | `rate(samp_money_created_total[5m])` |
| Money removed | `rate(samp_money_removed_total[5m])` |

## Build From Source

This repository uses a simple `Makefile`.

### Requirements

| Target | Requirement |
| --- | --- |
| Linux `.so` | `g++`, `make`, SA-MP plugin SDK |
| Windows `.dll` | `i686-w64-mingw32-g++` or compatible MinGW toolchain, `make`, SA-MP plugin SDK |

By default, the Makefile expects:

```text
/home/rch/Documents/project-main/build-linux-x64/_deps/samp-plugin-sdk-src
```

Override it for public builds:

```bash
make linux SDK_DIR=/path/to/samp-plugin-sdk
make windows SDK_DIR=/path/to/samp-plugin-sdk
```

### Linux

```bash
make linux
```

Output:

```text
dist/pawn_metrics.so
```

### Windows

```bash
make windows
```

Output:

```text
dist/pawn_metrics.dll
```

The default Windows target is 32-bit because classic SA-MP Windows servers are 32-bit.

### Package

```bash
make package
```

## GitHub Actions

The workflow in `.github/workflows/build.yml` fetches the SA-MP plugin SDK and builds release artifacts for Linux and Windows.

## Troubleshooting

| Problem | Fix |
| --- | --- |
| `Failed (plugins/pawn_metrics.so: cannot open shared object file)` | Check that the file is inside `plugins/` and the plugin name matches your config. |
| `undefined symbol: Metrics_Start` while compiling Pawn | Make sure `pawn_metrics.inc` is in your compiler include path. |
| `/metrics` does not open | Confirm `Metrics_Start(port)` returned `1`, the port is not already used, and firewall rules allow local scraping. |
| Prometheus cannot scrape | Use the server's reachable IP and port, not always `127.0.0.1`, when Prometheus runs on another machine. |
| Windows server cannot load DLL | Use the 32-bit DLL for classic SA-MP Windows server. |

## Security Notes

The metrics endpoint supports a simple bearer/query token. Bind or firewall it so only Prometheus, Grafana, or trusted monitoring hosts can access it.

Do not expose sensitive data such as passwords, auth tokens, private IPs, or player personal information as metric names or values.

## License

Pawn Metrics is released under the MIT License.
