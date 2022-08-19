//#include <sgx_tcrypto.h>
// #include "sgxaes.h"

#define restrict __restrict__

extern "C" {
#include "AArch64cryptolib.h"
}
#ifndef CRYPTO_H
#define CRYPTO_H

#define SGX_AESGCM_IV_SIZE 12
#define SGX_AESGCM_KEY_SIZE 16
#define SGX_AESGCM_MAC_SIZE 16

typedef uint8_t sgx_aes_gcm_128bit_key_t[SGX_AESGCM_KEY_SIZE];
typedef uint8_t sgx_aes_gcm_128bit_tag_t[SGX_AESGCM_MAC_SIZE];
typedef uint8_t sgx_aes_gcm_128bit_iv_t[SGX_AESGCM_IV_SIZE];

// encrypt() and decrypt() should be called from enclave code only

// encrypt using a global key
// TODO: fix this; should use key obtained from client
void encrypt(uint8_t *plaintext, uint32_t plaintext_length, uint8_t *ciphertext,
             sgx_aes_gcm_128bit_iv_t *iv_ptr,
             sgx_aes_gcm_128bit_tag_t *mac_ptr);

void decrypt(const uint8_t *ciphertext, uint32_t ciphertext_length,
             uint8_t *plaintext, sgx_aes_gcm_128bit_iv_t *iv_ptr,
             sgx_aes_gcm_128bit_tag_t *mac_ptr, uint8_t *blind);

struct Tag {
    unsigned char t[16];
};

class MAC {
  private:
    armv8_cipher_state_t cs;

  public:
    MAC();
    Tag mac(const uint8_t *aad_ptr, uint32_t aad_len);
};

#endif
