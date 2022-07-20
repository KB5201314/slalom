#include "Crypto.h"

#ifdef USE_SGX
#include "Enclave.h"
#include <sgx_trts.h>
#else

#include <stdio.h>
void sgx_read_rand(unsigned char *buf, size_t size) {}
#endif

#include <cstring>

sgx_aes_gcm_128bit_key_t key_data = {0};


bool key_initialized = false;
armv8_cipher_constants_t cc;

void init_key() {
    if (!key_initialized) {
        // We use aes-gcm-128 mode
        armv8_cipher_mode_t mode = AES_GCM_128;
        uint8_t tag_byte_length = SGX_AESGCM_MAC_SIZE;
        armv8_aes_gcm_set_constants(mode, tag_byte_length, (uint8_t *)key_data,
                                    &cc);
        key_initialized = true;
    }
}

// encrypt() and decrypt() should be called from enclave code only
// TODO: encrypt() and decrypt() should return status

// encrypt using a global key
// TODO: fix this; should use key obtained from client
void encrypt(uint8_t *plaintext, uint32_t plaintext_length, uint8_t *ciphertext,
             sgx_aes_gcm_128bit_iv_t *iv_ptr,
             sgx_aes_gcm_128bit_tag_t *mac_ptr) {

    init_key();
    // key size is 12 bytes/128 bits
    // IV size is 12 bytes/96 bits
    // MAC size is 16 bytes/128 bits

    // generate random IV
    sgx_read_rand((uint8_t *)iv_ptr, SGX_AESGCM_IV_SIZE);

    armv8_cipher_state_t cs;
    cs.constants = &cc;
    armv8_operation_result_t result =
        armv8_aes_gcm_set_counter((uint8_t *)iv_ptr, SGX_AESGCM_IV_SIZE, &cs);
    if (result != SUCCESSFUL_OPERATION) {
        printf("Failure while setting nonce!\n");
        abort();
    }
    result =
        armv8_enc_aes_gcm_from_state(&cs, NULL, 0, plaintext, plaintext_length,
                                     ciphertext, (uint8_t *)mac_ptr);
    if (result != SUCCESSFUL_OPERATION) {
        printf("Failure while encrypting!\n");
        abort();
    }
}

void decrypt(const uint8_t *ciphertext, uint32_t ciphertext_length,
             uint8_t *plaintext, sgx_aes_gcm_128bit_iv_t *iv_ptr,
             sgx_aes_gcm_128bit_tag_t *mac_ptr, uint8_t *blind) {

    init_key();

    // decrypt using a global key
    // TODO: fix this; should use key obtained from client

    // key size is 12 bytes/128 bits
    // IV size is 12 bytes/96 bits
    // MAC size is 16 bytes/128 bits

    armv8_cipher_state_t cs;
    cs.constants = &cc;
    armv8_operation_result_t result =
        armv8_aes_gcm_set_counter((uint8_t *)iv_ptr, SGX_AESGCM_IV_SIZE, &cs);
    if (result != SUCCESSFUL_OPERATION) {
        printf("Failure while setting nonce!\n");
        abort();
    }
    // imlk: just ignore `blind`, since it was not actually used by
    // intel_aes_gcmDEC in sgxaex_asm.S
    result = armv8_dec_aes_gcm_from_state(&cs, NULL, 0, (uint8_t *)ciphertext,
                                          ciphertext_length, (uint8_t *)mac_ptr,
                                          plaintext);
    if (result == AUTHENTICATION_FAILURE) {
        printf("Decrypt: invalid mac\n");
    } else if (result != SUCCESSFUL_OPERATION) {
        printf("Failure while decrypting!\n");
        abort();
    }
}

MAC::MAC() {
    uint8_t iv[SGX_AESGCM_IV_SIZE + 1] = "000000000000";
    size_t iv_len = 12; // SGX_AESGCM_IV_SIZE
    cs.constants = &cc;
    armv8_operation_result_t result =
        armv8_aes_gcm_set_counter((uint8_t *)iv, iv_len, &this->cs);
    if (result != SUCCESSFUL_OPERATION) {
        printf("Failure while setting nonce!\n");
        abort();
    }
}

Tag MAC::mac(const uint8_t *aad_ptr, uint32_t aad_len) {
    Tag ret = Tag();
    armv8_enc_aes_gcm_from_state(&this->cs, (uint8_t *)aad_ptr, aad_len, NULL,
                                 0, NULL, (uint8_t *)ret.t);
    return ret;
}
