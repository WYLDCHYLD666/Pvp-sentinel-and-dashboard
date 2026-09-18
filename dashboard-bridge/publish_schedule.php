<?php

declare(strict_types=1);

if (PHP_SAPI !== 'cli') {
    http_response_code(404);
    exit;
}

$configPath = $argv[1] ?? '';
$tenantId = $argv[2] ?? '';
$serverId = $argv[3] ?? '';

if ($configPath === '' && $tenantId !== '' && $serverId !== '') {
    require_once __DIR__ . '/lib/CredentialVault.php';
    $storage = getenv('SENTINEL_BRIDGE_STORAGE') ?: dirname(__DIR__, 2) . '/sentinel-secrets/file-bridge';
    $masterKey = getenv('SENTINEL_BRIDGE_KEY') ?: '';
    $vault = new SentinelCredentialVault($storage, $masterKey);
    $config = $vault->load($tenantId, $serverId);
    $config['schedule_url'] = getenv('SENTINEL_SCHEDULE_URL') ?: 'https://sentineladmin.co.uk/dashboard/api/schedule.php';
} elseif ($configPath !== '') {
    if (!is_file($configPath)) {
        fwrite(STDERR, "Missing bridge configuration: {$configPath}\n");
        exit(2);
    }

    $config = require $configPath;
} else {
    fwrite(STDERR, "Usage: php publish_schedule.php [config.php] OR php publish_schedule.php '' TENANT_ID SERVER_ID\n");
    exit(2);
}
$required = ['schedule_url', 'transport', 'host', 'port', 'username', 'password', 'remote_file'];

foreach ($required as $key) {
    if (!isset($config[$key]) || $config[$key] === '') {
        fwrite(STDERR, "Missing configuration value: {$key}\n");
        exit(2);
    }
}

$timeout = (int)($config['timeout_seconds'] ?? 20);
$context = stream_context_create([
    'http' => [
        'timeout' => $timeout,
        'ignore_errors' => true,
        'header' => "Accept: application/json\r\nUser-Agent: Sentinel-File-Bridge/1.0\r\n",
    ],
]);
$json = @file_get_contents((string)$config['schedule_url'], false, $context);

if ($json === false) {
    fwrite(STDERR, "Could not read scheduler endpoint\n");
    exit(3);
}

$payload = json_decode($json, true);

if (!is_array($payload) || ($payload['ok'] ?? false) !== true || !isset($payload['data']['state'])) {
    fwrite(STDERR, "Scheduler endpoint returned an invalid payload\n");
    exit(3);
}

// Re-encode validated JSON so a partial/HTML response can never reach the game server.
$json = json_encode($payload, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES) . "\n";
$transport = strtolower((string)$config['transport']);

if ($transport === 'sftp') {
    if (!extension_loaded('ssh2')) {
        fwrite(STDERR, "PHP SSH2 extension is required for SFTP\n");
        exit(4);
    }

    $session = @ssh2_connect((string)$config['host'], (int)$config['port']);

    if ($session === false || !@ssh2_auth_password($session, (string)$config['username'], (string)$config['password'])) {
        fwrite(STDERR, "Could not connect or authenticate to game-server SFTP storage\n");
        exit(4);
    }

    $sftp = @ssh2_sftp($session);
    $remoteFile = (string)$config['remote_file'];
    $remoteTemp = $remoteFile . '.uploading';
    $remoteTempUri = 'ssh2.sftp://' . intval($sftp) . $remoteTemp;

    if (@file_put_contents($remoteTempUri, $json, LOCK_EX) === false || !@ssh2_sftp_rename($sftp, $remoteTemp, $remoteFile)) {
        @ssh2_sftp_unlink($sftp, $remoteTemp);
        fwrite(STDERR, "SFTP upload or activation failed\n");
        exit(5);
    }

    fwrite(STDOUT, "Published Sentinel schedule to {$remoteFile}\n");
    exit(0);
}

if ($transport === 'ftps') {
    $ftp = @ftp_ssl_connect((string)$config['host'], (int)$config['port'], $timeout);
} elseif ($transport === 'ftp') {
    $ftp = @ftp_connect((string)$config['host'], (int)$config['port'], $timeout);
} else {
    fwrite(STDERR, "Unsupported transport. Use sftp, ftps or ftp.\n");
    exit(2);
}

if ($ftp === false || !@ftp_login($ftp, (string)$config['username'], (string)$config['password'])) {
    fwrite(STDERR, "Could not connect or authenticate to game-server file storage\n");
    exit(4);
}

ftp_pasv($ftp, true);
$remoteFile = (string)$config['remote_file'];
$remoteTemp = $remoteFile . '.uploading';
$stream = fopen('php://temp', 'w+b');
fwrite($stream, $json);
rewind($stream);

if (!@ftp_fput($ftp, $remoteTemp, $stream, FTP_BINARY)) {
    fclose($stream);
    ftp_close($ftp);
    fwrite(STDERR, "Upload failed\n");
    exit(5);
}

fclose($stream);

// Atomic replacement prevents DayZ from reading a partially uploaded JSON file.
if (!@ftp_rename($ftp, $remoteTemp, $remoteFile)) {
    @ftp_delete($ftp, $remoteFile);

    if (!@ftp_rename($ftp, $remoteTemp, $remoteFile)) {
        ftp_close($ftp);
        fwrite(STDERR, "Could not activate uploaded schedule file\n");
        exit(5);
    }
}

ftp_close($ftp);
fwrite(STDOUT, "Published Sentinel schedule to {$remoteFile}\n");
