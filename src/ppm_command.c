/*
 * ppm_command.c
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

#include "config.h"

#ifdef HAVE_LIBREADLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#endif

#include "ppm_command.h"
#include "ppm_db.h"
#include "ppm_mem.h"
#include "ppm.h"

#define PROMPT "ppm > "

typedef unsigned int Function(size_t, char **);
static char *line = NULL;

static unsigned int
add(size_t argc, char **args)
{
    char *app, *pass;
    
    app = args[0];
    pass = args[1];
    ppmD_add(app, pass);
    if (ppm_autosave) ppmD_save();
    return 1;
}

static unsigned int
update(size_t argc, char **args)
{
    char *app, *pass;

    app = args[0];
    pass = args[1];
    ppmD_update(app, pass);
    if (ppm_autosave) ppmD_save();
    return 1;
}

static unsigned int
get(size_t argc, char **args)
{
    char *app, *pass;
    
    app = args[0];
    pass = ppmD_get(app);
    if (pass) puts(pass);
    return 1;
}

static unsigned int
list(size_t argc, char **args)
{
    ppmD_list();
    return 1;
}

static unsigned int
bye(size_t argc, char **args)
{
    ppm_cleanup();
    exit(EXIT_SUCCESS);
}

static unsigned int
save(size_t argc, char **args)
{
    if (ppmD_save())
    {
        printf("saved!\n");
        return 1;
    }
    return 0;
}

static unsigned int
rm(size_t argc, char **args)
{
    char *app;
    
    app = args[0];
    ppmD_rm(app);
    if (ppm_autosave) ppmD_save();
    return 1;
}

typedef struct
{
    char *name;
    Function *f;
    int argc;
    char *descr;
    char *usage;
} 
Command;

static unsigned int help(size_t argc, char **args);

static Command commands[] = 
{
    { "add", add, 2, "add a new password", "add <user> <password>" },
    { "update", update, 2, "update a user", "update <user> <password>" },
    { "list", list, 0, "list all passwords", "list" },
    { "get", get, 1, "get a password for a specific user", "get <user>" },
    { "rm", rm, 1, "remove a user from the database", "rm <user>" },
    { "bye", bye, 0, "exit this program", "bye" },
    { "help", help, -1, "display a list of possible commands", "help [command]" },
    { "save",  save, -1, "save the list of passwords", "save" },
    { NULL, NULL,  0, NULL, NULL }
};

static unsigned int 
help(size_t argc, char **args)
{
    unsigned int i, j;
    
    if (argc == 0)
    {
        ppm_usage();
        fprintf(stdout, "%sThe following commands are available: \n", PPMC(BLUE));
    }
    for (i = 0; commands[i].name; i++)
    {
        Command *cmd = commands + i;
        if (argc == 0)
        {
            printf("%s* %s%s%s: %s%s%s\n", 
                    PPMC(RED), PPMC(CYAN),  cmd->name, 
                    PPMC(BLUE), PPMC(CYAN), cmd->descr, 
                    PPMC(NONE));
            continue;
        }
        for (j = 0; j < argc; j++)
        {
            if (strcmp(cmd->name, args[j]) != 0)
                continue;

            printf("%s* %s%s%s: %s%s%s\n", 
                    PPMC(RED), PPMC(CYAN),  cmd->name, 
                    PPMC(BLUE), PPMC(CYAN), cmd->descr, 
                    PPMC(NONE));

            fprintf(stdout, "  %susage%s: %s%s%s\n",
                    PPMC(RED), PPMC(BLUE), PPMC(CYAN), 
                    cmd->usage, PPMC(NONE));
        }
    }
    return 1;
}

static Command *
find_command(const char *name)
{
    unsigned int i;

    for (i = 0; commands[i].name; i++)
    {
        Command *cmd = commands + i;

        if (strcmp(cmd->name, name) == 0)
            return cmd;
    }
    return NULL;
}

static char *
strip_whitespace(char *string)
{
    char *s, *e;

    for (s = string; isspace(*s); s++);
        ;
    if (!*s) 
        return (s);
    for (e = s + strlen(s) - 1; e > s && isspace(*e); e--)
        ;
    *++e = '\0';
    return s;
}

#ifdef HAVE_LIBREADLINE
static char *
completion_cmd(const char *text, int state)
{
    static unsigned int i;
    static size_t len;
    char *name;

    if (!state)
    {
        i = 0;
        len = strlen(text);
    }

    while ((name = commands[i].name))
    {
        i++;
        if (strncmp(name, text, len) == 0)
            return (ppmM_strdup(name));
    }
    return NULL;
}

static char **
completion(const char *text, int start, int end)
{
    char **matches;

    matches = NULL;
    if (start == 0)
        matches = rl_completion_matches(text, completion_cmd);
    return matches;
}
#endif

void
ppmC_init(void)
{
#ifdef HAVE_LIBREADLINE
    rl_readline_name = "ppm";
    rl_attempted_completion_function = completion;
#endif
}

const char *
getprompt(void)
{
    static char prompt[255];

    if (!ppm_usecolor) return PROMPT;
    sprintf(prompt, "%s%s%s", PPMC(CYAN), PROMPT, PPMC(NONE)); 
    return prompt;
}

char *
ppmC_getline(const char *prompt)
{
    char *line;
    char c;
    size_t len = 0;
    size_t size = 32;
    
    fprintf(stdout, "%s", prompt);
    line = ppmM_alloc(size);

    while ((c = fgetc(stdin)) != EOF && c != '\n')
    {
        line[len++] = c;
        if (len < size) continue;
        size += 32;
        line = ppmM_realloc(line, size);
    }
    line[len] ='\0';
    return line;
}

char *
ppmC_readline(void)
{
    if (line) free(line);

#ifdef HAVE_LIBREADLINE
    line = readline(getprompt());
    if (line && *line)
        add_history(line);
#else
    line = getline(getprompt());
#endif
    return strip_whitespace(line);
}

unsigned int
ppmC_command(int argc, char **args)
{
    Command *cmd;

    cmd = find_command(*args);
    if (!cmd)
    {
        ppm_error("unkown command: %s, type 'help' for a list of commands", *args);
        return 0;
    }
    
    if (cmd->argc >= 0 && argc != cmd->argc)
    {
        ppm_error("'%s' expects %d arguments, %d given", cmd->name, cmd->argc, argc);
        return 0;
    }
        
    return cmd->f(argc, args + 1);
}

unsigned int
ppmC_eval(const char *line)
{
    char **args;
    size_t argc = 0; 
    size_t i = 0; 
    size_t len;
    unsigned int ret = 1;

    args = ppmM_alloc(sizeof(char *));
    len = strlen(line) + 1;
    args[0] = ppmM_alloc(len);

    for (; *line; line++)
    {
        if (*line == '\n') continue;
        if (*line != ' ')
        {
            args[argc][i++] = *line;
            continue;
        }
        
        args[argc][i] = '\0';
        argc++;
        i = 0;
        args = ppmM_realloc(args, sizeof(char *) * (argc + 1));
        args[argc] = ppmM_alloc(len);
        while (line[1] == ' ') line++;
    }

    args[argc][i] = '\0';
    ret = ppmC_command(argc, args);
    for (i = 0; i <= argc; i++)
        free(args[i]);
    free(args);
    return ret;
}

