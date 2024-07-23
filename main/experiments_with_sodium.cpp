#include <iostream>
#include <sodium.h>

#include <memory.h>

void experiment_1(void)
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

    // Generate a public key
    unsigned char pk[crypto_box_PUBLICKEYBYTES];
    unsigned char sk[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(pk, sk);

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

void experiment_2()
{
    if (sodium_init() == -1)
    {
        std::cerr << "Failed to initialize libsodium" << std::endl;
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

    // Get shared key
    unsigned char shared_secret_1[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(shared_secret_1, device_private_key, cloud_public_key) != 0)
    {
        std::cerr << "Failed to generate shared secret" << std::endl;
        return;
    }

    unsigned char shared_secret_2[crypto_scalarmult_BYTES];
    if (crypto_scalarmult(shared_secret_2, device_private_key, cloud_public_key) != 0)
    {
        std::cerr << "Failed to generate shared secret" << std::endl;
        return;
    }

    memcmp(shared_secret_1, shared_secret_2, crypto_scalarmult_BYTES) == 0
        ? std::cout << "Shared secrets are equal" << std::endl
        : std::cout << "Shared secrets are not equal" << std::endl;

    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
    randombytes_buf(nonce, sizeof(nonce));

    const unsigned char *message = (const unsigned char *)"Hello, World!";

    unsigned char ciphertext[1024];
    unsigned long long ciphertext_len;

    crypto_aead_xchacha20poly1305_ietf_encrypt(ciphertext, &ciphertext_len,
                                               message, strlen((const char *)message),
                                               NULL, 0, NULL, nonce, key);
}

// void experiment_3()
// {
//     if (sodium_init() == -1)
//     {
//         std::cerr << "Failed to initialize libsodium" << std::endl;
//         return;
//     }

//     // Generate keys
//     unsigned char device_private_key[crypto_kx_SECRETKEYBYTES];
//     unsigned char device_public_key[crypto_kx_PUBLICKEYBYTES];
//     unsigned char cloud_public_key[crypto_kx_PUBLICKEYBYTES];

//     if (crypto_kx_keypair(device_public_key, device_private_key) != 0)
//     {
//         std::cerr << "Failed to generate key pair" << std::endl;
//         return;
//     }

//     unsigned char their_private_key[crypto_kx_SECRETKEYBYTES];
//     if (crypto_kx_keypair(cloud_public_key, their_private_key) != 0)
//     {
//         std::cerr << "Failed to generate their key pair" << std::endl;
//         return;
//     }

//     // Generate common key
//     unsigned char shared_secret[crypto_scalarmult_BYTES];
//     if (crypto_scalarmult(shared_secret, device_private_key, cloud_public_key) != 0)
//     {
//         std::cerr << "Failed to generate shared secret" << std::endl;
//         return;
//     }

//     unsigned char shared_secret[crypto_scalarmult_BYTES];
//     if (crypto_scalarmult(shared_secret, device_private_key, cloud_public_key) != 0)
//     {
//         std::cerr << "Failed to generate shared secret" << std::endl;
//         return;
//     }

//     // Receive message, nonce и AEAD tag
//     unsigned char ciphertext[] = "Encrypted message..."; // Зашифрованное сообщение
//     unsigned char nonce[crypto_aead_chacha20poly1305_IETF_NPUBBYTES];
//     unsigned char aead_tag[crypto_aead_chacha20poly1305_IETF_ABYTES];

//     randombytes_buf(nonce, sizeof nonce);
//     randombytes_buf(aead_tag, sizeof aead_tag);

//     unsigned char decrypted_message[sizeof(ciphertext)];
//     unsigned long long decrypted_message_len;

//     if (crypto_aead_chacha20poly1305_ietf_decrypt(
//             decrypted_message,
//             &decrypted_message_len,
//             NULL,
//             ciphertext,
//             sizeof(ciphertext),
//             aead_tag,
//             sizeof(aead_tag),
//             nonce,
//             rx) != 0)
//     {
//         std::cerr << "Failed to decrypt message" << std::endl;
//         return;
//     }

//     std::cout << "Decrypted message: " << decrypted_message << std::endl;
// }
