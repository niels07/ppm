/*
 * ppm_string.h
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

#ifndef PPM_STRING_H
#define PPM_STRING_H

#include <stdio.h>

typedef struct
{
    char *cstr;
    size_t len;
    size_t size;
} 
ppm_String;

extern void ppmS_init(ppm_String * /* string */, const char * /* str */);
extern ppm_String *ppmS_new(const char * /* str */);
extern void ppmS_addch(ppm_String * /* string */, char /* c */);
extern void ppmS_set(ppm_String * /* string */, const char * /* str */);
extern void ppmS_append(ppm_String * /* string */, const char * /* str */);

#endif /* PPM_STRING_H */
