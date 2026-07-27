# Operation Overwatch

Operation Overwatch builds Sentinel's live tactical investigation layer on top of the completed Operation Atlas dashboard architecture.

## Completed

### Dev9 — Tactical View
- Smooth player interpolation
- Follow player
- Movement trails
- Gunshot markers
- Death markers
- Vehicle markers
- Tactical layer controls
- Fit players

### Dev9.1–Dev9.6 — Stabilisation
- Direct tactical toolbar bindings
- Persistent follow state
- Reliable pointer interaction on rapidly refreshed markers
- Rolling two-minute followed-player trail history
- Trail hidden immediately when follow mode is stopped
- Lightweight two-second tactical telemetry polling
- Fifteen-second full dashboard polling retained for heavier datasets

### Dev9.7 — Combat Event Timeline
- Live combat-focused event stream
- Shot, hit, kill, death, unconscious, vehicle and zone activity descriptions
- Event filtering
- Click-to-map incident navigation when coordinates are present
- Timeline updates through the fast tactical refresh cycle

## Next planned milestones

### Dev10 — Incident Replay
- Select a time range or combat incident
- Replay players and events on the tactical map
- Play, pause, seek and speed controls
- Timeline becomes the replay index

### Dev11 — Analytics
- PvP hotspots
- Death and weapon trends
- Zone activity
- Player behaviour and incident summaries

### Dev12 — Plugin SDK
- Extension hooks
- Custom tactical layers
- Custom event renderers
- External administration integrations

## Telemetry requirements

The dashboard can only display data that the DayZ mod and synchronisation layer publish. Rich timeline events should include, where available:

```json
{
  "type": "player_hit",
  "timestamp": "2026-07-27T18:42:17Z",
  "attacker_name": "PlayerA",
  "victim_name": "PlayerB",
  "weapon": "M4A1",
  "position": { "x": 7500, "z": 7500 }
}
```

Coordinates enable direct investigation navigation from the event timeline to the tactical map.
