# Changelog

All notable changes to Sentinel are documented in this file.

## v0.8.1 - 2026-07-19

### Added

- `player_died` event generation through the existing event pipeline
- UTC ISO-8601 timestamps on events
- Per-session `session_id` metadata
- Event schema version 2
- Event retention metadata with a default limit of 1000 events

### Validated

- Initial player connection
- Player disconnection
- Player reconnect
- First player death
- Multiple player deaths after respawn
- No duplicate death events
- Correct event sequence ordering
- Stable player identity across lifecycle events
- Existing player telemetry remained operational

### Stable Event Flow

```text
1  player_connected
2  player_died
3  player_disconnected
4  player_connected
5  player_died
6  player_disconnected
```

## v0.8.0 - 2026-07-19

### Added

- Event schema versioning
- Session IDs
- UTC event timestamps
- Event retention metadata

### Validated

- Player connected events
- Player disconnected events
- Player reconnect detection
- Ordered event sequence numbers
- Live player telemetry compatibility

## v0.7.1c

### Added

- Stable player lifecycle event testing baseline
- Player connected event
- Player disconnected event
- Reconnect detection
- Ordered event sequence numbers

## Planned for v0.8.2

- Killer attribution
- Victim and killer identity
- Cause-of-death classification
- Foundation for weapon, distance, and headshot telemetry
