/*
 * ppm_aes.c
 *
 * Copyright (C) 2014 Niels Vanden Eynde
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#include "ppm_aes.h"
#include "ppm.h"
#include "ppm_mem.h"

static const unsigned int salt[] = { 1234, 5432 };
static EVP_CIPHER_CTX e_ctx, d_ctx;

unsigned int
ppmA_initcipher(const char *key)
{
    const EVP_CIPHER *cipher;
    const EVP_MD *md;
    unsigned int bytes;
    size_t len, count = 5;
    unsigned char k[32], iv[32];

    cipher = EVP_aes_256_cbc();
    md = EVP_sha1();
    len = strlen(key);
    bytes = EVP_BytesToKey(cipher, md, NULL, (unsigned char *)key, len, count, k, iv);

    if (bytes != 32) 
    {
        ppm_error("Key size is %d bits - should be 256 bits", bytes);
        return 0;
    }
  
    EVP_CIPHER_CTX_init(&e_ctx);
    EVP_EncryptInit_ex(&e_ctx, EVP_aes_256_cbc(), NULL, k, iv);
    EVP_CIPHER_CTX_init(&d_ctx);
    EVP_DecryptInit_ex(&d_ctx, EVP_aes_256_cbc(), NULL, k, iv);

    return 1;
}

char *
ppmA_encrypt(const char *data, size_t *len) 
{
    int slen = strlen(data) + 1;
    int clen = slen + AES_BLOCK_SIZE;
    int flen = 0;
    unsigned char *text = ppmM_alloc(clen);

    EVP_EncryptInit_ex(&e_ctx, NULL, NULL, NULL, NULL);
    EVP_EncryptUpdate(&e_ctx, text, &clen, (unsigned char *)data, slen);
    EVP_EncryptFinal_ex(&e_ctx, text + clen, &flen);

    *len = (size_t)(flen + slen);
    return (char *)text;
}

char *
ppmA_decrypt(const char *data, size_t len)
{
    int plen = (int)len;
    int flen = 0;
    unsigned char *text = ppmM_alloc(len);
  
    EVP_DecryptInit_ex(&d_ctx, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(&d_ctx, text, &plen, (unsigned char *)data, (int)len);
    EVP_DecryptFinal_ex(&d_ctx, text + plen, &flen);

    return (char *)text;
}

void
ppmA_cleanup(void)
{
    EVP_CIPHER_CTX_cleanup(&d_ctx);
    EVP_CIPHER_CTX_cleanup(&e_ctx);
}
