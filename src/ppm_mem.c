/*
 * ppm_mem.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
out_of_memory(const char *type, size_t size)
{
    fprintf(stderr, "(%s) failed to allocate %d bytes, exiting...\n", type, size);
    exit(EXIT_FAILURE);
}

void *
ppmM_alloc(size_t size)
{
    void *p = malloc(size);

    if (!p)
        out_of_memory("malloc", size);

    return p;
}

void *
ppmM_realloc(void *ptr, size_t size)
{
    void *p;

    if (!ptr)
        return ppmM_alloc(size);

    p = realloc(ptr, size);
    if (!p)
    {
        free(ptr);
        out_of_memory("realloc", size);
    }

    return p;
}

char *
ppmM_strdup(const char *string)
{
    char *copy;
    size_t len = strlen(string);

    copy = ppmM_alloc(len + 1);
    strncpy(copy, string, len);
    copy[len] = '\0';

    return copy;
}
