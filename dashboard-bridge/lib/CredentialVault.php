<?php

declare(strict_types=1);

final class SentinelCredentialVault
{
    private string $directory;
    private string $key;

    public function __construct(string $directory, string $base64Key)
    {
        if (!extension_loaded('sodium')) {
            throw new RuntimeException('PHP sodium extension is required');
        }

        $key = base64_decode($base64Key, true);

        if ($key === false || strlen($key) !== SODIUM_CRYPTO_SECRETBOX_KEYBYTES) {
            throw new RuntimeException('SENTINEL_BRIDGE_KEY must be a base64-encoded 32-byte key');
        }

        $this->directory = rtrim($directory, '/\\');
        $this->key = $key;
    }

    public function save(string $tenantId, string $serverId, array $connection): void
    {
        $path = $this->recordPath($tenantId, $serverId);
        $directory = dirname($path);

        if (!is_dir($directory) && !mkdir($directory, 0700, true) && !is_dir($directory)) {
            throw new RuntimeException('Could not create protected credential directory');
        }

        $plain = json_encode($connection, JSON_THROW_ON_ERROR | JSON_UNESCAPED_SLASHES);
        $nonce = random_bytes(SODIUM_CRYPTO_SECRETBOX_NONCEBYTES);
        $record = [
            'version' => 1,
            'cipher' => 'sodium_secretbox',
            'nonce' => base64_encode($nonce),
            'data' => base64_encode(sodium_crypto_secretbox($plain, $nonce, $this->key)),
            'updated_at' => gmdate('c'),
        ];
        $temporary = $path . '.' . bin2hex(random_bytes(6)) . '.tmp';

        if (file_put_contents($temporary, json_encode($record, JSON_PRETTY_PRINT | JSON_THROW_ON_ERROR), LOCK_EX) === false) {
            throw new RuntimeException('Could not write encrypted credential record');
        }

        @chmod($temporary, 0600);

        if (!rename($temporary, $path)) {
            @unlink($temporary);
            throw new RuntimeException('Could not activate encrypted credential record');
        }
    }

    public function load(string $tenantId, string $serverId): array
    {
        $path = $this->recordPath($tenantId, $serverId);
        $record = json_decode((string)file_get_contents($path), true, 512, JSON_THROW_ON_ERROR);
        $nonce = base64_decode((string)($record['nonce'] ?? ''), true);
        $cipher = base64_decode((string)($record['data'] ?? ''), true);

        if ($nonce === false || $cipher === false) {
            throw new RuntimeException('Credential record is malformed');
        }

        $plain = sodium_crypto_secretbox_open($cipher, $nonce, $this->key);

        if ($plain === false) {
            throw new RuntimeException('Credential record could not be decrypted');
        }

        return json_decode($plain, true, 512, JSON_THROW_ON_ERROR);
    }

    public function summary(string $tenantId, string $serverId): ?array
    {
        $path = $this->recordPath($tenantId, $serverId);

        if (!is_file($path)) {
            return null;
        }

        $connection = $this->load($tenantId, $serverId);
        unset($connection['password']);
        $connection['configured'] = true;
        return $connection;
    }

    private function recordPath(string $tenantId, string $serverId): string
    {
        $tenant = $this->safeId($tenantId);
        $server = $this->safeId($serverId);
        return $this->directory . DIRECTORY_SEPARATOR . $tenant . DIRECTORY_SEPARATOR . $server . '.json.enc';
    }

    private function safeId(string $value): string
    {
        if ($value === '' || !preg_match('/^[A-Za-z0-9_-]{1,80}$/', $value)) {
            throw new InvalidArgumentException('Invalid tenant or server ID');
        }

        return $value;
    }
}
