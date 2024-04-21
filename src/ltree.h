// forget about this LOL

#pragma once

typedef struct ltree {
        int h, w;
} ltree_t;

void ltree_build(ltree *lt, int h, int w, int **data);

// frees the ltree
void ltree_burn(ltree *lt);
