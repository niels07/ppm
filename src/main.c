/*
 * main.c
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "ppm.h"
#include "ppm_db.h"
#include "ppm_command.h"
#include "ppm_mem.h"

static unsigned int
interactive(void)
{
    char *line;

    if (!ppm_cipherkey)
    {
        /* ppm_cipherkey was not set using '--key' so we'll
           prompt for it before running ppm. */
        line = ppmC_getline("cipher key: ");
        if (!line)
            return EXIT_FAILURE;
        ppm_cipherkey = ppmM_strdup(line);
    }

    if (!ppm_init())
        return EXIT_FAILURE;
    
    while ((line = ppmC_readline()))
        ppmC_eval(line);

    return 0;
}

static char **
parse_argv(int *argc, char **argv)
{
    char **args;
    unsigned int i;

    i = 0;
    (*argc)--;
    argv++;
    args = ppmM_alloc(*argc * sizeof(char *));
    for (; *argv; argv++)
    {
        char *arg;
        char c;

        arg = *argv;
        if (*arg != '-' || arg[1] == '\0')
        {
            args[i++] = arg;
            continue;
        }

        if (arg[1] == '-')
        {
            arg += 2;
            if (strcmp(arg, "save") == 0)
            {
                ppm_autosave = 1;
                continue;
            }
            
            if (strcmp(arg, "no-color") == 0)
            {
                ppm_usecolor = 0;
                continue;
            }
     
            if (!*++argv)
            {
                ppm_error("no argument provided for '-%s'", arg);
                free(args);
                return NULL;
            }

            if (strcmp(arg, "key") == 0)
                ppm_cipherkey = ppmM_strdup(*argv);

            else
            if (strcmp(arg, "file") == 0)
                ppm_dbfile = *argv;

            continue;
        }
        c = arg[1];
        
        if (!strchr("sc", c) && !*++argv)
        {
            ppm_error("no argument provided for '-%c'", c);
            free(args);
            return NULL;
        }

        switch (c)
        {
        case 'k':
            ppm_cipherkey = ppmM_strdup(*argv);
            break;

        case 'f':
            ppm_dbfile = *argv;
            break;

        case 's':
            ppm_autosave = 1;
            break;

        case 'c':
            ppm_usecolor = 0;
            break;

        default:
            ppm_error("unrecognized option '-%c'");
            return NULL;
        }

    }
    args[i] = NULL;
    *argc = i - 1;
    return args;
}

int 
main(int argc, char *argv[])
{
    int ret;
    char **args;

    program_name = argv[0];
    args = parse_argv(&argc, argv);

    /* ARGS and ARGC's value refer to the number of arguments passed
       to a command for ppm i.e: "./ppm -k key add user pass" counts 
       the number of arguments passed to 'add'. This means we have '2' 
       arguments. If argc is less than 0, no command was request so
       interactive mode is expected. */
    if (argc < 0)
        return interactive();

    if (!args) return EXIT_FAILURE;
    if (!ppm_init())
    {
        free(args);
        return EXIT_FAILURE;
    }
    ppm_autosave = 1;
    if (!ppmC_command(argc, args))
        ret = EXIT_FAILURE;

    ppmD_save();
    ppm_cleanup();
    free(args);
    return ret;
}
