# Sentinel v0.7.1a — Stable telemetry heartbeat and event foundation

Status: Verified on the Sentinel development server.

## Added

- Persistent `build_info.json` metadata.
- Live `heartbeat` field in `status.json` using server uptime seconds.
- Empty versioned `events.json` event-stream schema.
- Modular telemetry architecture.
- Player heading, health, blood, shock, and session online time.

## Verified

- Server starts without Sentinel compilation errors.
- `build_info.json` reports `v0.7.1-event-system-phase1aa`.
- `status.json` heartbeat increases during the 15-second scan.
- Online player count changes correctly.
- `players.json` continues exporting live player telemetry.
- `events.json` is created with schema version 1 and an empty event list.

## Dashboard health model

- `build_info.json` is build metadata and is written at startup.
- `status.json` is the live heartbeat source.
- Connected: heartbeat changed within 30 seconds.
- Stale: heartbeat unchanged for 30–120 seconds.
- Offline: heartbeat unchanged for more than 120 seconds or repeated read failures.

## Next

Phase 1B will add only `player_connected` events.
