#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// Include any other headers we need here

// NOTE: You should NOT include <stdlib.h> in your final implementation.
// malloc.h turns every call to malloc/calloc/free in this file into a call
// to mymalloc/mycalloc/myfree, so calling them here would recurse forever.

#include <debug.h> // definition of debug_printf

typedef struct block {
    size_t size;
    struct block *next;
    int free;
} block_t;

static block_t *head = NULL;
static block_t *tail = NULL;

static block_t *find_free_block(size_t size) {
    block_t *current = head;

    while (current != NULL) {
        if (current->free == 1 && current->size >= size) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

static block_t *request_block(size_t size) {
    if (size > (size_t)INTPTR_MAX - sizeof(block_t)) {
        return NULL;
    }

    void *memory = sbrk(sizeof(block_t) + size);
    if (memory == (void *)-1) {
        return NULL;
    }

    block_t *block = memory;
    block->size = size;
    block->next = NULL;
    block->free = 0;

    if (head == NULL) {
        head = block;
    } else {
        tail->next = block;
    }
    tail = block;

    return block;
}

void *mymalloc(size_t s) {
    if (s == 0) {
        return NULL;
    }

    debug_printf("Malloc %zu bytes\n", s);

    void *p = NULL; // TODO: find the first free block that fits, or request a
                    // new one with sbrk. Determine how you will request memory :)

    block_t *block = find_free_block(s);
    if (block == NULL) {
        block = request_block(s);
    }

    if (!block) {
        // We are out of memory
        // if sbrk fails (returns (void *) -1)
        return NULL;
    }

    block->free = 0;
    p = (void *)(block + 1);
    return p;
}

void *mycalloc(size_t nmemb, size_t s) {
    if (s != 0 && nmemb > ((size_t)-1) / s) {
        return NULL;
    }

    size_t total = nmemb * s;
    void *p = mymalloc(total); // TODO: get the memory from mymalloc, then set it to 0

    if (!p) {
        // We are out of memory
        // if we get NULL back from mymalloc
        debug_printf("Calloc %zu bytes\n", total);
        return NULL;
    }

    memset(p, 0, total);
    debug_printf("Calloc %zu bytes\n", total);
    return p;
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    block_t *block = (block_t *)ptr - 1;
    size_t size = block->size; // TODO: the size stored in ptr's block header

    // TODO: mark ptr's block as free
    assert(block->free == 0);
    block->free = 1;

    debug_printf("Freed %zu bytes\n", size);
}
