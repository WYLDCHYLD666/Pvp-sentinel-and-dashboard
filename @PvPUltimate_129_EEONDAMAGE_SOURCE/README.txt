PvPUltimate 1.29 EEOnDamageCalculated SOURCE

This build avoids EEHitBy and uses EEOnDamageCalculated, the same damage hook style used by Community Online Tools.

Pack:
@PvPUltimate_129_EEONDAMAGE_SOURCE/addons/pvpultimate

Output:
@PvPUltimate/addons/pvpultimate.pbo

Rules:
- Weekend PvP is global.
- Otherwise attacker and victim must both be inside a PvP zone.
- Illegal PvP returns false from EEOnDamageCalculated, blocking damage.
- Punishment is log-safe only: warning -> simulated kick -> simulated ban.
