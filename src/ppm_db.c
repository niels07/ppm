/*
 * ppm_db.c
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
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>

#include "ppm_db.h"
#include "ppm.h"
#include "ppm_aes.h"
#include "ppm_mem.h"
#include "ppm_table.h"
#include "ppm_string.h"

static char *dbpath;
static ppm_Table *dbtable;

static char *
gethome(void)
{
    char *home;
    struct passwd *pwd; 
    
    home = getenv("HOME");
    if (home) return home;
    pwd = getpwuid(getuid());
    if (!pwd)
    {
        ppm_error("failed to find home directory");
        return NULL;
    }
    return pwd->pw_dir;
}

static void
parsedbstr(char *string)
{
    ppm_String pass, app;
    unsigned int getp = 0;
    char *p;

    ppmS_init(&pass, NULL);
    ppmS_init(&app, NULL);
    for (p = string; *p; p++)
    {
        if (*p == '\n')
        {
            ppmT_insert(dbtable, app.cstr, pass.cstr);
            if (!p[1]) break;
            ppmS_set(&pass, NULL);
            ppmS_set(&app, NULL);
            getp = 0;
            continue;
        }
        if (*p == '\t')
        {
            getp = 1;
            continue;
        }
        ppmS_addch(getp ? &pass : &app, *p);
    }
    free(pass.cstr);
    free(app.cstr);
}

static void
readdb(FILE *dbfile)
{
    size_t fsize;
    char *buffer;
    size_t bytes;
    char *dbtext;

    fseek(dbfile, 0 , SEEK_END);
    fsize = ftell(dbfile);
    rewind(dbfile);

    buffer = ppmM_alloc(fsize);
    bytes = fread(buffer, 1, fsize, dbfile);
    if (bytes < 0)
    {
        free(buffer);
        ppm_error("read failed");
        return;
    }
    else
    if (bytes == 0)
    {
        free(buffer);
        return;
    }
    dbtext = ppmA_decrypt(buffer, bytes);
    free(buffer);
    parsedbstr(dbtext);
    free(dbtext);
}

unsigned int
ppmD_save(void)
{
    unsigned int i;
    ppm_String dbtext;
    char *buffer;
    FILE *dbfile;
    size_t len;

    if (!dbtable) return 0;
    if (dbtable->count == 0) return 1;
    dbfile = fopen(dbpath, "wb");
    if (!dbfile)
    {
        ppm_error("failed to open %s", dbpath);
        return 0;
    }
    ppmS_init(&dbtext, NULL);
    for (i = 0; i < dbtable->size; i++)
    {
        ppm_Node *node = dbtable->nodes[i];
        while (node)
        {
            ppmS_append(&dbtext, node->key);
            ppmS_addch(&dbtext, '\t');
            ppmS_append(&dbtext, node->value);
            ppmS_addch(&dbtext, '\n');
            node = node->next;
        }
    }
    buffer = ppmA_encrypt(dbtext.cstr, &len);
    free(dbtext.cstr);
    
    if (fwrite(buffer, 1, len, dbfile) < 0)
    {
        free(buffer);
        fclose(dbfile);
        ppm_error("failed to write to file");
        return 0;
    }
    fclose(dbfile);
    free(buffer);
    return 1;
}

unsigned int 
ppmD_init(void)
{
    FILE *dbfile = NULL;
    char *home;

    if (!ppm_dbfile)
    {
        home = gethome();
        if (!home) return 0;
        
        dbpath = ppmM_alloc(strlen(home) + 6); 
        sprintf(dbpath, "%s/.ppm", home);
    }
    else
        dbpath = ppm_dbfile;

    dbtable = ppmT_new(32);
    dbfile = fopen(dbpath, "rb");
    if (!dbfile) return 1;
    
    readdb(dbfile);
    fclose(dbfile);
    return 1;
}

void
ppmD_add(const char *app, const char *pass)
{
    if (!dbtable) return;
    if (ppmT_get(dbtable, app))
    {
        ppm_error("'%s%s%s' already exists, use '%supdate%s' to change the password", 
                 PPMC(WHITE), app, PPMC(RED), 
                 PPMC(WHITE), PPMC(RED));
        return;
    }
    ppmT_insert(dbtable, app, pass);
    ppm_message("'%s%s%s' added", PPMC(WHITE), app, PPMC(GREEN));
}

void
ppmD_update(const char *app, const char *pass)
{
    ppm_Node *node;
    
    if (!dbtable) return;
    node = ppmT_getnode(dbtable, app);
    if (!node)
    {
        ppm_error("%s%s%s not found, use '%sadd%s' to add a new user", 
                  PPMC(WHITE), app, PPMC(RED),
                  PPMC(WHITE), PPMC(RED));
        return;
    }

    free(node->value);
    node->value = ppmM_strdup(pass);
    ppm_message("'%s%s%s' updated", PPMC(WHITE), app, PPMC(GREEN));
}

void
ppmD_rm(const char *app)
{
    if (!dbtable) return;
    if (ppmT_remove(dbtable, app))
        ppm_message("'%s%s%s' deleted", PPMC(WHITE), app, PPMC(GREEN));
    else
        ppm_error("'%s%s%s' not found", PPMC(WHITE), app, PPMC(RED));
}

char *
ppmD_get(const char *app)
{
    if (!dbtable) return NULL;
    return ppmT_get(dbtable, app);
}

void
ppmD_list(void)
{
    unsigned int i;
    ppm_Node *node;
    
    if (!dbtable) return;
    for (i = 0; i < dbtable->size; i++)
    {
        node = dbtable->nodes[i];
        while (node) 
        {
            fprintf(stdout, "%s%s%s => %s%s%s\n", 
                    PPMC(WHITE), node->key,   PPMC(GREEN),
                    PPMC(BLUE),  node->value, PPMC(NONE));
            node = node->next;
        }
    }
}

void
ppmD_cleanup(void)
{
    ppmT_free(dbtable);
}
