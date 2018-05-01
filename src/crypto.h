#include <stdlib.h>

#ifndef CRYPTO_H
#define CRYPTO_H

char *sign_body(char *body, size_t body_len, char *secret,
                         size_t secret_len, unsigned int *res_len);

#endif
