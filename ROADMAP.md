# Sentinel Roadmap

## v0.6.1 — Stable server-only login baseline

Status: Complete

- Server-only deployment confirmed.
- Player login confirmed.
- Clean baseline established.
- Sentinel profile/config directory created.

## v0.6.2 — Live JSON Export Engine

Status: Next

- Export live server state to `status.json`.
- Export online players to `players.json`.
- Export configured PvP zones to `zones.json`.
- Maintain valid JSON even when no records exist.
- Add timestamps and schema versions.
- Confirm login after every exporter change.

## v0.6.3 — PvP telemetry

- Record legal and illegal PvP incidents.
- Record warnings and punishments.
- Add event identifiers and timestamps.
- Prevent unbounded JSON file growth.

## v0.7.0 — Dashboard API

- PHP REST endpoints.
- API authentication.
- MariaDB storage.
- Server heartbeat and status history.

## v0.8.0 — Dashboard frontend

- Server overview.
- Live players.
- PvP mode and zones.
- Incidents and punishments.
- Analytics and charts.

## v0.9.0 — Administration

- Zone management.
- Schedule management.
- Punishment review.
- Multiple server support.
- Discord integration.

## v1.0.0 — Public release

- Stable installer and documentation.
- Release packaging and signed builds.
- Upgrade and rollback procedure.
- Security and reliability review.
