# Changelog

All notable changes to Sentinel are recorded in this file.

## [2.0.0-dev9.7] - 2026-07-27

### Added
- Combat Event Timeline for shots, hits, kills, deaths, unconscious events, vehicles and zone activity.
- Timeline filtering for combat activity, all events, PvP kills, deaths, connections and disconnections.
- Click-to-map investigation navigation for events containing coordinates.
- Live timeline refresh through the 2-second tactical telemetry cycle.

### Preserved
- Follow mode and persistent followed-player movement trails.
- Stop-follow trail visibility behaviour.
- Player fitting and tactical map controls.
- Modular Map Engine, Zone Manager, Professional Editor and GIS tools.

## [2.0.0-dev9.6] - 2026-07-27

### Performance
- Added lightweight tactical telemetry polling every 2 seconds.
- Kept the full dashboard snapshot on its stable 15-second refresh cycle.
- Added fallback to the full snapshot endpoint when tactical polling is unavailable.

## [2.0.0-dev9.5] - 2026-07-27

### Fixed
- Followed-player trail is visible only while follow mode is active.
- Stopping follow mode clears the rendered trail immediately.

## [2.0.0-dev9.4] - 2026-07-27

### Fixed
- Movement trails now append to a rolling history instead of replacing the previous segment.
- Added a two-minute trail history with bounded point retention.

## [2.0.0-dev9.3] - 2026-07-27

### Fixed
- Player follow activation moved to pointer-down to survive rapidly refreshed tactical markers.
- Single-player Fit Players zoom reduced to a practical level.

## [2.0.0-dev9] - 2026-07-27

### Added
- Operation Overwatch live tactical view.
- Smooth player interpolation.
- Player follow mode and trails.
- Gunshot, death and vehicle markers.
- Tactical layer controls and player fitting.

## [2.0.0-dev8] - 2026-07-27

### Added
- GIS grid, snapping, distance measurement, coordinate inspection and precision editing.
- Layer visibility, Fit Selection and Fit All Zones.

## [2.0.0-dev7] - 2026-07-27

### Added
- Professional zone editing with selection, dragging, resizing, polygon editing, undo/redo and box selection.

## [2.0.0-dev6] - 2026-07-27

### Added
- Modular Zone Manager extracted from the main application.

## [2.0.0-dev5] - 2026-07-27

### Added
- Modular Map Engine and Operation Atlas architecture baseline.

## [0.6.1] - 2026-07-10

### Added
- Clean Sentinel baseline source.
- Dedicated Sentinel Development Environment workflow.
- Server-side profile/config folder creation.

### Fixed
- Fixed `PvPManager::IsWeekend()` so every code path returns a boolean value.
- Resolved client login failure by deploying Sentinel as a server-only mod.

### Deployment
- Sentinel must be loaded by the DayZ server only.
- Players must not load `@Sentinel` in their client mod list.
