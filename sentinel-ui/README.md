# SentinelUI v0.3.0 Purge scheduler alpha 8 - complete zone cards

Trader, Admin, Event, Radiation, and Raid transitions retain their zone type instead of falling back to Safe. Their HUD cards use the supplied transparent `_ca.paa` assets and the matching dashboard palette.

The purge siren uses a validated 44.1 kHz, mono, 16-bit PCM WAV with DayZ's non-spatial sound configuration. Runtime logging now verifies both the sound set and sound shader before requesting playback.

This build preserves the confirmed-working v0.2.8 hatched Expansion-map zones and adds the first read-only Rule Scheduler bridge for Purge presentation.

Alpha 4 replaces the WAV that DayZ failed to start with a validated 18-second mono Ogg Vorbis file. It also mirrors the dashboard's exact type palette for PvP, safe, trader, event, radiation, admin, raid, and custom zones instead of showing every non-PvP/non-safe zone as yellow.

## Behaviour

- The server-side part of SentinelUI polls `https://sentineladmin.co.uk/dashboard/api/schedule.php` every 10 seconds.
- A schedule profile is treated as Purge only when its `pvp_mode` is exactly `WHOLE_MAP_KOS`.
- The server calculates the dashboard's timezone-qualified `next_transition.at` against UTC.
- At ten seconds before a Purge start, all connected clients receive authoritative `10` through `1` countdown events.
- At the transition, clients receive one Purge-start event, display a centre-screen warning, play the bundled 18-second original siren, and change the lower HUD card to `PURGE / WHOLE MAP PVP`.
- At the Purge end, the normal SAFE/PVP zone card is restored.
- Players joining while Purge is already active receive a silent state sync, so the siren is not replayed on every login.
- If the dashboard endpoint is temporarily unavailable, SentinelUI keeps its last accepted state and throttles failure messages in the server RPT.

## Scope boundary

This alpha links scheduler state to player-facing UI only. It does not change Sentinel core and does not yet implement server-side damage, raiding, or punishment enforcement from scheduler profiles.

The siren is an original generated asset. The supplied YouTube clip was used only as a tonal reference and is not included or redistributed.

## Expected RPT markers

- `[SentinelUI] Purge scheduler bridge started endpoint=...`
- `[SentinelUI] Purge scheduler initial state active=...`
- `[SentinelUI] next Purge transition=...`
- `[SentinelUI] Purge countdown 10` through `1`
- `[SentinelUI] Purge started at scheduled transition event=...`
- Client: `[SentinelUI] Purge siren requested on local player effect channel`
- Client: `[SentinelUI] Purge siren playback confirmed`

Rollback baseline: `SentinelUI-v0.2.8-hatched-zones.zip`.
