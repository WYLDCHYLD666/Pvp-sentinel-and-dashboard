# Dev13.3 Test Plan

1. Open World Library and locate World Package Builder.
2. Confirm the five progress steps render correctly.
3. Select Chernarus+, Livonia, Sakhal and Custom; confirm ID, name, size and staging path update.
4. Continue to Import Source and confirm only Staging Folder is enabled.
5. Validate with no raster present; confirm the builder remains incomplete.
6. Add `top.jpg` or `satellite.jpg` to `map-tools/import/<world-id>/` and validate again.
7. Confirm optional vector layers show as valid empty-layer fallbacks.
8. Generate the package and confirm the progress bar reaches 100%.
9. Confirm `maps/<world-id>/world.json`, `tile-manifest.json`, `signature.json` and layer files exist.
10. Confirm the result screen shows package name, checksum, signature state and path.
11. Refresh World Library and confirm the generated world is registered.
12. Confirm existing Live Map, Zone Designer and map-provider functionality still works.
