# Sentinel Roadmap

## v0.6.1 — Stable server-only login baseline

Status: Complete

- Server-only deployment confirmed.
- Player login confirmed.
- Clean baseline established.
- Sentinel profile/config directory created.

## v0.6.2 — Live JSON Export Engine

Status: Complete

- Export live server state to `status.json`.
- Export online players to `players.json`.
- Export configured PvP zones to `zones.json`.
- Maintain valid JSON even when no records exist.
- Add timestamps and schema versions.
- Confirm login after every exporter change.

## v0.6.3 — PvP telemetry

Status: In progress

- Record legal and illegal PvP incidents.
- Record warnings and punishments.
- Add event identifiers and timestamps.
- Prevent unbounded JSON file growth.
- Add killer, victim, weapon, distance, damage and position metadata.

## Operation Atlas — Mapping and GIS

Status: Active

- Dev5: modular map engine.
- Dev6: extracted zone manager.
- Dev7: professional zone editor.
- Dev8: GIS tools, measuring, snapping, coordinates and layers.

### High-resolution game-data map pipeline

Status: Planned — high priority

Build the Sentinel basemap directly from DayZ world data so the dashboard does not depend on a low-resolution single image.

- Extract terrain, roads, paths, water, forests and settlement data from the supported DayZ world.
- Extract building and object positions so individual houses and major structures become visible at high zoom levels.
- Generate a multi-resolution tile pyramid rather than one oversized image.
- Add progressive zoom levels, from full-map overview down to town, street and building detail.
- Preserve exact DayZ world coordinates so players, zones, events and measurements align with the generated map.
- Add named locations, roads, landmarks and optional contour/elevation layers.
- Support layer toggles for terrain, buildings, roads, vegetation, water and tactical overlays.
- Add a repeatable map-generation tool so tiles can be rebuilt after world or object updates.
- Cache generated tiles on the web server for fast loading.
- Design the pipeline to support ChernarusPlus first and additional maps later.
- Investigate legal redistribution limits before publishing extracted game assets; prefer generated geometry and locally produced tiles where required.

Definition of done:

- Houses and major structures are clearly visible when zoomed in.
- The map remains sharp at every supported zoom level.
- Sentinel coordinates line up accurately with roads, buildings and terrain features.
- Live players, trails, zones and timeline events remain correctly positioned.
- Map loading remains responsive on desktop and mobile connections.

## Operation Overwatch — Tactical View

Status: Active

- Dev9: live tactical player view.
- Dev9.1–Dev9.5: interaction, follow and trail stabilisation.
- Dev9.6: two-second lightweight tactical polling.
- Dev9.7: live combat event timeline with map-centred incident navigation.

## Dev10 — Incident Replay

Status: Planned

- Reconstruct player movement and combat events over time.
- Timeline scrubbing and playback controls.
- Jump from a combat event to its replay position.
- Display attackers, victims, shots, hits, deaths and zone transitions.

## Dev11 — Investigation Analytics

Status: Planned

- Player and incident heatmaps.
- Repeated-offender and hotspot detection.
- Weapon, distance and zone analytics.
- Investigation summaries and exportable reports.

## Dev12 — Integrations and extensibility

Status: Planned

- Multiple server support.
- Discord integration.
- Plugin and webhook architecture.
- Administration and punishment workflows.

## Dev13 — Server Integration and In-Game Experience

Status: Planned — required before public release

Connect the completed dashboard features back into the DayZ server mod so Sentinel becomes one coordinated system rather than separate web and game components.

### Dashboard-to-server control channel

- Add an authenticated server-side agent that downloads approved Sentinel configuration changes over HTTPS.
- Synchronise PvP zones, schedules, purge-night settings and notification preferences from the dashboard to the DayZ server.
- Use revision numbers, checksums and acknowledgements so the dashboard can confirm which configuration is active on the server.
- Keep the last known-good local configuration if the web service is unavailable or returns invalid data.
- Write audit events whenever a configuration is published, received, rejected or activated.
- Require explicit publish and activation actions so editing a draft cannot immediately change the live server.

### In-game zone map integration

- Display Sentinel PvP zones on the player's in-game map.
- Support circle and polygon zones using the same coordinates as the dashboard Zone Manager.
- Use distinct map colours and labels for safe zones, PvP zones, event zones and purge-wide PvP status.
- Update map markers when the server activates a new zone revision.
- Hide restricted administrative metadata from normal players.
- Ensure servers can disable in-game zone drawing when desired.

### Zone entry and exit notifications

- Detect each player's transition into and out of every enabled zone.
- Show clear in-game notifications containing the zone name and current rules.
- Prevent notification spam by only alerting on a genuine boundary transition and applying a short debounce near zone edges.
- Support configurable messages, colours and display duration.
- Optionally play a short approved notification sound for entry and exit.
- Record zone-entered and zone-exited events for the dashboard timeline and replay system.

Example player messages:

```text
ENTERING PVP ZONE — Kamenka
Weapons free. PvP rules are active.

LEAVING PVP ZONE — Kamenka
Safe-area rules are active.
```

### Purge-night scheduler and in-game announcement

- Link purge nights to the Rule Scheduler and the authoritative server clock.
- Support scheduled start and end times, recurring rules, manual activation and emergency cancellation.
- Announce upcoming purge nights at configurable intervals such as 30, 10, 5 and 1 minute before activation.
- At activation, switch the server to global PvP and publish the new state to the dashboard immediately.
- Display a full-screen or prominent in-game purge announcement.
- Play a configurable server-provided purge siren or announcement sound to connected players.
- Use an original, licensed or server-owned audio asset rather than distributing copyrighted film or television audio without permission.
- Allow volume, duration and replay behaviour to be configured; do not loop indefinitely.
- Notify players who connect while purge mode is already active.
- At the scheduled end, restore the normal zone rules, play an optional all-clear sound and notify players.
- Record purge scheduled, warning, started, cancelled and ended events in Sentinel telemetry.

### Reliability and safety requirements

- The DayZ server remains authoritative for active rules and enforcement.
- Dashboard failure must never disable PvP enforcement or corrupt the live zone configuration.
- Configuration parsing must fail closed and retain the previous valid configuration.
- Every server-side integration change must pass server-start and player-login tests.
- Purge activation and cancellation must be idempotent so duplicate sync messages cannot trigger duplicate state changes or repeated sounds.
- Provide an administrator kill switch to disable remote configuration while retaining local enforcement.

Definition of done:

- A zone created and published in the dashboard appears accurately on the DayZ server and in the in-game map.
- Players receive one correct entry notification and one correct exit notification per transition.
- Dashboard, server enforcement, in-game map and telemetry all report the same active zone revision.
- A scheduled purge produces advance warnings, the configured start sound and announcement, global PvP activation, dashboard state updates and a clean scheduled end.
- Restarting the server during an active purge restores the correct state from the schedule and persisted configuration.
- Loss of web connectivity does not interrupt existing server rules.

## Public release

Status: Future

- Stable installer and documentation.
- Release packaging and signed builds.
- Upgrade and rollback procedure.
- Security and reliability review.
