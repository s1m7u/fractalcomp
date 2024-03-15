#ifndef UTILITY_H
#define UTILITY_H

#include <math.h>
#include "constants.h"
#include "structs.h"
#include "globals.h"

void init_image();
void print_encoding();
void print_image(const char *s, tile image_to_print[NUM_TILES][NUM_TILES]);
void compare(const char *original, const char *decoded);
void import_image(const char* s);

void save_image(const char *s, tile image_to_print[NUM_TILES][NUM_TILES]);

void tile_stats_calc(tile* t);
inline int& yx_to_val(int y, int x, tile i[NUM_R_BLOCKS][NUM_R_BLOCKS]) {
    return i[y/TILE_SIZE][x/TILE_SIZE].vals[y%TILE_SIZE][x%TILE_SIZE];
}
tile& yx_to_tile(int y, int x, tile i[NUM_R_BLOCKS][NUM_R_BLOCKS]);

inline void calc_pos(int i, int j, int* y, int* x, int orient) {
    i++;
    j++;
    *y = orientations[orient][0][0] * i + orientations[orient][0][1] * j;
    *x = orientations[orient][1][0] * i + orientations[orient][1][1] * j;
    *y = (*y + TILE_SIZE+1)% (TILE_SIZE+1);
    *x = (*x + TILE_SIZE+1)% (TILE_SIZE+1);
    *y -= 1;
    *x -= 1;
}


#endif
