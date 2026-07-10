# Changelog

All notable changes to Sentinel will be recorded in this file.

## [Unreleased]

### Planned
- Live `status.json` export.
- Live `players.json` export.
- Zone, incident, and punishment export.

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

### Verified
- Server startup successful.
- Player login successful.
- Sentinel profile/config folder created.

### Known issue
- JSON files are created but are not yet populated with live server data.
