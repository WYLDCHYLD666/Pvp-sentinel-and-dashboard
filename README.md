# Sentinel - DayZ Server Management Platform

## Overview

Sentinel is a server-side management platform for DayZ that provides
live telemetry from your server to a web-based dashboard.

### Current Features

-   Live server status
-   Live player count
-   Live player list
-   JSON telemetry generation
-   Automatic dashboard synchronization
-   PvP zone framework
-   Event logging framework

## Project Status

### Completed

-   Stable Sentinel server mod
-   Live JSON telemetry
-   Dashboard integration
-   GPortal FTP/cron synchronization
-   Live player detection
-   Live player list export

### In Development

-   Player position tracking
-   Health & blood telemetry
-   Kill feed
-   PvP event logging
-   AI event logging
-   Live map integration

### Planned

-   User accounts
-   Multi-server management
-   Server registration
-   Licensing
-   REST API
-   Discord integration

## JSON Output

    config/
    └── Sentinel/
        ├── build_info.json
        ├── events.json
        ├── players.json
        ├── status.json
        └── zones.json

## Dashboard

http://sentineladmin.co.uk/dashboard/

## Repository Structure

    Pvp-sentinel-and-dashboard/
    ├── README.md
    ├── CHANGELOG.md
    ├── ROADMAP.md
    ├── LICENSE
    ├── sentinel-mod/
    ├── dashboard/
    ├── website/
    ├── docs/
    ├── releases/
    └── tools/

## Current Milestone

**Version:** v0.6.2.6

**Status:** Stable live telemetry pipeline

Pipeline: DayZ Server -\> Sentinel Mod -\> JSON -\> GPortal Sync -\>
Dashboard
