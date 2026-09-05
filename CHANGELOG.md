# Changelog

## 0.2.0

- Added labeled metrics.
- Added histogram observations with default buckets.
- Added summary observations with `_sum` and `_count`.
- Added auth token support for `/metrics`.
- Added custom bind address support.
- Added simple `key=value` config file support.
- Added native open.mp component target (`components/pawn_metrics.so` and `components/pawn_metrics.dll`).
- Refactored native bindings into shared layer (`src/natives.*`) used by both legacy plugin and open.mp component.
- Added clean symbol exports with `.def` files for Windows (undecorated entrypoints for SA-MP and ComponentEntryPoint for open.mp).
- Updated example script and release packaging with `plugins/` and `components/` trees.

## 0.1.0

- Initial plugin scaffold.
- Added Prometheus text endpoint at `/metrics`.
- Added basic Pawn natives for gauges and counters.
- Added Pawn include and example gamemode.
