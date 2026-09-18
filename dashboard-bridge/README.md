# Sentinel provider-independent file bridge

This bridge removes the requirement for the DayZ process to make outbound HTTPS requests.
The dashboard host publishes its validated scheduler response to the game server over the
provider's supported file-transfer service. SentinelUI reads the local file and performs
the ten-second countdown locally.

## Server file

The destination must resolve to this DayZ profile path:

```text
$profile:SentinelUI/schedule.json
```

On G-Portal with `-profiles=config`, that is normally:

```text
config/SentinelUI/schedule.json
```

Create the `SentinelUI` directory before the first publish if the provider does not create
directories automatically.

## Publisher setup

1. Reuse the existing authenticated **Settings > Server Connection** record. Do not show a
   second credential form. Pass its provider, FTP/SFTP host, port, username, write-only
   password and remote Sentinel folder to the bridge.
2. After checking server ownership and CSRF, pass them to
   `sentinel_save_file_bridge_connection()`. It validates the values, tests the login and
   encrypts the password using PHP Sodium.
3. Keep `SENTINEL_BRIDGE_KEY` and `SENTINEL_BRIDGE_STORAGE` outside the web root. Records are
   separated by tenant ID and server ID.
4. Run `php publish_schedule.php '' TENANT_ID SERVER_ID` once to verify delivery.
5. Schedule the same command once per minute, and call it immediately whenever scheduler
   settings are saved.

Never place the credential file in Git or below a publicly served directory. Prefer FTPS.
If a provider supports only SFTP, add a provider adapter using its API or SSH2 support rather
than falling back to unencrypted FTP.

When the existing remote Sentinel folder is `/config/Sentinel`, the bridge automatically
derives `/config/SentinelUI/schedule.json`; no extra path field is needed.

The password is write-only in the dashboard: return `CredentialVault::summary()` to the UI,
which deliberately removes it. Editing a connection requires entering the password again.

## Expected DayZ log markers

```text
[SentinelUI] Purge scheduler file bridge started path=$profile:SentinelUI/schedule.json
[SentinelUI] Purge scheduler file accepted revision=...
[SentinelUI] Purge countdown 10
```

The last valid state remains active if a later transfer fails. Uploads use a temporary file
and rename so SentinelUI never reads half-written JSON.
