#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <stdio.h>
#include <stdlib.h>

char ascii_of_byte(unsigned char b) {
    if (b < 10)
        return '0' + b;
    return 'a' + (b - 10);
}

char *sign_body(char *body, size_t body_len, char *secret, size_t secret_len,
                unsigned int *res_len) {

    unsigned char *result = calloc(EVP_MAX_MD_SIZE, sizeof(unsigned char));

    HMAC(EVP_sha1(), secret, secret_len, (unsigned char *)body, body_len,
         result, res_len);

    // 2 chars / byte, + 1 NULL byte
    char *hexa = calloc(*res_len * 2 + 1, sizeof(char));
    int pos = 0;
    for (unsigned int i = 0; i < *res_len * 2; pos++) {
        unsigned char curr = result[pos];
        hexa[i++] = ascii_of_byte((curr & (0xF0)) >> 4);
        hexa[i++] = ascii_of_byte(curr & (0x0F));
    }

    free(result);
    return hexa;
}
