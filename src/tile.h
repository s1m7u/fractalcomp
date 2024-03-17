#ifndef TILE_H
#define TILE_H

#include "constants.h"
#include "config.h"

typedef struct struct_tile {
    unsigned int vals[TILE_SIZE][TILE_SIZE]; // 4 x 4
} tile;

#endif
