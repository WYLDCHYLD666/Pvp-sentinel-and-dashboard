# Sentinel Dashboard Stable Baseline

Current validated dashboard baseline: **Dev13.7c**.

Validated behaviour:

- Native XYZ world packages install, verify, activate and uninstall.
- Topographic and Satellite multi-zoom tile pyramids load through zoom 6.
- Live Map and Zone Designer retain zoom and pan.
- Live Map and Zone Designer use unified native-tile rendering.
- Zone polygon, midpoint and circle-radius edit handles stay a constant screen size while zooming.

The deployment archive generated during development is named:

`Sentinel-Dev13.7c-Fixed-Size-Zone-Handles-Polygon-UI-Cleanup.zip`

SHA-256:

`09b1b03a157d27b87924b7df5b6614341aec7668dbb797c9fedd68d470512b99`

Apply `patches/DEV13.7c.patch` on top of the Dev13.7b dashboard source snapshot.
