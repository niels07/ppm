/*
 * ppm_table.c
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

#include <stdlib.h>
#include <string.h>
#include "ppm_table.h"
#include "ppm.h"
#include "ppm_mem.h"

#define TABLE_GROW 32

static unsigned int
hash(const char *string)
{
    unsigned int hashval = 0;

    while (*string != '\0')
        hashval = ((hashval << 5) + hashval) + *string++;

    return hashval;
}

static ppm_Node *
newnode(const char *key, const char *value)
{
    ppm_Node *node;

    node = NEW(ppm_Node);
    node->key  = ppmM_strdup(key);
    node->value = ppmM_strdup(value);
    node->next = NULL;
    return node;
}

ppm_Table *
ppmT_new(size_t size)
{
    ppm_Table *table;
    
    table = NEW(ppm_Table);
    table->nodes = calloc(size, sizeof(ppm_Node *));
    table->size = size;
    table->count = 0;

    return table;
}

ppm_Table *
ppmT_resize(ppm_Table *table, size_t size)
{
    ppm_Table *ntable;
    unsigned int i;

    ntable = ppmT_new(size);
    ntable->count = table->count;

    for (i = 0; i < table->size; i++) 
    {
        ppm_Node *node = table->nodes[i];
        unsigned int hashkey;
            
        if (!node) continue;
        hashkey = hash(node->key) % size;
        ntable->nodes[hashkey] = node;
    }
    free(table->nodes);
    free(table);
    return ntable;
}

void
ppmT_insert(ppm_Table *table, const char *key, const char *value)
{
    unsigned int hashkey;
    ppm_Node *node;
        
    if (table->count == table->size)
        table = ppmT_resize(table, TABLE_GROW);

    hashkey = hash(key) % table->size;
    node = table->nodes[hashkey];

    while (node && node->next)
    {
        if (strcmp(node->key, key) == 0) 
        {
            free(node->value);
            node->value = ppmM_strdup(value);
            return;
        }
        node = node->next;
    }
    
    node = newnode(key, value);
    node->next = table->nodes[hashkey];

    table->count++;
    table->nodes[hashkey] = node;
    return;
}

static void
freenode(ppm_Node *node)
{
    free(node->key);
    free(node->value);
    free(node);
}

char *
ppmT_remove(ppm_Table *table, const char *key)
{
    ppm_Node *node, *prev = NULL;
    unsigned int hashkey;
    char *value = NULL;

    hashkey = hash(key) % table->size;
    node = table->nodes[hashkey];

    while (node)
    {
        if (strcmp(node->key, key) != 0) 
        {
            prev = node;
            node = node->next;
            continue;
        }

        if (prev)
            prev->next = node->next;
        else
            table->nodes[hashkey] = node->next;
        
        value = node->value;
        freenode(node);
        break;
    }
    return value;
}

void
ppmT_free(ppm_Table *table)
{
    unsigned int i;
    ppm_Node *node, *prev;
    
    if (!table) return;
    for (i = 0; i < table->size; i++)
    {
        node = table->nodes[i];
        while (node)
        {
            prev = node;
            node = node->next;
            freenode(prev);
        }
    }
    
    free(table->nodes);
    free(table);
}

ppm_Node *
ppmT_getnode(ppm_Table *table, const char *key)
{
    unsigned int hashkey;
    ppm_Node *node;

    if (!table) return NULL;
    hashkey = hash(key) % table->size;
    node = table->nodes[hashkey];

    while (node)
    {
        if (strcmp(node->key, key) == 0)
            return node;
        else
            node = node->next;
    }
    return NULL;
}

char *
ppmT_get(ppm_Table *table, const char *key)
{
    ppm_Node *node;

    node = ppmT_getnode(table, key);
    return (node) ? node->value : NULL;
}
