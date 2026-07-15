# Sentinel v0.6.2.7 — Live Player Position

Status: Verified stable on the Sentinel development server.

## Added

- Live player world position export in `players.json`.
- Numeric `x`, `y`, and `z` coordinates for each connected player.
- Position refresh through the existing 15-second player scan.

## Verified

- Server starts and remains stable.
- Player login succeeds.
- Live player count remains correct.
- Player name and identity ID remain present.
- Position values are written as valid JSON.

## Example

```json
{
  "count": 1,
  "players": [
    {
      "name": "WYLDCHYLD666",
      "id": "internal-player-id",
      "position": {
        "x": 1260.16,
        "y": 149.789,
        "z": 6314.9
      }
    }
  ]
}
```

## Rollback

Restore v0.6.2.6 and its matching signature.
