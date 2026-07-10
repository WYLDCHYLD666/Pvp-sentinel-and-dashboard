# PvP Sentinel and Dashboard

Sentinel is a DayZ server-side PvP enforcement, telemetry, and administration platform.

The current stable baseline is **v0.6.1**. Sentinel runs as a **server-only mod** on the DayZ server and must not be loaded by players through the client mod list.

## Current status

- Server starts successfully with Sentinel enabled.
- Players can log in when Sentinel is loaded through the server mod list only.
- The Sentinel profile/config folder is created.
- JSON files are created, but live data export still needs to be connected.
- The dedicated test server is **The Sentinel Development Environment**.

## Project layout

```text
Pvp-sentinel-and-dashboard/
├── dayz/        # DayZ server mod source
├── api/         # PHP/MySQL REST API
├── dashboard/   # Web dashboard frontend
├── agent/       # Optional server-to-web sync agent
├── docs/        # Architecture, setup, roadmap, and developer journal
├── tools/       # Helper and release scripts
└── releases/    # Packaged builds and release notes
```

## Deployment model

```text
DayZ Server
    │
    ├── @Sentinel loaded as server-only
    │
    ├── writes JSON telemetry
    │
    └── Sentinel Agent sends data over HTTPS
            │
            ▼
       Web API + MySQL
            │
            ▼
        Dashboard
```

## Stable baseline

**Version:** `v0.6.1-login-baseline`

Rules for the stable branch:

1. `main` must always remain deployable.
2. Every feature is developed from `develop` on a dedicated feature branch.
3. Every change is tested on The Sentinel Development Environment.
4. Login must be tested before a change can be merged.
5. Every change requires a changelog entry and Git commit.

## Next milestone

**v0.6.2 — Live JSON Export Engine**

Expected output:

```text
$profile:Sentinel/
├── status.json
├── players.json
├── incidents.json
├── punishments.json
├── zones.json
└── config.json
```

See `ROADMAP.md`, `CHANGELOG.md`, and `docs/DEVELOPMENT_JOURNAL.md` for the full project record.
