# PvP Sentinel and Dashboard

Sentinel is a DayZ PvP enforcement dashboard built around the stable PvPUltimate mod.

PvPUltimate remains the authority for PvP rules, zones, punishments, and weekend PvP. Sentinel adds logging, JSON export, dashboard data, API endpoints, and future admin tooling.

## Project layout

```text
Pvp-sentinel-and-dashboard/
├── dayz/        # DayZ mod integration and PvPUltimate Sentinel module
├── api/         # PHP/MySQL REST API for hosting provider
├── dashboard/   # Web dashboard frontend
├── agent/       # Optional sync/FTP/hosting agent tooling
├── docs/        # Architecture, setup, and roadmap
├── tools/       # Helper scripts
└── releases/    # Packaged builds and release notes
```

## First milestone: v0.1.0

- Add Sentinel as a module inside PvPUltimate.
- Export server status JSON to the GPORTAL config/profile area.
- Export players, incidents, and punishments as dashboard-readable JSON.
- Keep PvPUltimate stable and avoid changing enforcement behaviour until logging is proven.

## Expected DayZ output

```text
config/PvPSentinel/
├── status.json
├── players.json
├── incidents.json
└── punishments.json
```
