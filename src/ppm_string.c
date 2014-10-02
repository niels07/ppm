/*
 * ppm_string.c
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
#include <stdlib.h>
#include "ppm_string.h"
#include "ppm_mem.h"

#define STRING_GROW 32

void
ppmS_init(ppm_String *string, const char *str)
{
    if (!str)
    {
        string->len = 0;
        string->size = STRING_GROW;
        string->cstr = ppmM_alloc(string->size);
        string->cstr[0] = '\0';
    }
    else
    {
        string->len =  strlen(str);
        string->size = string->len + 1 + STRING_GROW;
        string->cstr = ppmM_alloc(string->size);
        strncpy(string->cstr, str, string->len);
        string->cstr[string->len] = '\0';
    }
}

ppm_String *
ppmS_new(const char *str)
{
    ppm_String *string = NEW(ppm_String);

    ppmS_init(string, str);
    return string;
}

void
ppmS_addch(ppm_String *string, char c)
{
    if (string->len + 1 == string->size)
    {
        string->size += STRING_GROW;
        string->cstr = ppmM_realloc(string->cstr, string->size);
    }

    string->cstr[string->len++] = c;
    string->cstr[string->len] = '\0';
}

void
ppmS_set(ppm_String *string, const char *str)
{
    free(string->cstr);
    ppmS_init(string, str);
}

void
ppmS_append(ppm_String *string, const char *str)
{
    while (*str)
    {
        if (string->len + 1 == string->size)
        {
            string->size += STRING_GROW;
            string->cstr = ppmM_realloc(string->cstr, string->size);
        }
        string->cstr[string->len++] = *str++;
    }
    string->cstr[string->len] = '\0';
}

