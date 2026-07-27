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

## Public release

Status: Future

- Stable installer and documentation.
- Release packaging and signed builds.
- Upgrade and rollback procedure.
- Security and reliability review.
