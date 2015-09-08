/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. datafl4sh.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>

#ifndef USE_STATIC_MEMORY
#include <stdlib.h>
#endif

#include "buffer.h"

struct circular_buffer {
    size_t      size;
    size_t      start, stop;
#ifdef USE_STATIC_MEMORY
    buffer_element_t    elements[BUFFER_SIZE];
#else
    buffer_element_t    *elements;
#endif
};

#ifdef USE_STATIC_MEMORY
static struct circular_buffer buf;
#endif

struct circular_buffer *
#ifdef USE_STATIC_MEMORY
buffer_init()
#else
buffer_init(size_t size)
#endif
{
    struct circular_buffer *b;
    
#ifdef USE_STATIC_MEMORY
    b = &buf;
#else
    b = (struct circular_buffer *) malloc(sizeof(struct circular_buffer));
    if (!b)
    {
        perror("malloc");
        return NULL;
    }
#endif
    
#ifdef USE_STATIC_MEMORY
    b->size = BUFFER_SIZE+1;
#else
    b->size = size+1;
#endif
    
    b->start = 0;
    b->stop = 0;
    
#ifndef USE_STATIC_MEMORY
    b->elements = (buffer_element_t *) calloc(b->size, sizeof(buffer_element_t));
    if (!b->elements)
    {
        perror("calloc");
        free(b);
        return NULL;
    }
#endif
    
    return b;
}


void
buffer_destroy(struct circular_buffer *b)
{
#ifdef USE_STATIC_MEMORY
    (void) b;
#else
    free(b->elements);
    free(b);
#endif
}


int
buffer_empty(struct circular_buffer *b)
{
    return b->stop == b->start;
}

int
buffer_full(struct circular_buffer *b)
{
    return ((b->stop + 1) % b->size) == b->start;
}

void
buffer_write(struct circular_buffer *b, buffer_element_t elem)
{
    b->elements[b->stop] = elem;
    b->stop = (b->stop + 1) % b->size;
    if (b->stop == b->start)
        b->start = (b->start + 1) % b->size;
}

void
buffer_read(struct circular_buffer *b, buffer_element_t *elem)
{
    *elem = b->elements[b->start];
    b->start = (b->start + 1) % b->size;
}

void
buffer_print(struct circular_buffer *b)
{
    size_t i;
    
    printf("size: %d, start: %d, stop: %d\n", b->size, b->start, b->stop);

    for(i = 0; i < b->size; i++)
        printf("%03x ", b->elements[i]);
    printf("\n");
}



