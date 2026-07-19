# Sentinel - DayZ Server Management Platform

## Overview

Sentinel is a server-side management and telemetry platform for DayZ. It exports live server and player data to JSON for use by a web dashboard and future integrations.

## Current Stable Version

**Version:** v0.8.1  
**Status:** Stable player telemetry, lifecycle events, and death detection

## Current Features

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

## In Development

- Killer attribution
- Victim and killer identity
- Cause-of-death classification
- Weapon used
- Kill distance
- Headshot detection
- PvP event feed
- Live map integration

## Planned

- Vehicle ownership and last-known-location tracking
- Vehicle driver history
- Discord notifications
- Player history and session analytics
- Heatmaps
- REST API
- User accounts
- Multi-server management
- Server registration and licensing

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

http://sentineladmin.co.uk/dashboard/

## Repository Structure

```text
Pvp-sentinel-and-dashboard/
├── README.md
├── CHANGELOG.md
├── ROADMAP.md
├── LICENSE
├── sentinel-mod/
├── dashboard/
├── website/
├── docs/
├── releases/
└── tools/
```

## Next Milestone

**v0.8.2-beta1 - Killer Attribution**

The next build will identify who or what caused a player death while preserving the stable v0.8.1 event pipeline.
