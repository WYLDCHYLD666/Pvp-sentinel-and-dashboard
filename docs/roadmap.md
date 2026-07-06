# Sentinel Roadmap

Sentinel is now the main product identity: a professional DayZ server management platform built around a stable server-side mod, dashboard, API, tooling, and future cloud services.

## Current Direction

- Product name: **Sentinel**
- Website: **sentineladmin.co.uk**
- Production server: **The Crazies | PvP/PvE | Hard**
- Development server: **The Sentinel Development Environment**
- Principle: **Built by server owners, for server owners**

## Development Workflow

1. Build and test new features on **The Sentinel Development Environment**.
2. Keep production server stable.
3. Promote only tested builds to production.
4. Tag working releases in GitHub.
5. Keep the Workshop item private until beta-ready.

## v0.5.x - Foundation Stabilisation

- [x] Rebrand external identity from PvPUltimate to Sentinel.
- [x] Create Sentinel config folder on the server.
- [ ] Confirm `status.json`, `players.json`, `incidents.json`, and `punishments.json` are created reliably.
- [ ] Confirm server login works with Sentinel enabled.
- [ ] Freeze first stable Sentinel foundation build.
- [ ] Document build/sign/upload process.
- [ ] Add debug startup logging.

## v0.6.0 - Live Players

- [ ] Export real online players to `players.json`.
- [ ] Include SteamID.
- [ ] Include player name.
- [ ] Include alive/dead state.
- [ ] Include position.
- [ ] Add schema version and generated timestamp.
- [ ] Test on development server before production.

## v0.7.0 - Incident Engine

- [ ] Log illegal PvP events.
- [ ] Log attacker and victim.
- [ ] Log weapon, damage, and hit zone where available.
- [ ] Log legal/illegal status.
- [ ] Add incident timeline support.
- [ ] Prepare for optional COT event enrichment.

## v0.8.0 - Punishment Engine

- [ ] Log warnings.
- [ ] Log kicks.
- [ ] Log temporary bans.
- [ ] Log permanent bans.
- [ ] Link punishments to incidents and player history.

## v0.9.0 - API and Database

- [ ] PHP API health endpoint.
- [ ] MySQL schema.
- [ ] API key authentication.
- [ ] Multi-tenant database model.
- [ ] Organisations/workspaces.
- [ ] Servers table.
- [ ] Players, incidents, punishments, and audit logs.

## v1.0.0 - Dashboard Beta

- [ ] Dark Sentinel dashboard shell.
- [ ] Live server status cards.
- [ ] Player list.
- [ ] Incident feed.
- [ ] Punishment feed.
- [ ] Basic analytics.
- [ ] Login/authentication.
- [ ] Documentation and install guide.

## Sentinel Tools

A future Windows companion tool for server owners.

- [ ] GPORTAL FTP updater.
- [ ] One-click deploy.
- [ ] Backup before upload.
- [ ] Rollback support.
- [ ] Log viewer.
- [ ] JSON config validator.
- [ ] Mod compatibility checker.
- [ ] PBO/signature/hash verification.

## Integrations

Sentinel should integrate with existing DayZ tools instead of replacing them.

- [ ] Community Framework (CF)
- [ ] Community Online Tools (COT)
- [ ] DayZ Expansion
- [ ] Trader systems
- [ ] Discord
- [ ] BattleMetrics
- [ ] GPORTAL tooling

## Sentinel Cloud - Future

A hosted service for server owners who want zero-maintenance dashboards.

- [ ] User accounts.
- [ ] Workspaces/organisations.
- [ ] Subscription support.
- [ ] Server API keys.
- [ ] Hosted dashboards.
- [ ] Multi-server dashboards.
- [ ] Cloud analytics.

## Public Release Requirements

Sentinel should remain private until it meets these standards:

- [ ] Stable install on a clean server.
- [ ] Clear documentation.
- [ ] Working dashboard.
- [ ] Working JSON/API pipeline.
- [ ] Versioned release package.
- [ ] Tested on the development server.
- [ ] Promoted safely to production.
