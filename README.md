# Sentinel - DayZ Server Management & Investigation Platform

## Overview

Sentinel is a server-side management, telemetry, mapping, and investigation platform for DayZ. The DayZ mod exports live server/player data and lifecycle events to JSON, while the Sentinel web dashboard turns that data into operational views for server administrators.

Sentinel is designed as a server-administration platform rather than an in-game client mod. The long-term goal is to reconstruct incidents, manage PvP rules/zones, investigate player activity, and support multiple DayZ servers from one dashboard.

## Current Stable Baselines

### DayZ telemetry mod

**Version:** v0.8.1  
**Status:** Stable player telemetry, lifecycle events, and death detection

### Web dashboard

**Version:** Dev13.7c  
**Status:** Stable native map rendering, persistent map zoom/pan, and fixed-size Zone Designer editing handles

**Development branch:** `feature/universal-server-setup`

### SentinelUI client mod

**Version:** v0.3.0-alpha9
**Status:** PvP, Safe, Trader, Admin, Event, Radiation, Raid, Purge and Expansion-map zone presentation

SentinelUI is maintained separately from the server-side Sentinel core. Its source is under `sentinel-ui/`, and the signed Workshop-ready package is under `releases/SentinelUI-v0.3.0-alpha9/`.

## Current DayZ Telemetry Features

- Live server status and heartbeat
- Live player count and player list
- Player position and heading
- Player health, blood, shock, and online time
- Server-only operation
- JSON telemetry generation
- Player connected events
- Player disconnected events
- Player reconnect detection
- Player death events
- Ordered event sequence numbers
- UTC ISO-8601 timestamps
- Per-server-session IDs
- Event schema versioning
- Event retention metadata
- PvP zone framework

## Validated Event Flow

```text
player_connected
player_died
player_disconnected
player_connected
player_died
player_disconnected
```

The v0.8.1 lifecycle and death-event pipeline has been tested across multiple deaths, disconnects, and reconnects without duplicate events.

## Dashboard & Mapping

The current dashboard baseline is **Dev13.7c**.

Current map/dashboard work includes:

- Live Map
- Zone Designer / Zone Builder
- Native XYZ WebP map tiles
- Topographic and Satellite layers
- Multi-level native tile pyramids
- Per-layer native zoom limits read from tile manifests
- Persistent zoom and pan during dashboard refreshes
- Shared map rendering behaviour between Live Map and Zone Designer
- Fixed-size polygon vertex handles at every map zoom level
- Fixed-size polygon midpoint handles
- Fixed-size circle radius editing handle
- Larger invisible handle hit targets for easier editing
- World Package Builder
- World Library package verification
- World package installation, activation and uninstall workflow
- Browser caching for installed map tiles

The tested Dev13.7c dashboard baseline is stored under:

```text
Sentinel/dashboard/
```

The Dev13.7c release notes and regression test plan are stored alongside the dashboard baseline.

## World Package Workflow

The mapping workflow is designed around locally installed world packages:

```text
Approved/licensed map source
        ↓
World Package Builder
        ↓
Native XYZ tile pyramid + manifests
        ↓
World Library
        ↓
Verify package
        ↓
Install / Activate
        ↓
Live Map + Zone Designer
```

Installed worlds contain a `world.json` definition plus layer-specific `tile-manifest.json` files and XYZ WebP tiles.

### Map source permission

Sentinel's map tooling must only be configured against a map/tile provider when the server owner has permission or an appropriate licence to use that source. Provider URLs should not be hard-coded into public releases unless redistribution/use is authorised.

## JSON Output

```text
$profile/Sentinel/
├── build_info.json
├── events.json
├── players.json
└── status.json
```

## Event Example

```json
{
  "sequence": 2,
  "timestamp": "2026-07-19T09:39:09Z",
  "type": "player_died",
  "player": {
    "name": "ExamplePlayer",
    "id": "example-player-id"
  }
}
```

Timestamps are stored in UTC. The dashboard can convert them to each viewer's local timezone without changing the authoritative event time.

## Dashboard

Production dashboard: http://www.sentineladmin.co.uk/dashboard/

## Repository Structure

```text
Pvp-sentinel-and-dashboard/
├── README.md
├── CHANGELOG.md
├── ROADMAP.md
├── LICENSE
├── sentinel-mod/              # DayZ server-side Sentinel mod
├── sentinel-ui/               # Separate DayZ client-facing SentinelUI mod
├── Sentinel/
│   └── dashboard/             # Current dashboard stable baseline / release documentation
├── dashboard/                 # Existing dashboard/project files
├── website/
├── docs/
├── releases/
└── tools/
```

## Server Deployment Notes

The production dashboard currently expects the web dashboard, API, installed map library, map tooling, and configuration to be deployed in their appropriate server locations. Keep secrets/provider configuration outside public Git commits where possible.

After deploying a dashboard update, hard-refresh the browser (`Ctrl + F5`) when testing so stale JavaScript/CSS does not mask the new build.

## In Development

- Killer attribution
- Victim and killer identity
- Cause-of-death classification
- Weapon used
- Kill distance
- Headshot detection
- PvP event feed
- Tactical Live Map overlays
- Additional Zone Designer tooling

## Planned

- Vehicle ownership and last-known-location tracking
- Vehicle driver history
- Discord notifications
- Player history and session analytics
- Heatmaps
- Expanded REST API
- User accounts and permissions
- Multi-server management
- Server registration and licensing

## Next Milestones

### DayZ mod

**v0.8.2-beta1 - Killer Attribution**

The next mod build will identify who or what caused a player death while preserving the stable v0.8.1 event pipeline.

### Dashboard

Continue development from the tested **Dev13.7c** baseline. Preserve the working native map renderer, zoom persistence, and fixed-size Zone Designer handles unless a later change specifically requires modifying them.
