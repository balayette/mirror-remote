/*
MIT License

Copyright (c) 2018 Nicolas Manichon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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

    // Don't really need to use EVP_MAX_MD_SIZE, because SHA1 HMAC is always
    // 20 bytes, but you never know...
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
