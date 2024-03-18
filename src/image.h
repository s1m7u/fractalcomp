#ifndef IMAGE_H
#define IMAGE_H

#include "config.h"

#include "tile.h"

typedef struct struct_image {
    unsigned int height, width;
    unsigned int tileNum;
    tile* pixels;
    int* pfxSum;
    long int* pfxSumOfSq;
} image;

image* image_create(unsigned int height, unsigned int width);
void image_destroy(image* img);

image* image_from_png(const char *s);
void save_image(const char *s, image* img);

image* downsample_by_two(image* img);

inline unsigned int& yx_to_val(int y, int x, image* i) {
    return i->pixels[(y/TILE_SIZE)*(i->width/TILE_SIZE) + x/TILE_SIZE].vals[y%TILE_SIZE][x%TILE_SIZE];
}

void print_image(const char *s, image* img);
#endif
