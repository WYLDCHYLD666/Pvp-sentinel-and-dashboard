# GPORTAL Setup Notes

## Current confirmed setup

The DayZ server runs on GPORTAL and the Sentinel files are generated server-side by the PvPUltimate Sentinel module.

## Expected output folder

On the server FTP, check for:

```text
PvPSentinel/
```

Inside it should be:

```text
status.json
players.json
incidents.json
punishments.json
```

## v0.4.0 behaviour

- `status.json` is written by Sentinel.
- `players.json` exists but is not yet populated with full live player details.
- `incidents.json` exists but is not yet hooked into PvP events.
- `punishments.json` exists but is not yet hooked into warnings/kicks/bans.

## Next install step

For v0.5.0, replace the Sentinel patch files in PvPUltimate and restart the server.

After restart, check:

```text
PvPSentinel/players.json
```

It should contain real online player entries.
