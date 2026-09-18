<?php

declare(strict_types=1);

require_once __DIR__ . '/lib/CredentialVault.php';
require_once __DIR__ . '/lib/FileBridgeConnection.php';

/**
 * Call this from the authenticated dashboard server-settings controller after
 * verifying the logged-in tenant, server ownership and CSRF token.
 */
function sentinel_save_file_bridge_connection(
    string $tenantId,
    string $serverId,
    array $form,
    string $protectedStorageDirectory,
    string $base64MasterKey
): array {
    $connection = SentinelFileBridgeConnection::validate($form);
    SentinelFileBridgeConnection::test($connection);

    $vault = new SentinelCredentialVault($protectedStorageDirectory, $base64MasterKey);
    $vault->save($tenantId, $serverId, $connection);

    return $vault->summary($tenantId, $serverId) ?? [];
}
