# Sentinel v0.4.0

## Status

First working Sentinel foundation for PvPUltimate.

## Confirmed working on GPORTAL

- PvPUltimate compiles again after the `GetWeekDay` fix.
- Sentinel creates the `PvPSentinel/` folder.
- Sentinel creates the dashboard JSON files.

## Files created by the server

```text
PvPSentinel/
├── status.json
├── players.json
├── incidents.json
└── punishments.json
```

## Scope

This release is intentionally conservative.

- No gameplay rule changes.
- No PvP enforcement changes.
- No punishment logic changes.
- No dashboard API yet.

## Next version

v0.5.0 will add live player tracking to `players.json`.
