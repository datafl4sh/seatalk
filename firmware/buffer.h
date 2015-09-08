/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <datafl4sh@toxicnet.eu> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. datafl4sh.
 * ----------------------------------------------------------------------------
 */

#pragma once

#define USE_STATIC_MEMORY
#define BUFFER_SIZE 64

typedef int buffer_element_t;


struct circular_buffer;

#ifdef USE_STATIC_MEMORY
struct circular_buffer *buffer_init(void);
#else
struct circular_buffer *buffer_init(size_t);
#endif

void buffer_destroy(struct circular_buffer *);
int buffer_empty(struct circular_buffer *);
int buffer_full(struct circular_buffer *);
void buffer_write(struct circular_buffer *, buffer_element_t);
void buffer_read(struct circular_buffer *, buffer_element_t *);
void buffer_print(struct circular_buffer *);
