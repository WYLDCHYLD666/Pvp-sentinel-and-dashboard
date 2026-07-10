# Sentinel Development Journal

## 2026-07-10 — v0.6.1 baseline

### Work completed
- Confirmed Sentinel can run without blocking player login.
- Confirmed the working deployment model is server-only.
- Confirmed clients must not load `@Sentinel` in their normal mod list.
- Established `main` as the stable branch and `develop` as the integration branch.
- Added the project changelog and roadmap.

### Issues found
- Loading Sentinel as a client-required mod caused PBO/mod validation failures.
- Earlier builds contained a missing return path in `PvPManager::IsWeekend()`.
- Sentinel JSON files were created but contained no useful live data.

### Lessons learned
- Sentinel should remain a server-side service unless a future feature genuinely requires client code.
- Every change must be tested for both server startup and player login.
- Build, Workshop, and server copies must never be mixed between versions.
- Each stable milestone needs a clear rollback point in Git.

### Next session
- Create a feature branch for v0.6.2.
- Implement the status exporter first.
- Verify valid JSON output.
- Test player login before adding the player exporter.
