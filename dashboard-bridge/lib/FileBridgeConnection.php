<?php

declare(strict_types=1);

final class SentinelFileBridgeConnection
{
    public static function validate(array $input): array
    {
        $transport = strtolower(trim((string)($input['transport'] ?? 'ftps')));

        if (!in_array($transport, ['sftp', 'ftps', 'ftp'], true)) {
            throw new InvalidArgumentException('Transport must be SFTP, FTPS or FTP');
        }

        $host = trim((string)($input['host'] ?? ''));
        $username = trim((string)($input['username'] ?? ''));
        $password = (string)($input['password'] ?? '');
        $remoteFile = trim((string)($input['remote_file'] ?? ''));
        $remoteSentinelFolder = trim((string)($input['remote_sentinel_folder'] ?? ''));
        $port = (int)($input['port'] ?? 21);

        if ($host === '' || preg_match('/[\/:?#\[\]@]/', $host)) {
            throw new InvalidArgumentException('Enter the FTP hostname without a protocol or path');
        }

        if ($username === '' || $password === '') {
            throw new InvalidArgumentException('FTP username and password are required');
        }

        if ($port < 1 || $port > 65535) {
            throw new InvalidArgumentException('FTP port is invalid');
        }

        if ($remoteFile === '' && $remoteSentinelFolder !== '') {
            $remoteSentinelFolder = rtrim(str_replace('\\', '/', $remoteSentinelFolder), '/');
            $remoteFile = dirname($remoteSentinelFolder) . '/SentinelUI/schedule.json';
        }

        $requiredSuffix = '/sentinelui/schedule.json';
        $normalisedRemoteFile = strtolower(str_replace('\\', '/', $remoteFile));

        if ($remoteFile === '' || strpos($remoteFile, '..') !== false || substr($normalisedRemoteFile, -strlen($requiredSuffix)) !== $requiredSuffix) {
            throw new InvalidArgumentException('Remote file must end with /SentinelUI/schedule.json');
        }

        return [
            'transport' => $transport,
            'host' => $host,
            'port' => $port,
            'username' => $username,
            'password' => $password,
            'remote_file' => $remoteFile,
            'timeout_seconds' => min(60, max(5, (int)($input['timeout_seconds'] ?? 20))),
        ];
    }

    public static function test(array $connection): void
    {
        self::assertPublicHost($connection['host']);

        if ($connection['transport'] === 'sftp') {
            self::testSftp($connection);
            return;
        }

        $timeout = (int)$connection['timeout_seconds'];
        $ftp = $connection['transport'] === 'ftps'
            ? @ftp_ssl_connect($connection['host'], $connection['port'], $timeout)
            : @ftp_connect($connection['host'], $connection['port'], $timeout);

        if ($ftp === false || !@ftp_login($ftp, $connection['username'], $connection['password'])) {
            throw new RuntimeException('Could not connect or authenticate using those details');
        }

        ftp_pasv($ftp, true);
        $directory = str_replace('\\', '/', dirname($connection['remote_file']));

        if (!@ftp_chdir($ftp, $directory)) {
            ftp_close($ftp);
            throw new RuntimeException('Connected, but the SentinelUI destination directory was not found');
        }

        $probeName = '.sentinel-write-test-' . bin2hex(random_bytes(6));
        $probe = fopen('php://temp', 'w+b');
        fwrite($probe, "Sentinel file bridge connection test\n");
        rewind($probe);

        if (!@ftp_fput($ftp, $probeName, $probe, FTP_BINARY)) {
            fclose($probe);
            ftp_close($ftp);
            throw new RuntimeException('Connected, but the destination directory is not writable');
        }

        fclose($probe);
        @ftp_delete($ftp, $probeName);

        ftp_close($ftp);
    }

    private static function testSftp(array $connection): void
    {
        if (!extension_loaded('ssh2')) {
            throw new RuntimeException('The dashboard host needs the PHP SSH2 extension for SFTP');
        }

        $session = @ssh2_connect($connection['host'], $connection['port']);

        if ($session === false || !@ssh2_auth_password($session, $connection['username'], $connection['password'])) {
            throw new RuntimeException('Could not connect or authenticate using those SFTP details');
        }

        $sftp = @ssh2_sftp($session);

        if ($sftp === false) {
            throw new RuntimeException('Could not initialise the SFTP connection');
        }

        $directory = str_replace('\\', '/', dirname($connection['remote_file']));
        $probeName = $directory . '/.sentinel-write-test-' . bin2hex(random_bytes(6));
        $probeUri = 'ssh2.sftp://' . intval($sftp) . $probeName;

        if (@file_put_contents($probeUri, "Sentinel file bridge connection test\n") === false) {
            throw new RuntimeException('Connected, but the SentinelUI destination directory is not writable');
        }

        @unlink($probeUri);
    }

    private static function assertPublicHost(string $host): void
    {
        $addresses = gethostbynamel($host);

        if ($addresses === false || count($addresses) === 0) {
            throw new RuntimeException('FTP hostname could not be resolved');
        }

        foreach ($addresses as $address) {
            if (filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4 | FILTER_FLAG_NO_PRIV_RANGE | FILTER_FLAG_NO_RES_RANGE) === false) {
                throw new RuntimeException('FTP hostname must resolve only to public addresses');
            }
        }
    }
}
