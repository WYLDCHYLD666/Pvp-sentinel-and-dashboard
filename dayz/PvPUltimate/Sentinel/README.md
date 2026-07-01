# PvPUltimate Sentinel Module

This folder contains the Sentinel telemetry module for the stable PvPUltimate DayZ mod.

Sentinel should be copied into the existing PvPUltimate source tree and included by that mod. It is not intended to replace PvPUltimate.

## First target

Write JSON files to the server config/profile area:

```text
config/PvPSentinel/status.json
config/PvPSentinel/players.json
config/PvPSentinel/incidents.json
config/PvPSentinel/punishments.json
```

## Safety rule

Do not change PvPUltimate enforcement behaviour in v0.1.0. Only export data.
