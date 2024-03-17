#ifndef UTILITY_H
#define UTILITY_H

#include <math.h>

#include "config.h"
#include "constants.h"
#include "tile.h"

void init_image();
void print_encoding();
void print_image(const char *s, tile image_to_print[NUM_TILES][NUM_TILES]);
void compare(const char *original, const char *decoded);
void import_image(const char* s);

void save_image(const char *s, tile image_to_print[NUM_TILES][NUM_TILES]);

inline unsigned int& yx_to_val(int y, int x, tile i[NUM_R_BLOCKS][NUM_R_BLOCKS]) {
    return i[y/TILE_SIZE][x/TILE_SIZE].vals[y%TILE_SIZE][x%TILE_SIZE];
}
inline void calc_pos(unsigned int* y, unsigned int* x, unsigned int orient, unsigned int size) {
    *y += 1;
    *x += 1;
    *y = orientations[orient][0][0] * *y + orientations[orient][0][1] * *x;
    *x = orientations[orient][1][0] * *y + orientations[orient][1][1] * *x;
    *y = (*y + size+1)% (size+1);
    *x = (*x + size+1)% (size+1);
    *y -= 1;
    *x -= 1;
}


#endif
