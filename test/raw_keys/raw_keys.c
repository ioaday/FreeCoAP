/*
 * Copyright (c) 2016 Keith Cullen.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 *  @file raw_keys.c
 *
 *  @brief Source file for the FreeCoAP raw keys module
 */

#include <stdio.h>
#include "raw_keys.h"
#include "coap_log.h"

#define RAW_KEYS_ECDSA_KEY_LEN  32

static unsigned char raw_keys_ecdsa_priv_key[RAW_KEYS_ECDSA_KEY_LEN] = {0};
static unsigned char raw_keys_ecdsa_pub_key_x[RAW_KEYS_ECDSA_KEY_LEN] = {0};
static unsigned char raw_keys_ecdsa_pub_key_y[RAW_KEYS_ECDSA_KEY_LEN] = {0};

unsigned char *raw_keys_get_ecdsa_priv_key(void)
{
    return raw_keys_ecdsa_priv_key;
}

unsigned char *raw_keys_get_ecdsa_pub_key_x(void)
{
    return raw_keys_ecdsa_pub_key_x;
}

unsigned char *raw_keys_get_ecdsa_pub_key_y(void)
{
    return raw_keys_ecdsa_pub_key_y;
}

static int raw_keys_read_file(FILE *file, unsigned char *buf)
{
    unsigned val = 0;
    char txt[3] = {0};
    int num = 0;
    int c = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < RAW_KEYS_ECDSA_KEY_LEN; i++)
    {
        for (j = 0; j < 2; j++)
        {
            c = fgetc(file);
            if (c == EOF)
            {
                return -1;
            }
            txt[j] = (char)c;
        }
        num = sscanf(txt, "%02x", &val);
        if (num != 1)
        {
            return -1;
        }
        buf[i] = (unsigned char)val;
    }
    return 0;
}

int raw_keys_load(const char *priv_key_file_name, const char *pub_key_file_name)
{
    FILE *file = NULL;
    int ret = 0;
    int j = 0;
    int c = 0;

    file = fopen(priv_key_file_name, "r");
    if (file == NULL)
    {
        coap_log_error("failed to open file '%s'", priv_key_file_name);
        return -1;
    }
    ret = raw_keys_read_file(file, raw_keys_ecdsa_priv_key);
    fclose(file);
    if (ret < 0)
    {
        coap_log_error("failed to read file '%s'", priv_key_file_name);
        return -1;
    }
    file = fopen(pub_key_file_name, "r");
    if (file == NULL)
    {
        coap_log_error("failed to open file '%s'", pub_key_file_name);
        return -1;
    }
    /* skip the first byte */
    for (j = 0; j < 2; j++)
    {
        c = fgetc(file);
        if (c == EOF)
        {
            coap_log_error("failed to read file '%s'", pub_key_file_name);
            fclose(file);
            return -1;
        }
    }
    ret = raw_keys_read_file(file, raw_keys_ecdsa_pub_key_x);
    if (ret < 0)
    {
        coap_log_error("failed to read file '%s'", pub_key_file_name);
        fclose(file);
        return -1;
    }
    ret = raw_keys_read_file(file, raw_keys_ecdsa_pub_key_y);
    fclose(file);
    if (ret < 0)
    {
        coap_log_error("failed to read file '%s'", pub_key_file_name);
        return -1;
    }
    return 0;
}