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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "ppm.h"
#include "ppm_db.h"
#include "ppm_command.h"
#include "ppm_aes.h"

unsigned int ppm_autosave = 0;
unsigned int ppm_usecolor = 1;

char *ppm_cipherkey = NULL;
char *ppm_dbfile = NULL;
char *program_name = NULL;

#define COLOR(c) "\033[" #c "m"
char *ppm_colors[] = { "", "", "", "", "", ""};

void 
ppm_error(const char *format, ...)
{
    va_list vl;
    
    va_start(vl, format);
    fprintf(stderr, "%s%s:%s ", 
            PPMC(BLUE),
            program_name,
            PPMC(RED));
    vfprintf(stderr, format, vl);
    if (errno != 0)
    {
        fprintf(stderr, ": %s", strerror(errno));
        errno = 0;
    }
    fprintf(stderr, "%s\n", PPMC(NONE));
}

void 
ppm_message(const char *format, ...)
{
    va_list vl;
    
    va_start(vl, format);
    fprintf(stdout, "%s%s:%s ", 
            PPMC(BLUE), 
            program_name,
            PPMC(GREEN));
    vfprintf(stdout, format, vl);
    fprintf(stdout, "%s\n", PPMC(NONE));
}


static void 
printopt(char ch, const char *str, const char *desc)
{
    printf("    %s-%c%s, %s--%s%s%s\n",
           PPMC(GREEN), ch, PPMC(BLUE), PPMC(GREEN), str, PPMC(BLUE), desc);

}

void
ppm_usage(void)
{
    printf("%sUsage%s: %s %s[%sOPTIONS%s]%s... %s[%sCOMMAND%s] %s[%sARGS%s]\n\n", 
           PPMC(RED),   PPMC(BLUE),  program_name,
           PPMC(WHITE), PPMC(GREEN), PPMC(WHITE),
           PPMC(BLUE), 
           PPMC(WHITE), PPMC(GREEN), PPMC(WHITE),
           PPMC(WHITE), PPMC(GREEN), PPMC(WHITE));

    printopt('k', "key", "       provide cipher key used for encryption en decryption");
    printopt('f', "file", "      specifiy file for storing passwords (default: $HOME/.ppm)");
    printopt('s', "save", "      automatically save when running interactively");
    printopt('c', "no-color", "  don't use colors");
    fprintf(stdout, "%s\n", PPMC(NONE));

}

static void
initcolors(void)
{
    if (!ppm_usecolor) return;
    PPMC(NONE)  = COLOR(0);
    PPMC(RED)   = COLOR(31);
    PPMC(GREEN) = COLOR(32);
    PPMC(BLUE)  = COLOR(34);
    PPMC(CYAN)  = COLOR(36);
    PPMC(WHITE) = COLOR(37);
}

unsigned int 
ppm_init(void)
{
    initcolors();
    if (!ppm_cipherkey)
        return 1;

    if (!ppmA_initcipher(ppm_cipherkey))
        return 0;
    
    if (!ppmD_init()) return 0;
    ppmC_init();
    return 1;
}

void
ppm_cleanup(void)
{
    ppmA_cleanup();
    ppmD_cleanup();
    if (ppm_cipherkey) free(ppm_cipherkey);
}
