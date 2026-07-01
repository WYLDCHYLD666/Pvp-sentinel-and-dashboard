# Sentinel Architecture

PvPUltimate is the stable DayZ PvP enforcement mod. Sentinel is added as a telemetry and dashboard layer.

## Rule

Sentinel must not change gameplay behaviour in v0.1.0.

It only exports data that PvPUltimate already knows about.

## Flow

```text
PvPUltimate enforcement logic
        ↓
Sentinel module hooks
        ↓
config/PvPSentinel/*.json
        ↓
Web hosting importer/API
        ↓
Dashboard
```

## GPORTAL output path

On the server this should appear under the config/profile area:

```text
config/PvPSentinel/
├── status.json
├── players.json
├── incidents.json
└── punishments.json
```

## Integration approach

PvPUltimate should call Sentinel from existing safe points:

```c
PvUSentinelLogger.WriteStatus("RESTRICTED");
PvUSentinelLogger.LogIncident(attackerId, victimId, reason, false);
PvUSentinelLogger.LogPunishment(playerId, "WARNING", "Illegal PvP");
```

The actual punishment, warning, kick, ban, and zone logic remains inside PvPUltimate.
