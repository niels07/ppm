/*
 * ppm.c
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

#ifndef PPM_H
#define PPM_H

enum
{
    PPM_CNONE = 0,
    PPM_CRED,
    PPM_CGREEN,
    PPM_CBLUE,
    PPM_CCYAN,
    PPM_CWHITE
};

extern void ppm_error(const char * /* format */, ...);
extern void ppm_message(const char * /* format */, ...);

extern unsigned int ppm_init();
extern void ppm_cleanup(void);
extern void ppm_usage(void);

extern unsigned int ppm_autosave;
extern unsigned int ppm_usecolor;
extern char *ppm_dbfile;
extern char *ppm_cipherkey;
extern char *program_name;
extern char *ppm_colors[];

#define PPMC(c) ppm_colors[PPM_C##c]

#endif /* PPM_H */
