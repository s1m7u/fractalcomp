#ifndef TILE_H
#define TILE_H

#include "config.h"

#include "constants.h"

typedef struct struct_tile {
    unsigned int vals[TILE_SIZE][TILE_SIZE]; // 4 x 4
} tile;

#endif
