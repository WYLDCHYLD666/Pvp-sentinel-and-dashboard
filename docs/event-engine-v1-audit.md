# Sentinel Event Engine V1 Architecture Audit

Date: 18 September 2026

## Scope

This audit was completed before Event Engine V1 implementation. No Event Engine code was changed during the audit. Existing zone, punishment, Purge, telemetry, map, and dashboard behaviour must remain backward compatible.

## Current architecture

Sentinel is currently split across three working areas:

- `sentinel-mod/`: authoritative DayZ server telemetry, player lifecycle tracking, death capture, zone contract loading, and JSON exporters.
- `sentinel-ui/`: client-facing HUD/map presentation plus the current scheduler, Purge, damage enforcement, and punishment integrations.
- The deployed dashboard working copy: PHP JSON APIs and a browser JavaScript dashboard. The current live working copy is not yet stored in this Git repository.

Telemetry is transported as JSON files under `$profile:Sentinel/`, synchronised to the dashboard through FTP/SFTP, and served from per-server dashboard directories.

## Existing event functionality

`SentinelEventStore` is already a partial event engine and should be extended instead of replaced. It currently provides:

- `player_connected`
- `player_disconnected`
- `player_died`
- `player_killed`
- UTC timestamps
- per-session IDs
- event IDs formed from session ID and sequence
- ordered sequence numbers
- a 1,000-event in-memory retention limit
- `events.json` schema version 2
- killer and victim identities
- killer and victim positions
- weapon class and display-name fields
- kill distance
- basic death-cause classification

`SentinelPlayerBase` records recent hit context in `EEHitBy` and captures authoritative deaths in `EEKilled`. It attempts direct player and weapon hierarchy resolution and distinguishes human PvP from non-human or unknown deaths. Unknown deaths are not automatically classified as PvP.

Current attribution limitations:

- The recorded weapon may be the damage-source object class rather than the weapon actually held by the killer.
- Headshot and exact projectile/ammunition attribution are not yet reliable.
- Optional combat fields must therefore remain nullable until they can be proven from real DayZ data.

## Zones, rules, punishment, and Purge

`SentinelPvPZoneManager` already supports circle and polygon zones, stable zone IDs, priorities, winning-zone resolution, contract revisions, content hashes, and five-second player-zone scans.

Zone entry and exit are detected but currently written only to the server log. They are not added to `events.json`.

The Rule Scheduler and SentinelUI currently implement:

- scheduled profile publication through `schedule.json`
- Purge countdown and siren
- Purge start/end state
- whole-map PvP
- base/container damage switching
- map-zone hiding during Purge
- Purge RPC and HUD presentation
- prohibited-shot warnings and third-warning kick

These actions are logged but are not yet connected to a shared canonical event stream.

## Dashboard functionality already present

The dashboard already has UI-level support for:

- Timeline rendering and filtering
- PvP/death recognition
- combat map markers
- expiring tactical markers
- incident detail cards
- event counters and death breakdowns
- per-server telemetry directories

This means the Timeline and tactical-map presentation are ahead of the persistence and API layers. They should be adapted to the Event Engine API rather than redesigned.

## Storage and deduplication

There is currently no database. Dashboard data is stored as flat JSON under `dashboard/data/servers/{serverId}/`.

The DayZ server recreates its in-memory event collection at startup and rewrites `events.json`. Durable cross-session history is therefore not guaranteed.

Source event IDs are useful for deduplication, but the dashboard currently has no ingestion ledger, unique constraint, or durable idempotency mechanism. Repeated FTP polling can read the same source event more than once.

The recommended idempotency constraint is:

```text
UNIQUE(server_id, source_event_id)
```

## API status

The deployed dashboard currently exposes combined snapshot and tactical endpoints. It does not yet provide a dedicated Event API with pagination, type/player/date filters, player combat statistics, or server-side analytics queries.

## Security finding

The largest implementation risk is server isolation.

Several dashboard endpoints accept a browser-supplied `serverId`, sanitise its characters, and then read the corresponding server directory. They do not consistently verify that the authenticated user is authorised to access that server.

The workspace/server list is primarily maintained in browser `localStorage`. Team invitations exist, but they are not a complete server-side authentication and authorisation boundary.

Historical Event Engine endpoints must not be exposed until server ownership/access is validated server-side. Client-supplied `serverId` values must never be trusted by themselves.

## Additional compatibility risks

- The Git worktree contains existing uncommitted SentinelUI and dashboard-bridge work that must be preserved.
- The live dashboard source is outside the Git repository, despite repository documentation referring to dashboard directories.
- Sentinel core and SentinelUI both contain server-side behaviour, so event ownership must be explicitly defined.
- JSON is manually emitted by Enforce Script; user-derived strings need safe JSON escaping.
- No automated test framework is currently present.
- No PHP dependency manager or database abstraction is present.
- Browser analytics currently operate on a maximum 1,000-event snapshot.
- Synthetic test events require an explicit marker and must be excluded from production statistics by default.

## Recommended Event Engine V1 design

1. Preserve `SentinelEventStore` as the authoritative DayZ event producer.
2. Extend schema version 2 compatibly rather than replacing it.
3. Add server ID, actor/target fields, zone/rule/Purge context, optional metadata, and an explicit synthetic flag.
4. Temporarily accept existing lowercase event names through a canonical normalisation layer.
5. Connect zone, punishment, scheduler, and Purge transitions through small adapter calls.
6. Add a dashboard ingestion service that imports every source event exactly once.
7. Prefer SQLite through PHP PDO if the production host supports it. This gives migrations, transactions, indexes, and unique constraints without introducing a separate database service.
8. Preserve existing JSON snapshot APIs for backward compatibility.
9. Add server-side authentication and server-access checks before exposing event history.
10. Connect the existing Timeline and tactical-map components to the paginated Event API.
11. Derive analytics and player statistics from canonical stored events.

## Recommended implementation order

The first safe implementation slice should contain:

1. Bring the current dashboard source into version control without deployment credentials or telemetry data.
2. Establish an authenticated server-access boundary.
3. Verify production PHP PDO/SQLite support.
4. Add a migration runner and canonical event table.
5. Add an idempotent importer for the existing `events.json` schema.
6. Add a read-only, server-scoped Event API.
7. Add tests for validation, deduplication, filtering, permissions, and server isolation.

Only after that baseline passes should the DayZ producer be extended for zone, rule, punishment, Purge, and improved combat events.

## Proposed validation checkpoints

- Existing DayZ and SentinelUI scripts still compile.
- Normal login, zone HUD, Expansion map overlays, punishment escalation, Purge, siren, and damage enforcement still work.
- Repeated sync of the same `events.json` inserts no duplicates.
- A user cannot retrieve another server's events by changing `serverId`.
- Unknown deaths remain non-PvP.
- Synthetic events are visibly marked and excluded from normal analytics.
- Restarting DayZ and the dashboard does not duplicate previously ingested events.

