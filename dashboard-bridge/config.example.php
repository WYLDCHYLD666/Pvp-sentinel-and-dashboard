<?php

return [
    // The existing public scheduler endpoint. Keep the trailing endpoint path here.
    'schedule_url' => 'https://sentineladmin.co.uk/dashboard/api/schedule.php',

    // Use FTPS when the provider supports it. Plain FTP is available only as a
    // compatibility fallback for hosts that do not offer encrypted file transfer.
    'transport' => 'ftps',
    'host' => 'example.hosting-provider.net',
    'port' => 21,
    'username' => 'server-file-user',
    'password' => 'replace-me',

    // This is relative to the FTP account root, not the Windows DayZ path.
    'remote_file' => '/config/SentinelUI/schedule.json',
    'timeout_seconds' => 20,
];
