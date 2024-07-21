#include <stdio.h>
#include <string.h>
#include "esp_mac.h"
#include "sodium.h"

void app_main(void)
{
    // Initialize the library
    if (sodium_init() < 0)
    {
        printf("Failed to initialize libsodium\n");
        return;
    }

    printf("libsodium initialized\n");

    // Generate a random key
    unsigned char key[crypto_aead_xchacha20poly1305_ietf_KEYBYTES];
    crypto_aead_xchacha20poly1305_ietf_keygen(key);

    printf("Key generated\n");

    // Encrypt a message
    const unsigned char *message = (const unsigned char *)"Hello, World!";
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
    unsigned char ciphertext[1024];
    unsigned long long ciphertext_len;

    randombytes_buf(nonce, sizeof(nonce));
    crypto_aead_xchacha20poly1305_ietf_encrypt(ciphertext, &ciphertext_len,
                                               message, strlen((const char *)message),
                                               NULL, 0, NULL, nonce, key);

    printf("Message encrypted\n");

    // Decrypt the message
    unsigned char decrypted[1024];
    unsigned long long decrypted_len;
    int ret = crypto_aead_xchacha20poly1305_ietf_decrypt(decrypted, &decrypted_len,
                                                         NULL, ciphertext, ciphertext_len,
                                                         NULL, 0, nonce, key);

    if (ret == 0)
    {
        printf("Decrypted message: %s\n", decrypted);
    }
    else
    {
        printf("Failed to decrypt message\n");
    }

    return;
}
