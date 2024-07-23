#include <stdio.h>
#include <string.h>
#include "esp_mac.h"
#include "sodium.h"
#include <memory.h>

void experiment_2()
{
    if (sodium_init() == -1)
    {
        printf("Failed to initialize libsodium\n");
        return;
    }

    unsigned char device_private_key[crypto_kx_SECRETKEYBYTES];
    unsigned char device_public_key[crypto_kx_PUBLICKEYBYTES];
    unsigned char cloud_private_key[crypto_kx_SECRETKEYBYTES];
    unsigned char cloud_public_key[crypto_kx_PUBLICKEYBYTES];

    // Generate private keys
    crypto_aead_xchacha20poly1305_ietf_keygen(device_private_key);
    crypto_aead_xchacha20poly1305_ietf_keygen(cloud_private_key);

    // Get public keys
    crypto_sign_ed25519_sk_to_pk(device_public_key, device_private_key);
    crypto_sign_ed25519_sk_to_pk(cloud_private_key, cloud_private_key);

    // Generate common key
    unsigned char shared_secret_1[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(shared_secret_1, device_private_key, cloud_public_key) != 0)
    {
        printf("Failed to generate shared secret\n");
        return;
    }

    unsigned char shared_secret_2[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(shared_secret_2, device_private_key, cloud_public_key) != 0)
    {
        printf("Failed to generate shared secret\n");
        return;
    }

    if (memcmp(shared_secret_1, shared_secret_2, crypto_scalarmult_BYTES) == 0)
        printf("Shared secrets are equal\n");
    else
        printf("Shared secrets are not equal\n");
}

void app_main(void)
{
    experiment_2();
}