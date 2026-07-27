# PvP Sentinel and Dashboard

Sentinel is a DayZ server-side PvP enforcement, telemetry, investigation and administration platform.

## Current development status

The active dashboard milestone is **v2.0.0-dev9.7 — Operation Overwatch**.

Completed dashboard architecture:

- Dev5 — modular Map Engine
- Dev6 — Zone Manager
- Dev7 — Professional Editor
- Dev8 — GIS Tools
- Dev9 — Live Tactical View
- Dev9.1–Dev9.6 — follow, trail and fast-polling stabilisation
- Dev9.7 — Combat Event Timeline

The current tactical dashboard supports live players, smooth interpolation, followed-player trails, player fitting, gunshot/death/vehicle markers, zone overlays and a dedicated 2-second tactical telemetry cycle.

## Combat Event Timeline

Dev9.7 adds a live investigation timeline for:

- gunshots
- player hits and damage
- PvP kills
- deaths
- unconscious events
- vehicle activity
- zone entry and exit activity

Events containing coordinates can be selected to open and centre the tactical map on the incident location.

## Project layout

```text
Pvp-sentinel-and-dashboard/
├── dayz/        # DayZ server mod source
├── api/         # PHP REST and telemetry endpoints
├── dashboard/   # Web dashboard frontend
├── agent/       # Optional server-to-web sync agent
├── docs/        # Architecture, setup and roadmap
├── tools/       # Helper and release scripts
└── releases/    # Packaged builds and release notes
```

## Deployment model

```text
DayZ Server
    │
    ├── @Sentinel loaded server-side
    ├── writes JSON telemetry
    └── sync process publishes telemetry over HTTPS
            │
            ▼
       PHP telemetry API
            │
            ├── full snapshot refresh: 15 seconds
            └── tactical refresh: 2 seconds
                    │
                    ▼
             Sentinel Dashboard
```

## Branch policy

- `main` remains the deployable stable baseline.
- `develop` contains active integrated development.
- Feature work is built on dedicated branches and squash-merged after testing.

## Roadmap

Operation Atlas established the reusable dashboard architecture. Operation Overwatch is adding tactical investigation, followed by replay, analytics and plugin support.

See `CHANGELOG.md` and `docs/OPERATION-OVERWATCH.md` for the current project record.
