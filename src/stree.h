#pragma once

#include <cstddef>

// suffix tree UwU

typedef unsigned char u8;

typedef struct stree {
        struct stree *children[1 << 8];
        struct stree *parent;
        struct stree *suffix;
        int start, end;
} stree_t; // street OwO

void stree_build(stree_t *st, const u8 *data, size_t n);

void stree_burn(stree_t *st);
