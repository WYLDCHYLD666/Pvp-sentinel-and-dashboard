# Sentinel Dev13.3 — World Package Builder

## Added

- Five-step World Package Builder wizard.
- Official DayZ world presets for Chernarus+, Livonia and Sakhal.
- Custom/community world preset.
- Import-source selection foundation.
- Guided source scanning and validation.
- Readiness percentage and validation summary.
- Generation progress display and detailed log.
- Package result screen with checksum, signature state and output path.
- SHA-256 checksum generation for each world manifest.
- Protected HMAC-SHA256 signing foundation using `/home/sentinel/config/world-signing.key` when configured.
- Direct actions to refresh World Library and open the active world.

## Security and licensing boundary

Dev13.3 does not download or extract proprietary DayZ or third-party map data. The builder accepts legally obtained, user-created or licensed derived assets from the staging folder.

## Compatibility

Map Providers, World Library, Live Map, Zone Designer, telemetry and the Dev13.2 provider framework remain compatible.
