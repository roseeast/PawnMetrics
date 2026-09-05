# open.mp Native Component Mode

Pawn Metrics supports running directly as a native **open.mp Component** or via the classic **SA-MP Legacy Plugin** ABI.

## Status

| Item | Status |
| --- | --- |
| Shared metrics core | Implemented in `src/metrics_store.*` and `src/metrics_server.*` |
| Shared native bindings | Implemented in `src/natives.*` |
| Pawn include file | Ready in `include/pawn_metrics.inc` |
| Legacy plugin target | Implemented (`plugins/pawn_metrics.so`, `plugins/pawn_metrics.dll`) |
| Native component target | Implemented (`components/pawn_metrics.so`, `components/pawn_metrics.dll`) |

## Installation (open.mp)

### Option A: Native Component (Recommended for open.mp)

1. Copy the component binary into your open.mp server `components/` folder:
   - Linux: `components/pawn_metrics.so` -> `<server>/components/pawn_metrics.so`
   - Windows: `components/pawn_metrics.dll` -> `<server>/components/pawn_metrics.dll`
2. Add `pawn_metrics` to the `components` list in your server's `config.json`:
   ```json
   {
     "components": [
       "pawn_metrics"
     ]
   }
   ```
3. Include `pawn_metrics.inc` in your Pawn gamemode or filterscripts.

### Option B: Legacy Plugin Mode

1. Copy the plugin binary into your server `plugins/` folder (`plugins/pawn_metrics.so` or `plugins/pawn_metrics.dll`).
2. Add `pawn_metrics` to `pawn.legacy_plugins` in `config.json`:
   ```json
   {
     "pawn": {
       "legacy_plugins": [
         "pawn_metrics"
       ]
     }
   }
   ```

## Component Architecture

- Class `PawnMetricsComponent` inherits `IComponent` and `PawnEventHandler`.
- Unique UID: `0x5061776e4d747263` (`"PawnMtrc"`).
- Implements `onLoad`, `onInit` (caches AMX function table from `IPawnComponent`), `onAmxLoad` (registers `Metrics_*` natives into every loaded script), and `free` (stops HTTP server and detaches event listeners cleanly).
- Exports standard open.mp entrypoint `ComponentEntryPoint`.

## Building

- Linux native component: `make openmp-linux`
- Windows native component: `make openmp-windows`
- Full package: `make all && make windows && make package`
